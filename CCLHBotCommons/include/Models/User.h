#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Exceptions/User/UserAlreadyExistsException.h"
#include "../Exceptions/User/UserNotExistsException.h"
#include "../Exceptions/User/UserNotActiveException.h"
#include "../Exceptions/UnexpectedException.h"

class User {

	public:
		//Constructor
		User();
		User(int64_t);
		User(int64_t, std::string);
		User(int64_t, std::string, uint8_t);

		//Destructor
		~User();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetName(std::string);
		void SetActive(bool);

		//Getters
		const int64_t GetID() { return id; }
		const std::string GetName() { return name; }
		const bool GetActive() { return active; }

	protected:
		int64_t id;
		std::string name;
		bool active;

};