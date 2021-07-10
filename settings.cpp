
#include "settings.h"
#include <string>
#include <iostream>

// * <value>
int settings_setFromString(Setting *setting, const std::string value, std::string *returnValue) {
	int error = 0;
	
	bool tempBool = false;
	int tempInt = 0;
	float tempFloat = 0.0;
	std::string dummy;
	
	if (returnValue == nullptr) {
		returnValue = &dummy;
	}
	
	switch (setting->type) {
	case settingsType_boolean:
		if (value.length() == 0) {
			try {
				*returnValue = std::to_string(setting->set(true));
			}
			catch (std::logic_error& e) {
				error = 2;
			}
		}
		try {
			// Mainly for fun.
			tempBool = !!std::stoi(value);
		}
		catch (std::invalid_argument& e) {
			std::cerr << "Could not convert option \"" << setting->name << "\"'s value \""
				<< value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
			error = 1;
			break;
		}
		catch (std::out_of_range& e) {
			std::cerr << "Could not convert option \"" << setting->name << "\"'s value \""
				<< value << "\" to a bool. Ignoring option. Exception: " << e.what() << std::endl;
			error = 1;
			break;
		}
		try {
			*returnValue = std::to_string(setting->set(tempBool));
		}
		catch (std::logic_error& e) {
			error = 2;
		}
		break;
	case settingsType_integer:
		if (value.length() == 0) {
			try {
				*returnValue = std::to_string(setting->set(1));
			}
			catch (std::logic_error& e) {
				error = 2;
			}
		}
		try {
			tempInt = std::stoi(value);
		}
		catch (std::invalid_argument& e) {
			std::cerr << "Could not convert option \"" << setting->name << "\"'s value \""
				<< value << "\" to an int. Ignoring option. Exception: " << e.what() << std::endl;
			error = 1;
			break;
		}
		catch (std::out_of_range& e) {
			std::cerr << "Could not convert option \"" << setting->name << "\"'s value \""
				<< value << "\" to an int. Ignoring option. Exception: " << e.what() << std::endl;
			error = 1;
			break;
		}
		try {
			*returnValue = std::to_string(setting->set(tempInt));
		}
		catch (std::logic_error& e) {
			error = 2;
		}
		break;
	case settingsType_float:
		if (value.length() == 0) {
			try {
				*returnValue = std::to_string(setting->set(1.0f));
			}
			catch (std::logic_error& e) {
				error = 2;
			}
		}
		try {
			tempFloat = std::stof(value);
		}
		catch (std::invalid_argument& e) {
			std::cerr << "Could not convert option \"" << setting->name << "\"'s value \""
				<< value << "\" to a float. Ignoring option. Exception: " << e.what() << std::endl;
			error = 1;
			break;
		}
		catch (std::out_of_range& e) {
			std::cerr << "Could not convert option \"" << setting->name << "\"'s value \""
				<< value << "\" to a float. Ignoring option. Exception: " << e.what() << std::endl;
			error = 1;
			break;
		}
		try {
			*returnValue = std::to_string(setting->set(tempFloat));
		}
		catch (std::logic_error& e) {
			error = 2;
		}
		break;
	case settingsType_string:
		try {
			*returnValue = setting->set(value);
		}
		catch (std::logic_error& e) {
			error = 2;
		}
		break;
	default:
		std::cerr << "Invalid type " << setting->type << " for setting \"" << setting->name << "\"." << std::endl;
		error = 2;
	}
	
	return error;
}

int settings_runSubcommands(std::string *line) {
	int error = 0;

	std::string varName;    // Overloaded.
	std::string value;
	std::string subcommand;
	std::string cleanedValue;
	std::string newLine = "";
	
	Setting *var = nullptr; // Overloaded.
	
	size_t openParen_index;
	size_t closeParen_index = -1;
	unsigned int parenLevel = 0;
	
	size_t spaceIndex;
	
	size_t line_length = line->length();
	
	while (1) {
	
		// Find start of next sub-command.
		openParen_index = line->find_first_of('(', closeParen_index + 1);
		if (openParen_index == std::string::npos) {
			// OK. There just aren't any more commands.
			newLine += line->substr(closeParen_index + 1);
			break;
		}
		
		newLine += line->substr(closeParen_index + 1, openParen_index - (closeParen_index + 1));
		
		// Find end of next sub-command.
		for (std::ptrdiff_t i = openParen_index; i < line_length; i++) {
			if ((*line)[i] == '(') {
				parenLevel++;
			}
			else if ((*line)[i] == ')') {
				--parenLevel;
				if (parenLevel == 0) {
					closeParen_index = i;
					// Found the end of the command.
					break;
				}
			}
		}
		
		if (parenLevel > 0) {
			std::cerr << "Unmatched parentheses. Parenthesis level: " << parenLevel << std::endl;
			error = 2;
			break;
		}
		
		if (closeParen_index < openParen_index) {
			// Not OK. This is a malformed command.
			// Don't set `newLine` because it's just a bad command. Can't do anything productive with it.
			error = 2;
			break;
		}
		
		subcommand = line->substr(openParen_index + 1, closeParen_index - (openParen_index + 1));
		// Shouldn't ever return an error.
		
		// Deal with sub-subcommands.
		settings_runSubcommands(&subcommand);
		
		/* Parse command */
		
		// Get the variable to be set.
		spaceIndex = subcommand.find_first_of(' ');
		// Don't check for `npos` now because no arguments may be fine.
		
		varName = subcommand.substr(0, spaceIndex);
		// Shouldn't return an error.
		
		// NOW we check.
		if (spaceIndex == std::string::npos) {
			value = ""; // No argument.
		}
		else {
			value = subcommand.substr(spaceIndex + 1, std::string::npos);
		}
		
		try {
			var = g_settings->find(varName);
		}
		catch (std::logic_error& e) {
			std::cerr << "(settings_runSubcommands) Could not find variable \"" << varName << "\"." << std::endl;
			error = 1;
			break;
		}
		
		// Set setting. Not our problem anymore. Let `var`'s callback deal with it.
		error = settings_setFromString(var, value, &cleanedValue);
		if (error) {
			break;
		}
		
		newLine += cleanedValue;
	}
	
	*line = newLine;
	
	return error;
}

// Usage:   set <var0> <value>
// Type:    std::string
/* Errors:
	0   OK
	1   Script error
	2   Type error
*/
int settings_callback_set(Setting *setting) {
	int error = 0;
	
	std::string varName;
	std::string value;
	std::string returnValue;
	
	Setting *var = nullptr;
	
	// SAVE THE STRING. IT WILL CHANGE WHEN SUBCOMMANDS ARE RUN.
	std::string line = setting->getString();
	size_t line_length = line.length();
	
	size_t spaceIndex;
	
	// Get the variable to be set.
	spaceIndex = line.find_first_of(' ');
	// Don't check for `npos` now because no arguments may be fine.
	
	varName = line.substr(0, spaceIndex);
	// Shouldn't return an error.
	
	// NOW we check.
	if (spaceIndex == std::string::npos) {
		value = ""; // No argument.
	}
	else {
		value = line.substr(spaceIndex + 1, std::string::npos);
	}
	
	try {
		var = g_settings->find(varName);
	}
	catch (std::logic_error& e) {
		std::cerr << "(settings_callback_set) Could not find variable \"" << varName << "\"." << std::endl;
		error = 1;
		return error;
	}
	
	// Set setting. Not our problem anymore. Let `var`'s callback deal with it.
	error = settings_setFromString(var, value, &returnValue);
	if (error) {
		return error;
	}
	
	// Set `set`'s value in order to return the `returnValue`.
	setting->callback = nullptr;    // HACK
	try {
		setting->set(returnValue);
	}
	catch (std::logic_error& e) {
		error = 2;
	}
	setting->callback = settings_callback_set;  // HACK
	
	return error;
}

int settings_callback_print(Setting *setting) {
	int error = 0;
	
	std::string varName;
	
	Setting *var = nullptr;
	
	// SAVE THE STRING. IT WILL CHANGE WHEN SUBCOMMANDS ARE RUN.
	std::string line = setting->getString();
	size_t line_length = line.length();
	
	varName = line.substr(0);
	// Shouldn't return an error.
	
	try {
		var = g_settings->find(varName);
	}
	catch (std::logic_error& e) {
		std::cerr << "(settings_callback_set) Could not find variable \"" << varName << "\"." << std::endl;
		error = 1;
		return error;
	}
	
	std::cout << varName << " (";
	
	switch (var->type) {
	case settingsType_boolean:
		std::cout << "bool): " << var->getBool();
		break;
	case settingsType_integer:
		std::cout << "int): " << var->getInt();
		break;
	case settingsType_float:
		std::cout << "float): " << var->getFloat();
		break;
	case settingsType_string:
		std::cout << "string): \"" << var->getString() << "\"";
		break;
	default:
		std::cerr << "(settings_callback_print) Invalid type " << setting->type << " for setting \"" << setting->name << "\"." << std::endl;
		error = 2;
	}
	
	std::cout << std::endl;
	
	return error;
}

int settings_callback_echo(Setting *setting) {
	int error = 0;
	
	std::cout << "\"" << setting->getString() << "\"" << std::endl;
	
	return error;
}

int settings_callback_chain(Setting *setting) {
	int error = 0;
	
	// SAVE THE STRING. IT WILL CHANGE WHEN SUBCOMMANDS ARE RUN.
	std::string line = setting->getString();
	size_t line_length = line.length();
	
	/* Run subcommands. */
	// This cleans out the parentheses.
	// This may change the value of `line`.
	error = settings_runSubcommands(&line);
	if (error) {
		return error;
	}
	
	return error;
}
