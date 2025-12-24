#pragma once
#include <string>

namespace storage {

    inline std::string& getAppStoragePath() {
        static std::string path;
        return path;
    }

    inline void setAppStoragePath(const std::string& path) {
        getAppStoragePath() = path;
    }
}