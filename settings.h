#ifndef SETTINGS_H
#define SETTINGS_H

#include <assert.h>
#include <vector>
#include <string>
#include <stdexcept>
// #include "array.h"

enum settingsType_t {
	settingsType_boolean,
	settingsType_integer,
	settingsType_float,
	settingsType_string
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
	Setting(std::string name, int value) {
		this->name = name;
		this->type = settingsType_integer;
		this->valueInt = value;
	}
	Setting(std::string name, float value) {
		this->name = name;
		this->type = settingsType_float;
		this->valueFloat = value;
	}
	Setting(std::string name, std::string value) {
		this->name = name;
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
			callback(this);
		}
		return this->valueBool;
	}
	int set(int value) {
		assert(type == settingsType_integer);
		this->valueInt = value;
		if (callback != nullptr) {
			callback(this);
		}
		return this->valueInt;
	}
	float set(float value) {
		assert(type == settingsType_float);
		this->valueFloat = value;
		if (callback != nullptr) {
			callback(this);
		}
		return this->valueFloat;
	}
	std::string set(std::string value) {
		assert(type == settingsType_string);
		*this->valueString = value;
		if (callback != nullptr) {
			callback(this);
		}
		return *this->valueString;
	}
	void set(const char *value) {
		assert(type == settingsType_string);
		*this->valueString = value;
		if (callback != nullptr) {
			callback(this);
		}
	}
};

class SettingsList {
private:
	std::vector<Setting> array;
public:
	void push(Setting value) {
		array.push_back(value);
	}
	Setting *operator[](std::ptrdiff_t index) {
		return &array[index];
	}
	Setting *find(std::string name) {
		for (std::ptrdiff_t i = 0; i < array.size(); i++) {
			if (array[i].name == name) {
				return &array[i];
			}
		}
		throw std::logic_error("SettingsList::find");
	}
};


extern SettingsList *g_settings;

int settings_callback_set(Setting *setting);
int settings_callback_print(Setting *setting);
int settings_callback_echo(Setting *setting);
int settings_callback_chain(Setting *setting);

#endif // SETTINGS_H
