#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Exceptions/Dictionary/DictionaryAlreadyExistsException.h"
#include "../Exceptions/Dictionary/DictionaryNotExistsException.h"
#include "../Exceptions/UnexpectedException.h"

class Dictionary {

	public:
		//Constructors
		Dictionary();
		Dictionary(int64_t);
		Dictionary(std::string, int64_t, int64_t);
		
		//Autofill Constructor
		Dictionary(int64_t, std::string, int64_t, int64_t, uint8_t);

		//Destructor
		~Dictionary();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetName(std::string);
		void SetCreatorID(int64_t);
		void SetMessageID(int64_t);
		void SetPublished(bool);

		//Getters
		int64_t GetID() { return id; }
		std::string GetName() { return name; }
		int64_t GetCreatorID() { return creator_id; }
		int64_t GetMessageID() { return message_id; }
		bool IsPublished() { return published; }

	private:
		int64_t id;
		std::string name;
		int64_t creator_id;
		int64_t message_id;
		bool published;

};