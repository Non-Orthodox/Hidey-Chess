#ifndef SETTINGS_H
#define SETTINGS_H

#include "array.h"

enum settingsType_t {
	settingsType_boolean,
	settingsType_integer,
	settingsType_float,
	settingsType_string
};

class Setting {
private:
	settingsType_t type = settingsType_boolean;
	union {
		bool valueBool = false;
		int valueInt;
		float valueFloat;
		std::string *valueString;
	};
public:
	std::string name = nullptr;
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
	~Setting() {
		name.clear();
	}
	template<typename T>
	T get() {
		switch (type) {
		case settingsType_boolean:
			return this->valueBool;
		case settingsType_integer:
			return this->valueInt;
		case settingsType_float:
			return this->valueFloat;
		case settingsType_string:
			return *this->valueString;
		}
	}
	template<typename T>
	void set(T value) {
		switch (type) {
		case settingsType_boolean:
			this->valueBool = value;
			break;
		case settingsType_integer:
			this->valueInt = value;
			break;
		case settingsType_float:
			this->valueFloat = value;
			break;
		case settingsType_string:
			*this->valueString = value;
			break;
		default:
			this->valueBool = 0;
		}
	}
};

class SettingsList: public Array<Setting> {
public:
	Setting find(std::string name) {
		for (ptrdiff_t i = 0; i < array.size(); i++) {
			if (array[i].name == name) {
				return array[i];
			}
		}
		throw;
	}
	// template<typename T>
	// void addSetting(std::string name, settingsType_t type, T value) {
	// 	Setting setting = new setting(name, type, value);
	// 	this.push(setting);
	// }
};

#endif // SETTINGS_H
