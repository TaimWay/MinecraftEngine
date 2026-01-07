/*
 * Minecraft Engine
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 * 
 * @File: source/java.cpp
 * @Description: The implementation of minecraft/java.hpp
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

#include <minecraft/java.hpp>

namespace cnt {
    namespace minecraft {
        JavaInfo::JavaInfo(String _$name, String _$publisher, String _$structure, fs::path _$path) :
            name(_$name), publisher(_$publisher), structure(_$structure), path(_$path) {}
        
        bool JavaInfo::operator==(const JavaInfo& other) const {
            return path == other.path;
        }

        namespace internal {
            // Helper function to check if a path is a valid Java executable
            bool isValidJavaExecutable(const fs::path& javaPath) {
                if (!fs::exists(javaPath) || !fs::is_regular_file(javaPath)) {
                    return false;
                }
                
                // Check file extension
                #ifdef _WIN32
                std::string ext = javaPath.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), 
                              [](unsigned char c){ return std::tolower(c); });
                if (ext != ".exe") {
                    return false;
                }
                #endif
                
                // Check if filename contains "java" (case-insensitive)
                std::string filename = javaPath.filename().string();
                std::transform(filename.begin(), filename.end(), filename.begin(), 
                              [](unsigned char c){ return std::tolower(c); });
                
                return filename.find("java") != std::string::npos;
            }

            // Helper function to get Java version info from directory
            std::string getJavaVersionInfo(const fs::path& javaDir) {
                // Use the directory name as version info
                return javaDir.filename().string();
            }

            // Helper function to get Java publisher/origin from directory
            std::string getJavaPublisher(const fs::path& javaDir) {
                std::string pathStr = javaDir.string();
                std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(),
                              [](unsigned char c){ return std::tolower(c); });
                
                // Check parent directories for publisher info
                std::vector<std::string> pathParts;
                fs::path currentPath = javaDir;
                
                for (int i = 0; i < 3 && !currentPath.empty(); i++) {
                    std::string part = currentPath.filename().string();
                    std::transform(part.begin(), part.end(), part.begin(),
                                  [](unsigned char c){ return std::tolower(c); });
                    pathParts.push_back(part);
                    currentPath = currentPath.parent_path();
                }
                
                // Check for known publishers in path parts
                for (const auto& part : pathParts) {
                    if (part.find("oracle") != std::string::npos) {
                        return "Oracle";
                    } else if (part.find("openjdk") != std::string::npos) {
                        return "OpenJDK";
                    } else if (part.find("adoptopenjdk") != std::string::npos) {
                        return "AdoptOpenJDK";
                    } else if (part.find("adoptium") != std::string::npos) {
                        return "Adoptium";
                    } else if (part.find("amazon") != std::string::npos || 
                               part.find("corretto") != std::string::npos) {
                        return "Amazon Corretto";
                    } else if (part.find("azul") != std::string::npos || 
                               part.find("zulu") != std::string::npos) {
                        return "Azul Zulu";
                    } else if (part.find("microsoft") != std::string::npos) {
                        return "Microsoft";
                    } else if (part.find("bellsoft") != std::string::npos || 
                               part.find("liberica") != std::string::npos) {
                        return "BellSoft Liberica";
                    } else if (part.find("graalvm") != std::string::npos) {
                        return "GraalVM";
                    } else if (part.find("java") != std::string::npos) {
                        return "Java";
                    }
                }
                
                return "Unknown";
            }

            // Helper function to get Java structure (JRE/JDK) from directory
            std::string getJavaStructure(const fs::path& javaDir) {
                std::string dirName = javaDir.filename().string();
                std::transform(dirName.begin(), dirName.end(), dirName.begin(),
                              [](unsigned char c){ return std::tolower(c); });
                
                if (dirName.find("jdk") != std::string::npos) {
                    return "JDK";
                } else if (dirName.find("jre") != std::string::npos) {
                    return "JRE";
                }
                
                // Check for typical JDK directories
                #ifdef _WIN32
                if (fs::exists(javaDir / "bin" / "javac.exe")) {
                    return "JDK";
                }
                #else
                if (fs::exists(javaDir / "bin" / "javac")) {
                    return "JDK";
                }
                #endif
                
                // Default to JRE if not sure
                return "JRE";
            }

            // Helper to get Java installation directory from executable path
            fs::path getJavaDirFromExecutable(const fs::path& javaExePath) {
                // Go up one level from bin directory
                if (javaExePath.has_parent_path()) {
                    fs::path binDir = javaExePath.parent_path();
                    if (binDir.filename() == "bin" && binDir.has_parent_path()) {
                        return binDir.parent_path();
                    }
                }
                // If not in bin directory, return parent directory
                return javaExePath.parent_path();
            }

            // Helper function to scan directory for Java installations
            void scanDirectoryForJava(const fs::path& directory, JavaList& result, bool recursive) {
                if (!fs::exists(directory) || !fs::is_directory(directory)) {
                    return;
                }
                
                try {
                    if (recursive) {
                        // Recursive scan - look for Java installation directories
                        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                            if (fs::is_directory(entry.path())) {
                                // Check if this is a Java installation directory
                                #ifdef _WIN32
                                fs::path javaExe = entry.path() / "bin" / "java.exe";
                                #else
                                fs::path javaExe = entry.path() / "bin" / "java";
                                #endif
                                
                                if (fs::exists(javaExe) && fs::is_regular_file(javaExe) && 
                                    isValidJavaExecutable(javaExe)) {
                                    
                                    // Get Java installation directory
                                    fs::path javaDir = getJavaDirFromExecutable(javaExe);
                                    
                                    // Only add if it's a proper Java installation
                                    if (fs::exists(javaDir / "bin" / javaExe.filename())) {
                                        std::string version = getJavaVersionInfo(javaDir);
                                        std::string publisher = getJavaPublisher(javaDir);
                                        std::string structure = getJavaStructure(javaDir);
                                        
                                        JavaInfo javaInfo(version, publisher, structure, javaDir);
                                        
                                        // Avoid duplicates
                                        if (std::find(result.begin(), result.end(), javaInfo) == result.end()) {
                                            result.push_back(javaInfo);
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        // Non-recursive scan - look for Java installations in standard structure
                        for (const auto& entry : fs::directory_iterator(directory)) {
                            if (fs::is_directory(entry.path())) {
                                // Check for bin directory with java executable
                                #ifdef _WIN32
                                fs::path javaExe = entry.path() / "bin" / "java.exe";
                                #else
                                fs::path javaExe = entry.path() / "bin" / "java";
                                #endif
                                
                                if (fs::exists(javaExe) && fs::is_regular_file(javaExe) && 
                                    isValidJavaExecutable(javaExe)) {
                                    
                                    // Get Java installation directory
                                    fs::path javaDir = entry.path();
                                    
                                    std::string version = getJavaVersionInfo(javaDir);
                                    std::string publisher = getJavaPublisher(javaDir);
                                    std::string structure = getJavaStructure(javaDir);
                                    
                                    JavaInfo javaInfo(version, publisher, structure, javaDir);
                                    
                                    // Avoid duplicates
                                    if (std::find(result.begin(), result.end(), javaInfo) == result.end()) {
                                        result.push_back(javaInfo);
                                    }
                                }
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    // Silently skip directories we can't access
                    std::cerr << "Error scanning directory " << directory << ": " << e.what() << std::endl;
                }
            }

            // Helper to convert Windows TCHAR to std::string
            #ifdef _WIN32
            std::string tcharToString(const TCHAR* tstr) {
                #ifdef UNICODE
                int len = WideCharToMultiByte(CP_UTF8, 0, tstr, -1, NULL, 0, NULL, NULL);
                std::string result(len, 0);
                WideCharToMultiByte(CP_UTF8, 0, tstr, -1, &result[0], len, NULL, NULL);
                // Remove null terminator
                if (!result.empty() && result.back() == '\0') {
                    result.pop_back();
                }
                return result;
                #else
                return std::string(tstr);
                #endif
            }
            #endif

            // Platform-specific common Java installation locations
            std::vector<fs::path> getCommonJavaLocations() {
                std::vector<fs::path> locations;
                
                #ifdef _WIN32
                // Windows common Java locations
                
                // Program Files
                TCHAR programFilesPath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programFilesPath))) {
                    locations.push_back(fs::path(tcharToString(programFilesPath)) / "Java");
                }
                
                // Program Files (x86)
                TCHAR programFilesX86Path[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILESX86, NULL, 0, programFilesX86Path))) {
                    locations.push_back(fs::path(tcharToString(programFilesX86Path)) / "Java");
                }
                
                // Local AppData
                TCHAR localAppDataPath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localAppDataPath))) {
                    locations.push_back(fs::path(tcharToString(localAppDataPath)) / "Programs" / "Java");
                }
                
                #else
                // Linux/Unix common Java locations
                
                // Standard JVM directories
                locations.push_back("/usr/lib/jvm");
                locations.push_back("/usr/lib64/jvm");
                locations.push_back("/usr/local/lib/jvm");
                
                // Common Java installation directories
                locations.push_back("/usr/java");
                locations.push_back("/usr/local/java");
                
                // OpenJDK locations
                locations.push_back("/usr/lib/jvm/java");
                locations.push_back("/usr/lib/jvm/openjdk");
                
                // Home directory installations
                const char* homeDir = std::getenv("HOME");
                if (homeDir != nullptr) {
                    locations.push_back(fs::path(homeDir) / ".jdks");
                    locations.push_back(fs::path(homeDir) / ".local" / "share" / "java");
                }
                #endif
                
                // Environment variable JAVA_HOME (cross-platform)
                const char* javaHome = std::getenv("JAVA_HOME");
                if (javaHome != nullptr && javaHome[0] != '\0') {
                    fs::path javaHomePath(javaHome);
                    if (fs::exists(javaHomePath)) {
                        locations.push_back(javaHomePath);
                    }
                }
                
                return locations;
            }

            // Platform-specific deep search locations
            std::vector<fs::path> getDeepSearchLocations() {
                std::vector<fs::path> locations = getCommonJavaLocations();
                
                #ifdef _WIN32
                // Windows deep search locations
                
                // User's home directory locations
                TCHAR userProfilePath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, userProfilePath))) {
                    fs::path userProfile(tcharToString(userProfilePath));
                    locations.push_back(userProfile / "Downloads");
                    locations.push_back(userProfile / "Desktop");
                    locations.push_back(userProfile / "Documents");
                    locations.push_back(userProfile / "AppData" / "Local" / "Programs");
                }
                
                // Additional common Windows locations
                locations.push_back("C:\\Program Files");
                locations.push_back("C:\\Program Files (x86)");
                
                #else
                // Linux deep search locations
                
                // Additional system directories
                locations.push_back("/opt");
                locations.push_back("/usr/local");
                locations.push_back("/var/lib");
                
                // SDKMAN installations
                const char* homeDir = std::getenv("HOME");
                if (homeDir != nullptr) {
                    locations.push_back(fs::path(homeDir) / ".sdkman" / "candidates" / "java");
                }
                
                // Try to expand SDKMAN path
                wordexp_t p;
                if (wordexp("~/.sdkman/candidates/java/*", &p, 0) == 0) {
                    for (size_t i = 0; i < p.we_wordc; i++) {
                        locations.push_back(fs::path(p.we_wordv[i]));
                    }
                    wordfree(&p);
                }
                #endif
                
                return locations;
            }

            // Check PATH environment variable for Java executables
            void checkPathForJava(JavaList& result) {
                const char* pathEnv = std::getenv("PATH");
                if (pathEnv == nullptr || pathEnv[0] == '\0') {
                    return;
                }
                
                std::string pathStr(pathEnv);
                #ifdef _WIN32
                const std::string delimiter = ";";
                #else
                const std::string delimiter = ":";
                #endif
                
                size_t start = 0;
                size_t end = 0;
                
                while ((end = pathStr.find(delimiter, start)) != std::string::npos) {
                    std::string path = pathStr.substr(start, end - start);
                    if (!path.empty()) {
                        fs::path dirPath(path);
                        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
                            // Look for java executable in this directory
                            #ifdef _WIN32
                            fs::path javaExe = dirPath / "java.exe";
                            #else
                            fs::path javaExe = dirPath / "java";
                            #endif
                            
                            if (fs::exists(javaExe) && fs::is_regular_file(javaExe) && 
                                isValidJavaExecutable(javaExe)) {
                                
                                // Get Java installation directory from executable
                                fs::path javaDir = getJavaDirFromExecutable(javaExe);
                                
                                // Only add if it's a proper Java installation
                                if (fs::exists(javaDir / "bin" / javaExe.filename())) {
                                    std::string version = getJavaVersionInfo(javaDir);
                                    std::string publisher = getJavaPublisher(javaDir);
                                    std::string structure = getJavaStructure(javaDir);
                                    
                                    JavaInfo javaInfo(version, publisher, structure, javaDir);
                                    
                                    // Avoid duplicates
                                    if (std::find(result.begin(), result.end(), javaInfo) == result.end()) {
                                        result.push_back(javaInfo);
                                    }
                                }
                            }
                        }
                    }
                    start = end + delimiter.length();
                }
                
                // Check the last segment
                if (start < pathStr.length()) {
                    std::string path = pathStr.substr(start);
                    if (!path.empty()) {
                        fs::path dirPath(path);
                        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
                            #ifdef _WIN32
                            fs::path javaExe = dirPath / "java.exe";
                            #else
                            fs::path javaExe = dirPath / "java";
                            #endif
                            
                            if (fs::exists(javaExe) && fs::is_regular_file(javaExe) && 
                                isValidJavaExecutable(javaExe)) {
                                
                                // Get Java installation directory from executable
                                fs::path javaDir = getJavaDirFromExecutable(javaExe);
                                
                                // Only add if it's a proper Java installation
                                if (fs::exists(javaDir / "bin" / javaExe.filename())) {
                                    std::string version = getJavaVersionInfo(javaDir);
                                    std::string publisher = getJavaPublisher(javaDir);
                                    std::string structure = getJavaStructure(javaDir);
                                    
                                    JavaInfo javaInfo(version, publisher, structure, javaDir);
                                    
                                    // Avoid duplicates
                                    if (std::find(result.begin(), result.end(), javaInfo) == result.end()) {
                                        result.push_back(javaInfo);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        /**
         * Quick search for Java installations in common locations
         * This function searches in standard installation directories
         * @return JavaList containing found Java installations
         */
        JavaList SearchJava$Quick() {
            JavaList result;
            
            // Get common Java locations
            auto locations = internal::getCommonJavaLocations();
            
            // Scan each location (non-recursive for speed)
            for (const auto& location : locations) {
                internal::scanDirectoryForJava(location, result, false);
            }
            
            // Also check PATH for java executables
            internal::checkPathForJava(result);
            
            // Remove duplicates by path
            std::sort(result.begin(), result.end(), 
                     [](const JavaInfo& a, const JavaInfo& b) { return a.path < b.path; });
            result.erase(std::unique(result.begin(), result.end(), 
                                   [](const JavaInfo& a, const JavaInfo& b) { return a.path == b.path; }), 
                        result.end());
            
            return result;
        }

        /**
         * Deep search for Java installations in both common and additional locations
         * This function performs a more thorough search including recursive scanning
         * in some locations
         * @return JavaList containing found Java installations
         */
        JavaList SearchJava$Deep() {
            JavaList result;
            
            // Get deep search locations
            auto locations = internal::getDeepSearchLocations();
            
            // Scan each location (with recursive scanning in appropriate directories)
            for (const auto& location : locations) {
                bool recursive = false;
                
                #ifdef _WIN32
                // On Windows, recursive scan for user directories
                std::string locationStr = location.string();
                std::transform(locationStr.begin(), locationStr.end(), locationStr.begin(),
                              [](unsigned char c){ return std::tolower(c); });
                
                if (locationStr.find("download") != std::string::npos ||
                    locationStr.find("desktop") != std::string::npos ||
                    locationStr.find("document") != std::string::npos ||
                    locationStr.find("appdata") != std::string::npos) {
                    recursive = true;
                }
                #else
                // On Linux, recursive scan for home directories and opt
                std::string locationStr = location.string();
                if (locationStr.find("/home/") == 0 ||
                    locationStr == "/opt" ||
                    locationStr == "/usr/local" ||
                    locationStr.find("/.sdkman/") != std::string::npos) {
                    recursive = true;
                }
                #endif
                
                internal::scanDirectoryForJava(location, result, recursive);
            }
            
            // Check PATH for java executables
            internal::checkPathForJava(result);
            
            // Remove duplicates by path
            std::sort(result.begin(), result.end(), 
                     [](const JavaInfo& a, const JavaInfo& b) { return a.path < b.path; });
            result.erase(std::unique(result.begin(), result.end(), 
                                   [](const JavaInfo& a, const JavaInfo& b) { return a.path == b.path; }), 
                        result.end());
            
            return result;
        }
    }
}