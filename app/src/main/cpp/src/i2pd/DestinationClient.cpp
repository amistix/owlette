#include "Streaming.h"
#include "Destination.h"
#include "Identity.h"
#include <chrono>
#include "ClientContext.h"
#include <android/log.h>
#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include "ui/View.h"
#include "ui/TextView.h"
#include "extra/vec4.h"

#include "storage/AppStorageManager.h"

// ✅ Store active stream globally for sending
static std::shared_ptr<i2p::stream::Stream> g_activeStream;
static std::string g_activePeerIdentity;

// ✅ Logcat-only macros
#define LOGI_LOGCAT(...) ((void)__android_log_print(ANDROID_LOG_INFO, "DestinationClient", __VA_ARGS__))
#define LOGE_LOGCAT(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DestinationClient", __VA_ARGS__))

// Store the destination so it stays alive
static std::shared_ptr<i2p::client::ClientDestination> g_localDest;

extern ui::View* rootView;
extern void attachMessage(const std::string& authorName, const std::string& text, bool ownMessage);

bool connectToDestination(const std::string& remoteDestBase64);

// ✅ Combined logging functions - log to both logcat AND UI
void LOGI(const std::string& content)
{
    LOGI_LOGCAT("%s", content.c_str());
    
    if (!rootView) return;
    
    auto& children = rootView->getChildren();
    if (children.empty()) return;
    
    auto& topBarChildren = children[0]->getChildren();
    if (topBarChildren.empty()) return;
    
    ui::TextView* topBarText = static_cast<ui::TextView*>(topBarChildren[0]);
    if (!topBarText) return;
    
    topBarText->setText(content);
}

void LOGE(const std::string& content)
{
    LOGE_LOGCAT("%s", content.c_str());
    
    if (!rootView) return;
    
    auto& children = rootView->getChildren();
    if (children.empty()) return;
    
    auto& topBarChildren = children[0]->getChildren();
    if (topBarChildren.empty()) return;
    
    ui::TextView* topBarText = static_cast<ui::TextView*>(topBarChildren[0]);
    if (!topBarText) return;
    
    topBarText->setText("ERROR: " + content);
    // topBarText->setColorText(vec4<float>(1.0f, 0.0f, 0.0f, 1.0f));
}

// ✅ Bidirectional stream handler - no timeouts, continuous receive
void handleStream(std::shared_ptr<i2p::stream::Stream> stream, 
                  std::shared_ptr<i2p::client::ClientDestination> localDest,
                  const std::string& peerIdentity) {
    
    try {
        LOGI_LOGCAT("Stream handler started for peer: %s", peerIdentity.substr(0, 20).c_str());
        g_activeStream = stream;
        
        while (true) {
            uint8_t buf[8192];
            
            // ✅ 30 second timeout - waits up to 30s for data
            size_t len = stream->Receive(buf, sizeof(buf), 30);
            
            if (len > 0) {
                std::string message(reinterpret_cast<char*>(buf), len);
                LOGI_LOGCAT("Received %zu bytes: %s", len, message.c_str());
                
                // Add to chat UI
                attachMessage(peerIdentity.substr(0, 20), message, false);
                
            } else {
                // Check if stream is still connected
                // i2p streams don't have a simple "isConnected" check,
                // but we can try to send a 0-byte packet
                try {
                    // Just continue - timeout doesn't mean disconnected
                    LOGI_LOGCAT("No data received, waiting...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } catch (...) {
                    LOGI("Stream closed");
                    break;
                }
            }
        }
        
        LOGI_LOGCAT("Stream handler finished");
        
    } catch (const std::exception& e) {
        LOGE_LOGCAT("Stream handler error: %s", e.what());
    }
}

std::string startDestinationClient() {
    try {
        LOGI("Creating PUBLIC destination...");
        
        auto localDest = i2p::client::context.CreateNewLocalDestination(
            true,
            i2p::data::SIGNING_KEY_TYPE_EDDSA_SHA512_ED25519,
            i2p::data::CRYPTO_KEY_TYPE_ECIES_X25519_AEAD
        );
        
        if (!localDest) {
            LOGE("Failed to create destination!");
            return "";
        }
        
        std::string identity = localDest->GetIdentity()->ToBase64();
        std::string base32 = localDest->GetIdentHash().ToBase32() + ".b32.i2p";
        
        LOGI("Created destination");
        LOGI_LOGCAT("Base32: %s", base32.c_str());
        LOGI_LOGCAT("Base64: %s", identity.substr(0, 50).c_str());
        LOGI_LOGCAT("Full base64: %s", identity.c_str());
        
        // ✅ Accept incoming connections with bidirectional handler
        localDest->AcceptStreams([localDest](std::shared_ptr<i2p::stream::Stream> stream) {
            LOGI("Incoming connection!");
            
            auto remoteIdentity = stream->GetRemoteIdentity();
            std::string peerDest = remoteIdentity ? remoteIdentity->ToBase64() : "Unknown";
            std::string peerBase32 = remoteIdentity ? 
                (remoteIdentity->GetIdentHash().ToBase32() + ".b32.i2p") : "Unknown";
            
            LOGI_LOGCAT("Peer: %s", peerBase32.c_str());
            
            // ✅ Save peer for reconnection
            // if (remoteIdentity) {
            //     storage::writePeer(peerDest);
            // }
            
            // ✅ Handle stream in separate thread with continuous receive
            std::thread([stream, localDest, peerDest]() {
                handleStream(stream, localDest, peerDest);
            }).detach();
        });
        
        LOGI("Listener started");
        
        g_localDest = localDest;
        
        LOGI("Waiting for tunnels...");
        int attempts = 0;
        std::string spinningSymbol = "/-\\|";
        
        while (!g_localDest->IsReady() && attempts < 60) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            attempts++;
            LOGI(std::string(1, spinningSymbol[attempts % 4]) + " Waiting... " + std::to_string(attempts) + "s");
        }
        
        if (!g_localDest->IsReady()) {
            LOGE("Tunnels not ready after 60s!");
            return "";
        }
        
        LOGI("Tunnels ready!");

        storage::writeDest(identity);
        LOGI_LOGCAT("Destination saved to storage");
        
        // ✅ Auto-connect to last peer
        std::string lastPeer = storage::readPeer();
        if (!lastPeer.empty() && lastPeer != identity) {
            LOGI("Auto-connecting to last peer...");
            std::thread([lastPeer]() {
                std::this_thread::sleep_for(std::chrono::seconds(2));
                connectToDestination(lastPeer);
            }).detach();
        }
        else storage::writePeer("");
        
        return identity;
        
    } catch (const std::exception& e) {
        LOGE(std::string("Exception: ") + e.what());
        return "";
    }
}


// ✅ Connect and keep stream alive for bidirectional communication
bool connectToDestination(const std::string& remoteDestBase64) {
    try {
        if (!g_localDest) {
            LOGE("Local destination not created!");
            return false;
        }

        if (!g_localDest->IsReady()) {
            LOGE("Tunnels not ready!");
            return false;
        }
        
        LOGI("Connecting...");
        LOGI_LOGCAT("Target: %s", remoteDestBase64.substr(0, 50).c_str());
        
        i2p::data::IdentityEx remoteIdentity;
        if (!remoteIdentity.FromBase64(remoteDestBase64)) {
            LOGE("Invalid destination format!");
            return false;
        }
        
        LOGI_LOGCAT("Remote identity parsed");
        
        LOGI("Creating stream...");
        auto stream = g_localDest->CreateStream(remoteIdentity.GetIdentHash());
        
        if (!stream) {
            LOGE("Failed to create stream!");
            return false;
        }
        
        LOGI("Stream created!");
        
        // ✅ Store stream globally for sending messages
        g_activeStream = stream;
        g_activePeerIdentity = remoteDestBase64;
        
        // ✅ Save peer
        // storage::writePeer(remoteDestBase64);
        
        // ✅ Start bidirectional handler in background
        std::thread([stream, remoteDestBase64]() {
            handleStream(stream, g_localDest, remoteDestBase64);
        }).detach();
        
        LOGI("Connected! Chat active.");
        
        return true;
        
    } catch (const std::exception& e) {
        LOGE(std::string("Connect error: ") + e.what());
        return false;
    }
}

// ✅ Send message through active stream
bool sendMessage(const std::string& message) {
    if (!g_activeStream) {
        LOGE("No active connection!");
        return false;
    }
    
    try {
        g_activeStream->Send(
            reinterpret_cast<const uint8_t*>(message.c_str()),
            message.length()
        );
        
        LOGI_LOGCAT("Sent: %s", message.c_str());
        
        // ✅ Add to UI as own message
        attachMessage("You", message, true);
        
        return true;
    } catch (const std::exception& e) {
        LOGE(std::string("Send error: ") + e.what());
        return false;
    }
}

void stopDestinationClient() {
    LOGI("Stopping client...");
    
    if (g_activeStream) {
        g_activeStream.reset();
        g_activeStream = nullptr;
    }
    
    if (g_localDest) {
        g_localDest.reset();
    }
    
    LOGI_LOGCAT("Destination client stopped");
}