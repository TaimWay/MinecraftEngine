/*
 * Minecraft Engine
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 *
 * @File: minecraft/version.hpp
 * @Description:
 * @Ownership: TaimWay <taimway@gmail.com> - 01/10/2026
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
#ifndef __MINECRAFT_ENGINE__VERSION_HPP__
#define __MINECRAFT_ENGINE__VERSION_HPP__

#include <minecraft/cntconfig.hpp>

#include <stdexcept>
#include <regex>
#include <memory>

namespace cnt
{
    namespace minecraft
    {

        // Version standard tags
        struct VersionBefore26
        {
        };
        struct VersionAfter26
        {
        };

        // Base version class template
        template <typename VersionStandard>
        class Version
        {
            // Static assertion to prevent compilation with unsupported types
            static_assert(std::is_same<VersionStandard, VersionBefore26>::value ||
                              std::is_same<VersionStandard, VersionAfter26>::value,
                          "Template parameter must be either VersionBefore26 or VersionAfter26");

        private:
            struct VersionData;
            std::unique_ptr<VersionData> data_;

        public:
            // Constructor
            Version();

            // Destructor
            ~Version();

            // Copy and move operations
            Version(const Version &other);
            Version(Version &&other) noexcept;
            Version &operator=(const Version &other);
            Version &operator=(Version &&other) noexcept;

            // Assignment from string
            Version &operator=(const String &versionStr);

            // String conversion
            explicit operator String() const;

            // Comparison operators
            bool operator==(const Version &other) const;
            bool operator!=(const Version &other) const;
            bool operator<(const Version &other) const;
            bool operator>(const Version &other) const;
            bool operator<=(const Version &other) const;
            bool operator>=(const Version &other) const;

        private:
            // Parse version string based on standard
            void parseFromString(const String &versionStr);

            // Validate parsed components
            void validateComponents() const;
        };

        // Specialization for VersionBefore26
        template <>
        class Version<VersionBefore26>
        {
        private:
            // Version data structure - not exposed
            struct VersionData
            {
                int minor = 0;
                int patch = 0;

                // Parse version string in format "1.minor.patch" or "minor.patch"
                void parse(const String &versionStr);

                // Convert to string
                String toString() const;

                // Comparison operators
                bool operator==(const VersionData &other) const;
                bool operator<(const VersionData &other) const;
            };

            std::unique_ptr<VersionData> data_;

        public:
            // Constructor
            Version();

            // Constructor with version string
            explicit Version(const String &versionStr);

            // Destructor
            ~Version();

            // Copy and move operations
            Version(const Version &other);
            Version(Version &&other) noexcept;
            Version &operator=(const Version &other);
            Version &operator=(Version &&other) noexcept;

            // Assignment from string
            Version &operator=(const String &versionStr);

            // String conversion
            explicit operator String() const;

            // Comparison operators
            bool operator==(const Version &other) const;
            bool operator!=(const Version &other) const;
            bool operator<(const Version &other) const;
            bool operator>(const Version &other) const;
            bool operator<=(const Version &other) const;
            bool operator>=(const Version &other) const;

            // Getter methods
            int minor() const;
            int patch() const;

        private:
            // Validate version components
            void validateComponents() const;
        };

        // Specialization for VersionAfter26
        template <>
        class Version<VersionAfter26>
        {
        private:
            // Version data structure - not exposed
            struct VersionData
            {
                int major = 26; // Default to 26 as per standard
                int minor = 0;
                int patch = 0;

                // Parse version string in format "major.minor" or "major.minor.patch"
                void parse(const String &versionStr);

                // Convert to string
                String toString() const;

                // Comparison operators
                bool operator==(const VersionData &other) const;
                bool operator<(const VersionData &other) const;
            };

            std::unique_ptr<VersionData> data_;

        public:
            // Constructor
            Version();

            // Constructor with version string
            explicit Version(const String &versionStr);

            // Destructor
            ~Version();

            // Copy and move operations
            Version(const Version &other);
            Version(Version &&other) noexcept;
            Version &operator=(const Version &other);
            Version &operator=(Version &&other) noexcept;

            // Assignment from string
            Version &operator=(const String &versionStr);

            // String conversion
            explicit operator String() const;

            // Comparison operators
            bool operator==(const Version &other) const;
            bool operator!=(const Version &other) const;
            bool operator<(const Version &other) const;
            bool operator>(const Version &other) const;
            bool operator<=(const Version &other) const;
            bool operator>=(const Version &other) const;

            // Getter methods
            int major() const;
            int minor() const;
            int patch() const;

        private:
            // Validate version components
            void validateComponents() const;
        };
    } // namespace minecraft
} // namespace cnt

#ifdef MINECRAFT_ENGINE_IMPLEMENTATION
#include <minecraft/source/version.cpp>
#endif // MINECRAFT_ENGINE_IMPLEMENTATION

#endif // __MINECRAFT_ENGINE__VERSION_HPP__