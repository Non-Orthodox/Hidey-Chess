#ifndef SETTINGS_H
#define SETTINGS_H

#include <assert.h>
#include <cstddef>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include <iterator>

enum settingsType_t {
	settingsType_boolean,
	settingsType_integer,
	settingsType_float,
	settingsType_string,
	settingsType_any
};

class Setting {
private:
public:
	union {
		bool valueBool = false;
		int valueInt;
		float valueFloat;
		std::string *valueString;
	};
	bool locked;
	void constantInit() {
		this->locked = false;
		this->save = true;
	}
	bool save;
	std::string name;
	settingsType_t type = settingsType_boolean;
	int (*callback)(Setting *) = nullptr;
	Setting(std::string name, bool value) {
		constantInit();
		this->name = name;
		this->type = settingsType_boolean;
		this->valueBool = value;
	}
	Setting(bool value) {
		constantInit();
		this->type = settingsType_boolean;
		this->valueBool = value;
	}
	Setting(std::string name, int value) {
		constantInit();
		this->name = name;
		this->type = settingsType_integer;
		this->valueInt = value;
	}
	Setting(int value) {
		constantInit();
		this->type = settingsType_integer;
		this->valueInt = value;
	}
	Setting(std::string name, float value) {
		constantInit();
		this->name = name;
		this->type = settingsType_float;
		this->valueFloat = value;
	}
	Setting(float value) {
		constantInit();
		this->type = settingsType_float;
		this->valueFloat = value;
	}
	Setting(std::string name, std::string value) {
		constantInit();
		this->name = name;
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	Setting(std::string value) {
		constantInit();
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	Setting(std::string name, const char *value) {
		constantInit();
		this->name = name;
		this->type = settingsType_string;
		this->valueString = new std::string();
		*this->valueString = value;
	}
	Setting(const char *value) {
		constantInit();
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
		if (this->locked) return this->valueBool;
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
		if (this->locked) return this->valueInt;
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
		if (this->locked) return this->valueFloat;
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
		if (this->locked) return *this->valueString;
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
		if (this->locked) return *this->valueString;
		*this->valueString = value;
		if (callback != nullptr) {
			if (callback(this)) {
				throw std::logic_error("Setting::set(std::string)");
			}
		}
		return *this->valueString;
	}
	int setFromString(const std::string value) {
		bool tempBool = false;
		int tempInt = 0;
		float tempFloat = 0.0;
		
		switch (this->type) {
		case settingsType_boolean:
			if (value.length() == 0) {
				try {
					std::to_string(this->set(true));
					return 0;
				}
				catch (std::logic_error& e) {
					return 2;
				}
			}
			try {
				// Mainly for fun.
				tempBool = !!std::stoi(value);
			}
			catch (std::invalid_argument& e) {
				return 1;
			}
			catch (std::out_of_range& e) {
				return 1;
			}
			try {
				std::to_string(this->set(tempBool));
				return 0;
			}
			catch (std::logic_error& e) {
				return 2;
			}
			break;
		case settingsType_integer:
			if (value.length() == 0) {
				try {
					 std::to_string(this->set(1));
					 return 0;
				}
				catch (std::logic_error& e) {
					return 2;
				}
			}
			try {
				tempInt = std::stoi(value);
			}
			catch (std::invalid_argument& e) {
				return 1;
			}
			catch (std::out_of_range& e) {
				return  1;
			}
			try {
				std::to_string(this->set(tempInt));
				return 0;
			}
			catch (std::logic_error& e) {
				return 2;
			}
			break;
		case settingsType_float:
			if (value.length() == 0) {
				try {
					std::to_string(this->set(1.0f));
					return 0;
				}
				catch (std::logic_error& e) {
					return 2;
				}
			}
			try {
				tempFloat = std::stof(value);
			}
			catch (std::invalid_argument& e) {
				return 1;
			}
			catch (std::out_of_range& e) {
				return 1;
			}
			try {
				std::to_string(this->set(tempFloat));
				return 0;
			}
			catch (std::logic_error& e) {
				return 2;
			}
			break;
		case settingsType_string:
			try {
				this->set(value);
				return 0;
			}
			catch (std::logic_error& e) {
				return 2;
			}
			break;
		default:
			return 2;
		}
	}
	std::string prettyPrint() {
		std::string string = "";
		switch (this->type) {
		case settingsType_boolean:
			string += std::string(this->getBool() ? "True" : "False") + "::Boolean";
			break;
		case settingsType_integer:
			string += std::to_string(this->getInt()) + "::Integer";
			break;
		case settingsType_float:
			string += std::to_string(this->getFloat()) + "::Float";
			break;
		case settingsType_string:
			string += std::string("\"") + this->getString() + "\"::String";
			break;
		default:
			string += "?::?";
		}
		if (this->callback != nullptr) {
			string += ", CALLBACK";
		}
		return string;
	}
	std::string serialize() {
		std::string string = "setting-set quote #" + this->name + " ";
		switch (this->type) {
		case settingsType_boolean:
			string += std::string(this->getBool() ? "true" : "false");
			break;
		case settingsType_integer:
			string += std::to_string(this->getInt());
			break;
		case settingsType_float:
			string += std::to_string(this->getFloat());
			break;
		case settingsType_string:
			string += std::string("\"") + this->getString() + "\"";
			break;
		default:
			string += "?::?";
		}
		return string;
	}
	bool lock() {
		this->locked = true;
		return this->locked;
	}
	bool unlock() {
		this->locked = false;
		return this->locked;
	}
};

class SettingsList {
private:
	std::vector<Setting> array;
	std::map<std::string, ptrdiff_t> map;
public:
	void insert(std::string name, Setting value) {
		map.insert(std::pair<std::string, ptrdiff_t>(name, array.size()));
		value.name = name;
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
	bool contains(std::string name) {
		return map.count(name) > 0;
	}
	auto begin() {
		return array.begin();
	}
	auto end() {
		return array.end();
	}
};


extern SettingsList *g_settings;

/* Settings */

#define SETTINGS_LIST \
	ENTRY(help, "", unlock, dont_save) \
	ENTRY(save, "", unlock, dont_save) \
	ENTRY(peer_ip_address, "localhost", unlock, save) \
	ENTRY(peer_network_port, 2850, unlock, save) \
	ENTRY(network_port, 2851, unlock, save) \
	ENTRY(disable_sdl, false, lock, save) \
	ENTRY(log_level, 5, unlock, save) \
	ENTRY(log_file, "", unlock, save) \
	ENTRY(config_compiler_heap_size, 1000000, lock, dont_save) \
	ENTRY(config_vm_heap_size, 1000000, lock, dont_save) \
	ENTRY(config_vm_max_objects, 1000, lock, dont_save) \
	ENTRY(game_compiler_heap_size, 1000000, unlock, save) \
	ENTRY(game_vm_heap_size, 1000000, lock, dont_save) \
	ENTRY(game_vm_max_objects, 1000, lock, dont_save) \
	ENTRY(gui_compiler_heap_size, 1000000, unlock, save) \
	ENTRY(gui_vm_heap_size, 1000000, lock, dont_save) \
	ENTRY(gui_vm_max_objects, 1000, lock, dont_save) \
	ENTRY(autoexec_script, "autoexec", lock, save) \
	ENTRY(config_script, "config", lock, save) \
	ENTRY(gui_script, "gui", lock, save) \
	ENTRY(scripts_directory, "../scripts/", lock, save) \
	ENTRY(game_root_directory, "../res/", lock, save) \
	ENTRY(disassemble, false, unlock, save) \
	ENTRY(repl, false, unlock, save) \
	ENTRY(repl_environment, "config", unlock, save) \
	ENTRY(switch_repl_environment, "", unlock, dont_save) \
	ENTRY(window_width, 640, unlock, save) \
	ENTRY(window_height, 480, unlock, save) \

#define SETTINGS_ALIAS_LIST \
	ENTRY('h', help) \
	ENTRY('a', peer_ip_address) \
	ENTRY('p', peer_network_port) \
	ENTRY('n', network_port) \
	ENTRY('s', disable_sdl) \
	ENTRY('l', log_level) \
	ENTRY('r', repl) \

#define ENTRY(ENTRY_name, ENTRY_value, ENTRY_lock, ENTRY_save) settingEnum_##ENTRY_name,
enum settingEnum_t {
	SETTINGS_LIST
};
#undef ENTRY

#endif // SETTINGS_H
