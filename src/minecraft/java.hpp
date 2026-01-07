/*
 * Minecraft Engine
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 * 
 * @File: minecraft/java.hpp
 * @Description: For managing Java.
 * @Ownership: TaimWay <taimway@gmail.com> - 01/07/2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
            
            // Less-than operator for sorting
            bool operator<(const JavaInfo& other) const {
                return path < other.path;
            }
        };

        typedef std::vector<JavaInfo> JavaList;

        namespace internal {
            // Helper function to check if a path is a valid Java executable
            bool isValidJavaExecutable(const fs::path& javaPath);
            
            // Helper function to get Java version info from directory
            std::string getJavaVersionInfo(const fs::path& javaDir);
            
            // Helper function to get Java publisher/origin from directory
            std::string getJavaPublisher(const fs::path& javaDir);
            
            // Helper function to get Java structure (JRE/JDK) from directory
            std::string getJavaStructure(const fs::path& javaDir);
            
            // Helper function to scan directory for Java installations
            void scanDirectoryForJava(const fs::path& directory, JavaList& result, bool recursive = false);
            
            // Helper to convert Windows TCHAR to std::string
            #ifdef _WIN32
            std::string tcharToString(const TCHAR* tstr);
            #endif
            
            // Helper to get Java installation directory from executable path
            fs::path getJavaDirFromExecutable(const fs::path& javaExePath);
            
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