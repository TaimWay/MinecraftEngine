#pragma once
#ifndef __MINECRAFT_ENGINE__JAVA_HPP__
#define __MINECRAFT_ENGINE__JAVA_HPP__

#include <minecraft/cntconfig.hpp>

#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <pwd.h>
#include <wordexp.h>
#endif

namespace cnt {
    namespace minecraft {
        struct JavaInfo {
            String name;
            String publisher;
            String structure;
            fs::path path;

            JavaInfo() = default;
            JavaInfo(String _$name, String _$publisher, String _$structure, fs::path _$path);
            
            // Operator to compare JavaInfo objects
            bool operator==(const JavaInfo& other) const;
        };

        typedef std::vector<JavaInfo> JavaList;

        namespace internal {
            // Helper function to check if a path is a valid Java executable
            bool isValidJavaExecutable(const fs::path& javaPath);
            
            // Helper function to get Java version info from executable
            std::string getJavaVersionInfo(const fs::path& javaPath);
            
            // Helper function to get Java publisher/origin
            std::string getJavaPublisher(const fs::path& javaPath);
            
            // Helper function to get Java structure (JRE/JDK)
            std::string getJavaStructure(const fs::path& javaPath);
            
            // Helper function to scan directory for Java installations
            void scanDirectoryForJava(const fs::path& directory, JavaList& result, bool recursive = false);
            
            // Helper to convert Windows TCHAR to std::string
            #ifdef _WIN32
            std::string tcharToString(const TCHAR* tstr);
            #endif
            
            // Platform-specific common Java installation locations
            std::vector<fs::path> getCommonJavaLocations();
            
            // Platform-specific deep search locations
            std::vector<fs::path> getDeepSearchLocations();
            
            // Check PATH environment variable for Java executables
            void checkPathForJava(JavaList& result);
        }

        /**
         * Quick search for Java installations in common locations
         * This function searches in standard installation directories
         * @return JavaList containing found Java installations
         */
        JavaList SearchJava$Quick();

        /**
         * Deep search for Java installations in both common and additional locations
         * This function performs a more thorough search including recursive scanning
         * in some locations
         * @return JavaList containing found Java installations
         */
        JavaList SearchJava$Deep();
    }
}

#endif // !__MINECRAFT_ENGINE__JAVA_HPP__