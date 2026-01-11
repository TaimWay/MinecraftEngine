/*
 * Minecraft Engine
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 *
 * @File: minecraft/source/version.cpp
 * @Description:
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

#include <minecraft/version.hpp>

#include <sstream>
#include <algorithm>

namespace cnt
{
    namespace minecraft
    {

        void Version<VersionBefore26>::VersionData::parse(const String &versionStr)
        {
            std::regex pattern(R"(^(\d+)\.(\d+)(?:\.(\d+))?$)");
            std::smatch matches;

            if (std::regex_match(versionStr, matches, pattern))
            {
                int major = std::stoi(matches[1]);

                // For VersionBefore26, major must be 1
                if (major != 1)
                {
                    throw std::runtime_error("VersionBefore26: Major version must be 1");
                }

                minor = std::stoi(matches[2]);

                // Patch is optional, default to 0 if not provided
                if (matches[3].matched)
                {
                    patch = std::stoi(matches[3]);
                }
                else
                {
                    patch = 0;
                }
            }
            else
            {
                throw std::runtime_error("Invalid version format for VersionBefore26. Expected format: 1.minor.patch or minor.patch");
            }
        }

        String Version<VersionBefore26>::VersionData::toString() const
        {
            std::stringstream ss;
            ss << "1." << minor << "." << patch;
            return ss.str();
        }

        bool Version<VersionBefore26>::VersionData::operator==(const VersionData &other) const
        {
            return minor == other.minor && patch == other.patch;
        }

        bool Version<VersionBefore26>::VersionData::operator<(const VersionData &other) const
        {
            if (minor != other.minor)
            {
                return minor < other.minor;
            }
            return patch < other.patch;
        }

        // VersionBefore26 member functions
        Version<VersionBefore26>::Version() : data_(std::make_unique<VersionData>()) {}

        Version<VersionBefore26>::Version(const String &versionStr)
            : data_(std::make_unique<VersionData>())
        {
            data_->parse(versionStr);
            validateComponents();
        }

        Version<VersionBefore26>::~Version() = default;

        Version<VersionBefore26>::Version(const Version &other)
            : data_(std::make_unique<VersionData>(*other.data_)) {}

        Version<VersionBefore26>::Version(Version &&other) noexcept
            : data_(std::move(other.data_)) {}

        Version<VersionBefore26> &Version<VersionBefore26>::operator=(const Version &other)
        {
            if (this != &other)
            {
                *data_ = *other.data_;
            }
            return *this;
        }

        Version<VersionBefore26> &Version<VersionBefore26>::operator=(Version &&other) noexcept
        {
            if (this != &other)
            {
                data_ = std::move(other.data_);
            }
            return *this;
        }

        Version<VersionBefore26> &Version<VersionBefore26>::operator=(const String &versionStr)
        {
            data_->parse(versionStr);
            validateComponents();
            return *this;
        }

        Version<VersionBefore26>::operator String() const
        {
            return data_->toString();
        }

        bool Version<VersionBefore26>::operator==(const Version &other) const
        {
            return *data_ == *other.data_;
        }

        bool Version<VersionBefore26>::operator!=(const Version &other) const
        {
            return !(*data_ == *other.data_);
        }

        bool Version<VersionBefore26>::operator<(const Version &other) const
        {
            return *data_ < *other.data_;
        }

        bool Version<VersionBefore26>::operator>(const Version &other) const
        {
            return *other.data_ < *data_;
        }

        bool Version<VersionBefore26>::operator<=(const Version &other) const
        {
            return !(*other.data_ < *data_);
        }

        bool Version<VersionBefore26>::operator>=(const Version &other) const
        {
            return !(*data_ < *other.data_);
        }

        int Version<VersionBefore26>::minor() const
        {
            return data_->minor;
        }

        int Version<VersionBefore26>::patch() const
        {
            return data_->patch;
        }

        void Version<VersionBefore26>::validateComponents() const
        {
            if (data_->minor < 0)
            {
                throw std::runtime_error("VersionBefore26: Minor version cannot be negative");
            }
            if (data_->patch < 0)
            {
                throw std::runtime_error("VersionBefore26: Patch version cannot be negative");
            }
        }

        // ============================================================================
        // Implementation for VersionAfter26 specialization
        // ============================================================================

        void Version<VersionAfter26>::VersionData::parse(const String &versionStr)
        {
            std::regex pattern(R"(^(\d+)\.(\d+)(?:\.(\d+))?$)");
            std::smatch matches;

            if (std::regex_match(versionStr, matches, pattern))
            {
                major = std::stoi(matches[1]);
                minor = std::stoi(matches[2]);

                // Patch is optional, default to 0 if not provided
                if (matches[3].matched)
                {
                    patch = std::stoi(matches[3]);
                }
                else
                {
                    patch = 0;
                }

                // For VersionAfter26, major must be >= 26
                if (major < 26)
                {
                    throw std::runtime_error("VersionAfter26: Major version must be 26 or greater");
                }
            }
            else
            {
                throw std::runtime_error("Invalid version format for VersionAfter26. Expected format: major.minor or major.minor.patch");
            }
        }

        String Version<VersionAfter26>::VersionData::toString() const
        {
            std::stringstream ss;
            ss << major << "." << minor;
            if (patch > 0)
            {
                ss << "." << patch;
            }
            return ss.str();
        }

        bool Version<VersionAfter26>::VersionData::operator==(const VersionData &other) const
        {
            return major == other.major &&
                   minor == other.minor &&
                   patch == other.patch;
        }

        bool Version<VersionAfter26>::VersionData::operator<(const VersionData &other) const
        {
            if (major != other.major)
            {
                return major < other.major;
            }
            if (minor != other.minor)
            {
                return minor < other.minor;
            }
            return patch < other.patch;
        }

        // VersionAfter26 member functions
        Version<VersionAfter26>::Version() : data_(std::make_unique<VersionData>()) {}

        Version<VersionAfter26>::Version(const String &versionStr)
            : data_(std::make_unique<VersionData>())
        {
            data_->parse(versionStr);
            validateComponents();
        }

        Version<VersionAfter26>::~Version() = default;

        Version<VersionAfter26>::Version(const Version &other)
            : data_(std::make_unique<VersionData>(*other.data_)) {}

        Version<VersionAfter26>::Version(Version &&other) noexcept
            : data_(std::move(other.data_)) {}

        Version<VersionAfter26> &Version<VersionAfter26>::operator=(const Version &other)
        {
            if (this != &other)
            {
                *data_ = *other.data_;
            }
            return *this;
        }

        Version<VersionAfter26> &Version<VersionAfter26>::operator=(Version &&other) noexcept
        {
            if (this != &other)
            {
                data_ = std::move(other.data_);
            }
            return *this;
        }

        Version<VersionAfter26> &Version<VersionAfter26>::operator=(const String &versionStr)
        {
            data_->parse(versionStr);
            validateComponents();
            return *this;
        }

        Version<VersionAfter26>::operator String() const
        {
            return data_->toString();
        }

        bool Version<VersionAfter26>::operator==(const Version &other) const
        {
            return *data_ == *other.data_;
        }

        bool Version<VersionAfter26>::operator!=(const Version &other) const
        {
            return !(*data_ == *other.data_);
        }

        bool Version<VersionAfter26>::operator<(const Version &other) const
        {
            return *data_ < *other.data_;
        }

        bool Version<VersionAfter26>::operator>(const Version &other) const
        {
            return *other.data_ < *data_;
        }

        bool Version<VersionAfter26>::operator<=(const Version &other) const
        {
            return !(*other.data_ < *data_);
        }

        bool Version<VersionAfter26>::operator>=(const Version &other) const
        {
            return !(*data_ < *other.data_);
        }

        int Version<VersionAfter26>::major() const
        {
            return data_->major;
        }

        int Version<VersionAfter26>::minor() const
        {
            return data_->minor;
        }

        int Version<VersionAfter26>::patch() const
        {
            return data_->patch;
        }

        void Version<VersionAfter26>::validateComponents() const
        {
            if (data_->major < 26)
            {
                throw std::runtime_error("VersionAfter26: Major version must be 26 or greater");
            }
            if (data_->minor < 0)
            {
                throw std::runtime_error("VersionAfter26: Minor version cannot be negative");
            }
            if (data_->patch < 0)
            {
                throw std::runtime_error("VersionAfter26: Patch version cannot be negative");
            }
        }

    } // namespace minecraft
} // namespace cnt