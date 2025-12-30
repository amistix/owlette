#include "Streaming.h"
#include "Destination.h"
#include "Identity.h"
#include <chrono>
#include "ClientContext.h"
#include <android/log.h>
#include <memory>
#include <string>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "DestinationClient", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DestinationClient", __VA_ARGS__))

// Store the destination so it stays alive
static std::shared_ptr<i2p::client::ClientDestination> g_localDest;

std::string startDestinationClient() {
    try {
        LOGI("Creating PUBLIC destination...");
        
        auto localDest = i2p::client::context.CreateNewLocalDestination(
            true,  // PUBLIC
            i2p::data::SIGNING_KEY_TYPE_EDDSA_SHA512_ED25519,
            i2p::data::CRYPTO_KEY_TYPE_ECIES_X25519_AEAD
        );
        
        if (!localDest) {
            LOGE("Failed to create destination!");
            return "";
        }
        
        std::string identity = localDest->GetIdentity()->ToBase64();
        std::string base32 = localDest->GetIdentHash().ToBase32() + ".b32.i2p";
        
        LOGI("Created destination (base32): %s", base32.c_str());
        LOGI("Created destination (base64): %s", identity.substr(0, 50).c_str());
        LOGI("Full length: %zu characters", identity.length());
        LOGI("Full base64: %s", identity.c_str());
        
        // IMPORTANT: Run each connection in its own thread!
        localDest->AcceptStreams([localDest](std::shared_ptr<i2p::stream::Stream> stream) {
            LOGI("Incoming connection!");
            
            // Handle in separate thread to avoid blocking
            std::thread([stream, localDest]() {
                try {
                    LOGI("Connection handler thread started");
                    
                    uint8_t buf[8192];
                    size_t len = stream->Receive(buf, sizeof(buf), 30);
                    
                    if (len > 0) {
                        std::string message(reinterpret_cast<char*>(buf), len);
                        LOGI("Received %zu bytes", len);
                        LOGI("Data: %s", message.substr(0, 200).c_str());
                        
                        // Build HTTP response if it looks like HTTP
                        if (message.find("GET") == 0 || message.find("POST") == 0) {
                            LOGI("Detected HTTP request");
                            
                            std::ostringstream response;
                            response << "HTTP/1.1 200 OK\r\n";
                            response << "Content-Type: text/html\r\n";
                            response << "Connection: close\r\n";
                            response << "\r\n";
                            response << "<!DOCTYPE html>\n";
                            response << "<html>\n";
                            response << "<head><title>Android i2pd</title></head>\n";
                            response << "<body>\n";
                            response << "<h1>Success! Connected to Android</h1>\n";
                            response << "<p>Your PC successfully reached this Android device via i2p!</p>\n";
                            response << "</body>\n";
                            response << "</html>\n";
                            
                            std::string responseStr = response.str();
                            stream->Send(
                                reinterpret_cast<const uint8_t*>(responseStr.c_str()),
                                responseStr.length()
                            );
                            
                            LOGI("Sent HTTP response");
                        } else {
                            // Echo back for non-HTTP
                            stream->Send(buf, len);
                            LOGI("Echoed back %zu bytes", len);
                        }
                    } else {
                        LOGI("Connection closed (no data)");
                    }
                    
                    LOGI("Connection handler finished");
                    
                } catch (const std::exception& e) {
                    LOGE("Error in handler: %s", e.what());
                }
            }).detach(); // Detach so thread runs independently
            
        });
        
        LOGI("AcceptStreams set up successfully");
        
        g_localDest = localDest;
        return identity;
        
    } catch (const std::exception& e) {
        LOGE("Exception: %s", e.what());
        return "";
    }
}

// Function to connect to another destination
bool connectToDestination(const std::string& remoteDestBase64) {
    try {
        if (!g_localDest) {
            LOGE("Local destination not created yet!");
            return false;
        }

        LOGI("Waiting for tunnels to be ready...");
        int attempts = 0;
        while (!g_localDest->IsReady() && attempts < 60) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            attempts++;
            if (attempts % 5 == 0) {
                LOGI("Still waiting for tunnels... (%d/60)", attempts);
            }
        }
        
        LOGI("Connecting to: %s", remoteDestBase64.substr(0, 50).c_str());
        
        // Parse remote destination
        i2p::data::IdentityEx remoteIdentity;
        if (!remoteIdentity.FromBase64(remoteDestBase64)) {
            LOGE("Invalid destination format!");
            return false;
        }
        
        LOGI("Remote identity parsed successfully");
        
        // Create stream
        LOGI("Creating stream...");
        auto stream = g_localDest->CreateStream(remoteIdentity.GetIdentHash());
        
        if (!stream) {
            LOGE("Failed to create stream!");
            return false;
        }
        
        LOGI("Stream created successfully!");
        
        // Send test message
        std::string testMsg = "Hello from startDestinationClient!";
        stream->Send(
            reinterpret_cast<const uint8_t*>(testMsg.c_str()),
            testMsg.length()
        );
        
        LOGI("Sent test message: %s", testMsg.c_str());
        
        // Wait for response
        uint8_t buf[1024];
        size_t len = stream->Receive(buf, sizeof(buf), 30);
        
        if (len > 0) {
            std::string response(reinterpret_cast<char*>(buf), len);
            LOGI("Received response (%zu bytes): %s", len, response.c_str());
        }
        
        return true;
        
    } catch (const std::exception& e) {
        LOGE("Exception in connectToDestination: %s", e.what());
        return false;
    }
}

// Cleanup
void stopDestinationClient() {
    LOGI("Stopping destination client...");
    if (g_localDest) {
        g_localDest.reset();
    }
    LOGI("Destination client stopped");
}