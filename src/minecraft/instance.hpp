/*
 * Minecraft Engine
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 * 
 * @File: minecraft/instance.hpp
 * @Description: Used to describe a Minecraft game instance
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
#ifndef __MINECRAFT_ENGINE__INSTANCE_HPP__
#define __MINECRAFT_ENGINE__INSTANCE_HPP__

#include <minecraft/cntconfig.hpp>
#include <minecraft/index.hpp>

namespace cnt
{
    namespace minecraft
    {
        class Instance {
        private:
            String name;
            String description;
            Index _father_path;
            
        public:
            Instance(String _name) : name(_name) { _init(); }

        private:
            void _init();
        };
    } // namespace minecraft
    
} // namespace cnt


#endif // !__MINECRAFT_ENGINE__INSTANCE_HPP__