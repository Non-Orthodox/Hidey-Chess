#ifndef SETTINGS_H
#define SETTINGS_H

#include <assert.h>
#include <cstddef>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
// #include "array.h"

enum settingsType_t {
	settingsType_boolean,
	settingsType_integer,
	settingsType_float,
	settingsType_string,
	settingsType_any
};

class Setting {
private:
	union {
		bool valueBool = false;
		int valueInt;
		float valueFloat;
		std::string *valueString;
	};
public:
	std::string name;
	settingsType_t type = settingsType_boolean;
	int (*callback)(Setting *) = nullptr;
	Setting(std::string name, bool value) {
		this->name = name;
		this->type = settingsType_boolean;
		this->valueBool = value;
	}
	Setting(bool value) {
		this->type = settingsType_boolean;
		this->valueBool = value;
	}
	Setting(std::string name, int value) {
		this->name = name;
		this->type = settingsType_integer;
		this->valueInt = value;
	}
	Setting(int value) {
		this->type = settingsType_integer;
		this->valueInt = value;
	}
	Setting(std::string name, float value) {
		this->name = name;
		this->type = settingsType_float;
		this->valueFloat = value;
	}
	Setting(float value) {
		this->type = settingsType_float;
		this->valueFloat = value;
	}
	Setting(std::string name, std::string value) {
		this->name = name;
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	Setting(std::string value) {
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	Setting(std::string name, const char *value) {
		this->name = name;
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	Setting(const char *value) {
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	~Setting() {
		name.clear();
	}
	bool getBool() {
		assert(type == settingsType_boolean);
		return this->valueBool;
	}
	int getInt() {
		assert(type == settingsType_integer);
		return this->valueInt;
	}
	float getFloat() {
		assert(type == settingsType_float);
		return this->valueFloat;
	}
	std::string getString() {
		assert(type == settingsType_string);
		return *this->valueString;
	}
	bool set(bool value) {
		assert(type == settingsType_boolean);
		this->valueBool = value;
		if (callback != nullptr) {
			if (callback(this)) {
				throw std::logic_error("Setting::set(bool)");
			}
		}
		return this->valueBool;
	}
	int set(int value) {
		assert(type == settingsType_integer);
		this->valueInt = value;
		if (callback != nullptr) {
			if (callback(this)) {
				throw std::logic_error("Setting::set(int)");
			}
		}
		return this->valueInt;
	}
	float set(float value) {
		assert(type == settingsType_float);
		this->valueFloat = value;
		if (callback != nullptr) {
			if (callback(this)) {
				throw std::logic_error("Setting::set(float)");
			}
		}
		return this->valueFloat;
	}
	std::string set(std::string value) {
		assert(type == settingsType_string);
		*this->valueString = value;
		if (callback != nullptr) {
			if (callback(this)) {
				throw std::logic_error("Setting::set(std::string)");
			}
		}
		return *this->valueString;
	}
	std::string set(const char *value) {
		assert(type == settingsType_string);
		*this->valueString = value;
		if (callback != nullptr) {
			if (callback(this)) {
				throw std::logic_error("Setting::set(std::string)");
			}
		}
		return *this->valueString;
	}
};

class SettingsList {
private:
	std::vector<Setting> array;
	std::map<std::string, ptrdiff_t> map;
public:
	void insert(std::string name, Setting value) {
		map.insert(std::pair<std::string, ptrdiff_t>(name, array.size()));
		array.push_back(value);
	}
	Setting *operator[](std::ptrdiff_t index) {
		return &array[index];
	}
	Setting *operator[](std::string name) {
		return &array[map.at(name)];
	}
	Setting *find(std::string name) {
		return &array[map.at(name)];
	}
};


extern SettingsList *g_settings;

/* Settings */

#define SETTINGS_LIST \
	ENTRY(peer_ip_address, "localhost") \
	ENTRY(peer_network_port, 2850) \
	ENTRY(network_port, 2851) \
	ENTRY(board_width, 8) \
	ENTRY(board_height, 8) \
	ENTRY(disable_sdl, false) \
	ENTRY(log_level, 0) \

#define SETTINGS_ALIAS_LIST \
	ENTRY('a', peer_ip_address) \
	ENTRY('p', peer_network_port) \
	ENTRY('n', network_port) \
	ENTRY('s', disable_sdl) \
	ENTRY('l', log_level) \

#define SETTINGS_CALLBACKS_LIST \

#define ENTRY(ENTRY_name, ENTRY_value) settingEnum_##ENTRY_name,
enum settingEnum_t {
	SETTINGS_LIST
};
#undef ENTRY

#endif // SETTINGS_H
