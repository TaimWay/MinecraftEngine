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
#ifndef __CNTLIB_CONFIG_HPP__
#define __CNTLIB_CONFIG_HPP__

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <optional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <utility>

namespace fs = std::filesystem;

namespace cnt {

// Forward declaration
class ConfigObject;

// Alias for internal use
using ConfigValue = std::variant<
    std::monostate,      // None
    long long,           // Integer
    double,              // Float
    bool,                // Boolean
    std::string,         // String
    char,                // Character
    std::shared_ptr<std::map<std::string, ConfigObject>>, // Object
    std::shared_ptr<std::vector<ConfigObject>>           // Array
>;

enum class ConfigType {
    NONE,
    NUMBER,
    FLOAT,
    BOOLEAN,
    STRING,
    CHARACTER,
    OBJECT,
    ARRAY
};

class ConfigObject {
private:
    ConfigValue value;
    ConfigType type;

public:
    // Constructors for each type
    ConfigObject() : value(std::monostate{}), type(ConfigType::NONE) {}
    ConfigObject(std::nullptr_t) : value(std::monostate{}), type(ConfigType::NONE) {}
    ConfigObject(long long val) : value(val), type(ConfigType::NUMBER) {}
    ConfigObject(int val) : value(static_cast<long long>(val)), type(ConfigType::NUMBER) {}
    ConfigObject(double val) : value(val), type(ConfigType::FLOAT) {}
    ConfigObject(bool val) : value(val), type(ConfigType::BOOLEAN) {}
    ConfigObject(const std::string& val) : value(val), type(ConfigType::STRING) {}
    ConfigObject(const char* val) : value(std::string(val)), type(ConfigType::STRING) {}
    ConfigObject(char val) : value(val), type(ConfigType::CHARACTER) {}
    
    // Object constructor
    ConfigObject(const std::map<std::string, ConfigObject>& obj) 
        : value(std::make_shared<std::map<std::string, ConfigObject>>(obj))
        , type(ConfigType::OBJECT) {}
    
    // Array constructor
    ConfigObject(const std::vector<ConfigObject>& arr)
        : value(std::make_shared<std::vector<ConfigObject>>(arr))
        , type(ConfigType::ARRAY) {}

    // Type checking
    bool is_none() const { return type == ConfigType::NONE; }
    bool is_number() const { return type == ConfigType::NUMBER; }
    bool is_float() const { return type == ConfigType::FLOAT; }
    bool is_boolean() const { return type == ConfigType::BOOLEAN; }
    bool is_string() const { return type == ConfigType::STRING; }
    bool is_character() const { return type == ConfigType::CHARACTER; }
    bool is_object() const { return type == ConfigType::OBJECT; }
    bool is_array() const { return type == ConfigType::ARRAY; }
    
    ConfigType get_type() const { return type; }

    // Value getters with type checking
    template<typename T>
    std::optional<T> get_as() const {
        try {
            if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, int>) {
                if (type == ConfigType::NUMBER) return std::get<long long>(value);
                if (type == ConfigType::FLOAT) return static_cast<long long>(std::get<double>(value));
            } else if constexpr (std::is_same_v<T, double>) {
                if (type == ConfigType::FLOAT) return std::get<double>(value);
                if (type == ConfigType::NUMBER) return static_cast<double>(std::get<long long>(value));
            } else if constexpr (std::is_same_v<T, bool>) {
                if (type == ConfigType::BOOLEAN) return std::get<bool>(value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (type == ConfigType::STRING) return std::get<std::string>(value);
                if (type == ConfigType::CHARACTER) return std::string(1, std::get<char>(value));
            } else if constexpr (std::is_same_v<T, char>) {
                if (type == ConfigType::CHARACTER) return std::get<char>(value);
                if (type == ConfigType::STRING) {
                    auto str = std::get<std::string>(value);
                    if (str.size() == 1) return str[0];
                }
            }
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
        return std::nullopt;
    }

    // Convenience getters
    std::optional<long long> as_number() const { return get_as<long long>(); }
    std::optional<double> as_float() const { return get_as<double>(); }
    std::optional<bool> as_boolean() const { return get_as<bool>(); }
    std::optional<std::string> as_string() const { return get_as<std::string>(); }
    std::optional<char> as_character() const { return get_as<char>(); }

    // Object and array access
    ConfigObject& operator[](const std::string& key) {
        if (type != ConfigType::OBJECT) {
            type = ConfigType::OBJECT;
            value = std::make_shared<std::map<std::string, ConfigObject>>();
        }
        auto& obj = *std::get<std::shared_ptr<std::map<std::string, ConfigObject>>>(value);
        return obj[key];
    }

    ConfigObject& operator[](size_t index) {
        if (type != ConfigType::ARRAY) {
            type = ConfigType::ARRAY;
            value = std::make_shared<std::vector<ConfigObject>>();
        }
        auto& arr = *std::get<std::shared_ptr<std::vector<ConfigObject>>>(value);
        if (index >= arr.size()) arr.resize(index + 1);
        return arr[index];
    }

    const ConfigObject& at(const std::string& key) const {
        if (type != ConfigType::OBJECT) {
            throw std::runtime_error("Not an object");
        }
        const auto& obj = *std::get<std::shared_ptr<std::map<std::string, ConfigObject>>>(value);
        return obj.at(key);
    }

    const ConfigObject& at(size_t index) const {
        if (type != ConfigType::ARRAY) {
            throw std::runtime_error("Not an array");
        }
        const auto& arr = *std::get<std::shared_ptr<std::vector<ConfigObject>>>(value);
        return arr.at(index);
    }

    size_t size() const {
        if (type == ConfigType::ARRAY) {
            return std::get<std::shared_ptr<std::vector<ConfigObject>>>(value)->size();
        } else if (type == ConfigType::OBJECT) {
            return std::get<std::shared_ptr<std::map<std::string, ConfigObject>>>(value)->size();
        } else if (type == ConfigType::STRING) {
            return std::get<std::string>(value).size();
        }
        return 0;
    }

    bool has_key(const std::string& key) const {
        if (type != ConfigType::OBJECT) return false;
        const auto& obj = *std::get<std::shared_ptr<std::map<std::string, ConfigObject>>>(value);
        return obj.find(key) != obj.end();
    }

    // String representation
    std::string to_string() const {
        switch (type) {
            case ConfigType::NONE: return "None";
            case ConfigType::NUMBER: return std::to_string(std::get<long long>(value));
            case ConfigType::FLOAT: return std::to_string(std::get<double>(value));
            case ConfigType::BOOLEAN: return std::get<bool>(value) ? "true" : "false";
            case ConfigType::STRING: return "\"" + std::get<std::string>(value) + "\"";
            case ConfigType::CHARACTER: return "'" + std::string(1, std::get<char>(value)) + "'";
            case ConfigType::OBJECT: {
                const auto& obj = *std::get<std::shared_ptr<std::map<std::string, ConfigObject>>>(value);
                std::string result = "{";
                bool first = true;
                for (const auto& [k, v] : obj) {
                    if (!first) result += ", ";
                    result += "\"" + k + "\": " + v.to_string();
                    first = false;
                }
                result += "}";
                return result;
            }
            case ConfigType::ARRAY: {
                const auto& arr = *std::get<std::shared_ptr<std::vector<ConfigObject>>>(value);
                std::string result = "[";
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += arr[i].to_string();
                }
                result += "]";
                return result;
            }
        }
        return "";
    }
};

class Config {
private:
    std::unique_ptr<std::map<std::string, ConfigObject>> data;
    std::optional<fs::path> filepath;
    bool opened = false;

    // Parser helpers
    std::string read_file(const fs::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + path.string());
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void skip_whitespace(const std::string& content, size_t& pos) {
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
    }

    void skip_comment(const std::string& content, size_t& pos) {
        if (pos < content.size() && content[pos] == '/') {
            if (pos + 1 < content.size()) {
                if (content[pos + 1] == '/') {
                    // Single line comment
                    while (pos < content.size() && content[pos] != '\n') {
                        pos++;
                    }
                } else if (content[pos + 1] == '*') {
                    // Multi-line comment
                    pos += 2;
                    while (pos + 1 < content.size() && 
                           !(content[pos] == '*' && content[pos + 1] == '/')) {
                        pos++;
                    }
                    if (pos + 1 < content.size()) {
                        pos += 2; // Skip "*/"
                    }
                }
            }
        }
    }

    std::string parse_key(const std::string& content, size_t& pos) {
        skip_whitespace(content, pos);
        
        std::string key;
        if (pos < content.size() && content[pos] == '"') {
            // Quoted key
            pos++;
            while (pos < content.size() && content[pos] != '"') {
                key += content[pos];
                pos++;
            }
            if (pos < content.size() && content[pos] == '"') {
                pos++;
            }
        } else {
            // Unquoted identifier
            while (pos < content.size() && 
                   (std::isalnum(content[pos]) || content[pos] == '_' || content[pos] == '-')) {
                key += content[pos];
                pos++;
            }
        }
        return key;
    }

    ConfigObject parse_value(const std::string& content, size_t& pos) {
        skip_whitespace(content, pos);
        
        if (pos >= content.size()) return ConfigObject();
        
        // Check for None
        if (content.substr(pos, 4) == "None") {
            pos += 4;
            return ConfigObject(nullptr);
        }
        
        // Check for boolean
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            return ConfigObject(true);
        }
        if (content.substr(pos, 5) == "false") {
            pos += 5;
            return ConfigObject(false);
        }
        
        // Check for string
        if (content[pos] == '"') {
            pos++;
            std::string str;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    // Handle escape sequences
                    pos++;
                    switch (content[pos]) {
                        case 'n': str += '\n'; break;
                        case 't': str += '\t'; break;
                        case 'r': str += '\r'; break;
                        case '\\': str += '\\'; break;
                        case '"': str += '"'; break;
                        default: str += content[pos]; break;
                    }
                } else {
                    str += content[pos];
                }
                pos++;
            }
            if (pos < content.size() && content[pos] == '"') {
                pos++;
            }
            return ConfigObject(str);
        }
        
        // Check for character
        if (content[pos] == '\'') {
            pos++;
            char ch = '\0';
            if (pos < content.size()) {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    // Handle escape sequences
                    pos++;
                    switch (content[pos]) {
                        case 'n': ch = '\n'; break;
                        case 't': ch = '\t'; break;
                        case 'r': ch = '\r'; break;
                        case '\\': ch = '\\'; break;
                        case '\'': ch = '\''; break;
                        default: ch = content[pos]; break;
                    }
                } else {
                    ch = content[pos];
                }
                pos++;
            }
            if (pos < content.size() && content[pos] == '\'') {
                pos++;
            }
            return ConfigObject(ch);
        }
        
        // Check for array
        if (content[pos] == '[') {
            pos++;
            std::vector<ConfigObject> array;
            
            skip_whitespace(content, pos);
            while (pos < content.size() && content[pos] != ']') {
                auto value = parse_value(content, pos);
                array.push_back(value);
                
                skip_whitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                    skip_whitespace(content, pos);
                }
            }
            if (pos < content.size() && content[pos] == ']') {
                pos++;
            }
            return ConfigObject(array);
        }
        
        // Check for object
        if (content[pos] == '{') {
            pos++;
            std::map<std::string, ConfigObject> object;
            
            skip_whitespace(content, pos);
            while (pos < content.size() && content[pos] != '}') {
                auto key = parse_key(content, pos);
                
                skip_whitespace(content, pos);
                if (pos < content.size() && content[pos] == ':') {
                    pos++;
                }
                
                auto value = parse_value(content, pos);
                object[key] = value;
                
                skip_whitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                    skip_whitespace(content, pos);
                }
            }
            if (pos < content.size() && content[pos] == '}') {
                pos++;
            }
            return ConfigObject(object);
        }
        
        // Check for number
        if (std::isdigit(content[pos]) || content[pos] == '-' || content[pos] == '+') {
            std::string num_str;
            bool has_decimal = false;
            
            // Handle sign
            if (content[pos] == '-' || content[pos] == '+') {
                num_str += content[pos];
                pos++;
            }
            
            // Parse number
            while (pos < content.size() && 
                   (std::isdigit(content[pos]) || content[pos] == '.' || 
                    content[pos] == 'e' || content[pos] == 'E' ||
                    content[pos] == '+' || content[pos] == '-')) {
                if (content[pos] == '.') has_decimal = true;
                num_str += content[pos];
                pos++;
            }
            
            try {
                if (has_decimal) {
                    return ConfigObject(std::stod(num_str));
                } else {
                    return ConfigObject(std::stoll(num_str));
                }
            } catch (...) {
                throw std::runtime_error("Invalid number: " + num_str);
            }
        }
        
        return ConfigObject();
    }

    void parse_content(const std::string& content) {
        data = std::make_unique<std::map<std::string, ConfigObject>>();
        size_t pos = 0;
        
        while (pos < content.size()) {
            skip_whitespace(content, pos);
            
            // Skip comments
            while (pos < content.size() && content[pos] == '/') {
                size_t old_pos = pos;
                skip_comment(content, pos);
                if (pos == old_pos) break; // Not a comment
                skip_whitespace(content, pos);
            }
            
            if (pos >= content.size()) break;
            
            // Parse key-value pair
            std::string key = parse_key(content, pos);
            
            skip_whitespace(content, pos);
            if (pos < content.size() && content[pos] == ':') {
                pos++;
            }
            
            ConfigObject value = parse_value(content, pos);
            
            if (!key.empty()) {
                (*data)[key] = value;
            }
            
            skip_whitespace(content, pos);
            if (pos < content.size() && content[pos] == ',') {
                pos++;
            }
        }
    }

    void write_value(std::ostream& os, const ConfigObject& obj, int indent = 0, bool is_inline = false) {
        const std::string indent_str(indent * 4, ' ');
        
        switch (obj.get_type()) {
            case ConfigType::NONE:
                os << "None";
                break;
            case ConfigType::NUMBER:
                os << obj.as_number().value();
                break;
            case ConfigType::FLOAT:
                os << obj.as_float().value();
                break;
            case ConfigType::BOOLEAN:
                os << (obj.as_boolean().value() ? "true" : "false");
                break;
            case ConfigType::STRING: {
                std::string str = obj.as_string().value();
                // Escape special characters
                std::string escaped;
                for (char c : str) {
                    switch (c) {
                        case '\n': escaped += "\\n"; break;
                        case '\t': escaped += "\\t"; break;
                        case '\r': escaped += "\\r"; break;
                        case '\\': escaped += "\\\\"; break;
                        case '"': escaped += "\\\""; break;
                        default: escaped += c; break;
                    }
                }
                os << "\"" << escaped << "\"";
                break;
            }
            case ConfigType::CHARACTER: {
                char ch = obj.as_character().value();
                switch (ch) {
                    case '\n': os << "'\\n'"; break;
                    case '\t': os << "'\\t'"; break;
                    case '\r': os << "'\\r'"; break;
                    case '\\': os << "'\\\\'"; break;
                    case '\'': os << "'\\''"; break;
                    default: os << "'" << ch << "'"; break;
                }
                break;
            }
            case ConfigType::OBJECT: {
                const auto obj_ptr = obj.get_as<std::shared_ptr<std::map<std::string, ConfigObject>>>();
                if (!obj_ptr) break;
                
                const auto& map_obj = *obj_ptr.value();
                if (map_obj.empty()) {
                    os << "{}";
                } else if (is_inline) {
                    os << "{";
                    bool first = true;
                    for (const auto& [key, value] : map_obj) {
                        if (!first) os << ", ";
                        os << "\"" << key << "\": ";
                        write_value(os, value, 0, true);
                        first = false;
                    }
                    os << "}";
                } else {
                    os << "{\n";
                    bool first = true;
                    for (const auto& [key, value] : map_obj) {
                        if (!first) os << ",\n";
                        os << indent_str << "    \"" << key << "\": ";
                        write_value(os, value, indent + 1, false);
                        first = false;
                    }
                    os << "\n" << indent_str << "}";
                }
                break;
            }
            case ConfigType::ARRAY: {
                const auto arr_ptr = obj.get_as<std::shared_ptr<std::vector<ConfigObject>>>();
                if (!arr_ptr) break;
                
                const auto& vec = *arr_ptr.value();
                if (vec.empty()) {
                    os << "[]";
                } else if (is_inline || vec.size() <= 3) {
                    os << "[";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        if (i > 0) os << ", ";
                        write_value(os, vec[i], 0, true);
                    }
                    os << "]";
                } else {
                    os << "[\n";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        if (i > 0) os << ",\n";
                        os << indent_str << "    ";
                        write_value(os, vec[i], indent + 1, false);
                    }
                    os << "\n" << indent_str << "]";
                }
                break;
            }
        }
    }

public:
    Config() : data(std::make_unique<std::map<std::string, ConfigObject>>()) {}
    
    ~Config() = default;
    
    void open(const fs::path& path) {
        try {
            std::string content = read_file(path);
            parse_content(content);
            filepath = path;
            opened = true;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to open config file: " + std::string(e.what()));
        }
    }
    
    bool is_open() const {
        return opened;
    }
    
    void close() {
        data->clear();
        filepath = std::nullopt;
        opened = false;
    }
    
    void save() {
        if (!filepath.has_value()) {
            throw std::runtime_error("No filepath specified");
        }
        save(filepath.value());
    }
    
    void save(const fs::path& path) {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot save to file: " + path.string());
        }
        
        for (const auto& [key, value] : *data) {
            file << key << ": ";
            write_value(file, value, 0, false);
            file << "\n";
        }
    }
    
    ConfigObject get(const std::string& name) const {
        if (!data) {
            throw std::runtime_error("Config not initialized");
        }
        auto it = data->find(name);
        if (it != data->end()) {
            return it->second;
        }
        return ConfigObject();
    }
    
    // Overloaded set methods for different types
    void set(const std::string& name, const std::string& value) {
        (*data)[name] = ConfigObject(value);
    }
    
    void set(const std::string& name, const char* value) {
        (*data)[name] = ConfigObject(std::string(value));
    }
    
    void set(const std::string& name, long long value) {
        (*data)[name] = ConfigObject(value);
    }
    
    void set(const std::string& name, int value) {
        (*data)[name] = ConfigObject(static_cast<long long>(value));
    }
    
    void set(const std::string& name, double value) {
        (*data)[name] = ConfigObject(value);
    }
    
    void set(const std::string& name, bool value) {
        (*data)[name] = ConfigObject(value);
    }
    
    void set(const std::string& name, char value) {
        (*data)[name] = ConfigObject(value);
    }
    
    void set(const std::string& name, std::nullptr_t) {
        (*data)[name] = ConfigObject(nullptr);
    }
    
    void set(const std::string& name, const std::map<std::string, ConfigObject>& value) {
        (*data)[name] = ConfigObject(value);
    }
    
    void set(const std::string& name, const std::vector<ConfigObject>& value) {
        (*data)[name] = ConfigObject(value);
    }
    
    // Generic set method using ConfigObject
    void set(const std::string& name, const ConfigObject& value) {
        (*data)[name] = value;
    }
    
    // Overloaded add methods for different types
    void add(const std::string& name, const std::string& value) {
        add_impl(name, ConfigObject(value));
    }
    
    void add(const std::string& name, const char* value) {
        add_impl(name, ConfigObject(std::string(value)));
    }
    
    void add(const std::string& name, long long value) {
        add_impl(name, ConfigObject(value));
    }
    
    void add(const std::string& name, int value) {
        add_impl(name, ConfigObject(static_cast<long long>(value)));
    }
    
    void add(const std::string& name, double value) {
        add_impl(name, ConfigObject(value));
    }
    
    void add(const std::string& name, bool value) {
        add_impl(name, ConfigObject(value));
    }
    
    void add(const std::string& name, char value) {
        add_impl(name, ConfigObject(value));
    }
    
    void add(const std::string& name, std::nullptr_t) {
        add_impl(name, ConfigObject(nullptr));
    }
    
    void add(const std::string& name, const ConfigObject& value) {
        add_impl(name, value);
    }
    
private:
    // Internal implementation for add
    void add_impl(const std::string& name, const ConfigObject& value) {
        if (data->find(name) != data->end() && (*data)[name].is_array()) {
            // Get the array and append
            auto arr_ptr = (*data)[name].get_as<std::shared_ptr<std::vector<ConfigObject>>>();
            if (arr_ptr) {
                arr_ptr.value()->push_back(value);
            }
        } else {
            set(name, value);
        }
    }
    
public:
    void remove(const std::string& name) {
        data->erase(name);
    }
    
    // Operators
    ConfigObject operator[](const std::string& name) {
        return get(name);
    }
    
    ConfigObject& operator=(const ConfigObject& obj) {
        // This is a bit tricky since Config is a container, not a ConfigObject
        // We'll interpret this as setting a special "_root" key
        (*data)["_root"] = obj;
        return (*data)["_root"];
    }
    
    ConfigObject operator+(const ConfigObject& obj) {
        // Merge objects
        if (obj.is_object() && data->find("_root") != data->end() && 
            (*data)["_root"].is_object()) {
            auto root_obj = (*data)["_root"];
            auto new_obj = ConfigObject(std::map<std::string, ConfigObject>());
            
            // Merge logic here (simplified)
            return new_obj;
        }
        return obj;
    }
    
    ConfigObject operator-(const std::string& name) {
        ConfigObject result = get(name);
        remove(name);
        return result;
    }
    
    // Iterator support
    auto begin() { return data->begin(); }
    auto end() { return data->end(); }
    auto begin() const { return data->begin(); }
    auto end() const { return data->end(); }
    auto cbegin() const { return data->cbegin(); }
    auto cend() const { return data->cend(); }
};

} // namespace cnt

#endif // __CNTLIB_CONFIG_HPP__