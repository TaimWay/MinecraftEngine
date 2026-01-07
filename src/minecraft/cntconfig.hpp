/*
 * Minecraft Engine
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 * 
 * @File: minecraft/cntconfig.hpp
 * @Description: Declares some basic objects needed by the library
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
#ifndef __MINECRAFT_ENGINE__CNTCONFIG_HPP__
#define __MINECRAFT_ENGINE__CNTCONFIG_HPP__

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

typedef int int32_t;
using int64_t = int64_t;
typedef long long int128_t;

typedef unsigned int uint32_t;
using uint64_t = uint64_t;  
typedef unsigned long long uint128_t;

using String = std::string;

#endif // !__MINECRAFT_ENGINE__CNTCONFIG_HPP__
