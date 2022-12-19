#include "pch.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Arguments.h"

Arguments::Arguments(std::string argument_string) {
	std::string arg;
	std::stringstream ss(argument_string);

	std::vector<std::string> args;
	while (ss >> arg) {
		args.push_back(arg);
	}

	if (args[0] == "redirect") {

		if (args.size() != 3) {
			// std::invalid_argument("Missing required arguments");
			this->invalid = true;
		}

		this->Action = args[0];
		this->FakeDstPort = std::stoi(args[1]);
		this->RedirectPort = std::stoi(args[2]);
	}
	else if (args[0] == "backdoor") {

		if (args.size() != 4) {
			// std::invalid_argument("Missing required arguments");
			this->invalid = true;
			return;
		}

		this->Action = args[0];
		this->FakeDstPort = std::stoi(args[1]);
		this->RedirectPort = std::stoi(args[2]);
		this->Password = args[3];
	}
	else if (args[0] == "remove") {
		this->Action = args[0];
		this->id = std::stoi(args[1]);
	}
	else if (args[0] == "list") {
		this->Action = args[0];
	}
	else {
		// std::invalid_argument("Please enter a valid action of redirect or backdoor");
		this->invalid = true;
	}
}