#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Exceptions/Room/RoomAlreadyExistsException.h"
#include "../Exceptions/Room/RoomNotExistsException.h"
#include "../Exceptions/Room/RoomNotActiveException.h"
#include "../Exceptions/UnexpectedException.h"

class Room {

	public:
		//Constructor
		Room();
		Room(int64_t);
		Room(int64_t, std::string, int64_t);
		Room(int64_t, std::string, int64_t, uint8_t);

		//Destructor
		~Room();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetName(std::string);
		void SetOwnerID(int64_t);
		void SetActive(bool);

		//Getters
		int64_t GetID() { return id; }
		std::string GetName() { return name; }
		int64_t GetOwnerID() { return owner_id; }
		bool GetActive() { return active; }

	protected:
		int64_t id;
		std::string name;
		int64_t owner_id;
		bool active;

};