/*
 * CNT Library
 * Copyright (C) 2026 TaimWay <taimway@gmail.com>
 *
 * @File: config.hpp
 * @Description: Configuration parser with support for None, Number, String, Character, Boolean, Float, Object, Array
 * @Ownership: TaimWay <taimway@gmail.com>
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
#ifndef __CNTLIB_NET_HPP__
#define __CNTLIB_NET_HPP__

#ifdef _UNICODE
#define __$_INCLUDE_UNICODE__
#undef _UNICODE
#endif

#ifdef _WIN32
#include <windows.h>

#pragma comment(lib, "urlmon.lib")
#endif

#include <string>

namespace cnt
{
    class HttpState
    {
    private:
        unsigned int _code;

    public:
        HttpState() : _code(0) {}
        HttpState(unsigned int _code) : _code(_code) {}
        HttpState(const HttpState &state) : _code(state._code) {}
        void operator=(unsigned int code) { _code = code; }
        void operator=(const HttpState &state) { _code = state._code; }
        bool operator==(const HttpState &state) const { return _code == state._code; }
        bool operator!=(const HttpState &state) const { return _code != state._code; }
        bool operator<(const HttpState &state) const { return _code < state._code; }
        bool operator<=(const HttpState &state) const { return _code <= state._code; }
        bool operator>(const HttpState &state) const { return _code > state._code; }
        bool operator>=(const HttpState &state) const { return _code >= state._code; }
        bool operator==(unsigned int code) const { return _code == code; }
        bool operator!=(unsigned int code) const { return _code != code; }
        bool operator<(unsigned int code) const { return _code < code; }
        bool operator<=(unsigned int code) const { return _code <= code; }
        bool operator>(unsigned int code) const { return _code > code; }
        bool operator>=(unsigned int code) const { return _code >= code; }

        unsigned int get() const { return _code; }
        bool isInfo() const { return _code >= 100 && _code < 200; }
        bool isSuccess() const { return _code >= 200 && _code < 300; }
        bool isRedirect() const { return _code >= 300 && _code < 400; }
        bool isClientError() const { return _code >= 400 && _code < 500; }
        bool isServerError() const { return _code >= 500 && _code < 600; }
        bool isError() const { return _code >= 400; }
        bool isOk() const { return _code == 200; }
    };

    HttpState DownloadFile(std::string url, std::string path)
    {
#ifdef _WIN32
        switch (URLDownloadToFile(NULL, url.c_str(), path.c_str(), 0, NULL))
        {
        case S_OK:
            return HttpState(200);
        case E_OUTOFMEMORY:
            return HttpState(500);
        case INET_E_DOWNLOAD_FAILURE:
            return HttpState(404);
        default:
            break;
        }
        return HttpState(400);
#endif
    }
}

#ifdef __$_INCLUDE_UNICODE__
#define _UNICODE
#undef __$_INCLUDE_UNICODE__
#endif
#endif // __CNTLIB_NET_HPP__