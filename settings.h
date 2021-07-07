#ifndef SETTINGS_H
#define SETTINGS_H

#include <assert.h>
#include "array.h"

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
	void set(bool value) {
		this->valueBool = value;
	}
	void set(int value) {
		this->valueInt = value;
	}
	void set(float value) {
		this->valueFloat = value;
	}
	void set(std::string value) {
		*this->valueString = value;
	}
	void set(const char *value) {
		*this->valueString = value;
	}
};

class SettingsList: public Array<Setting> {
public:
	Setting *operator[](std::ptrdiff_t index) {
		return &array[index];
	}
	Setting *find(std::string name) {
		for (ptrdiff_t i = 0; i < array.size(); i++) {
			if (array[i].name == name) {
				return &array[i];
			}
		}
		throw std::logic_error("SettingsList::find");
	}
};

#endif // SETTINGS_H
