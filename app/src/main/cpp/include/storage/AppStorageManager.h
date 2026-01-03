#pragma once
#include <string>
#include <jni.h>

namespace storage {
    inline std::string& getAppStoragePath() {
        static std::string path;
        return path;
    }
    
    inline void setAppStoragePath(const std::string& path) {
        getAppStoragePath() = path;
    }
    
    // ✅ Add these
    void initAppStorageManagerClass(JNIEnv* env);
    void cleanupAppStorageManagerClass(JNIEnv* env);

    void writePeer(const std::string& peerIdentity);
    std::string readPeer();
    
    void writeDest(const std::string& content);
    std::string readDest();
}