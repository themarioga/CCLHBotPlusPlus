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
		Dictionary(std::string, int64_t);
		
		//Autofill Constructor
		Dictionary(int64_t, std::string, int64_t, uint8_t, uint8_t);

		//Copy constructor
		Dictionary(const Dictionary &);

		//Destructor
		~Dictionary();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetName(std::string);
		void SetCreatorID(int64_t);
		void SetShared(bool);
		void SetPublished(bool);

		//Getters
		int64_t GetID() const { return id; }
		std::string GetName() const { return name; }
		int64_t GetCreatorID() const { return creator_id; }
		bool IsShared() const { return shared; }
		bool IsPublished() const { return published; }

	private:
		int64_t id;
		std::string name;
		int64_t creator_id;
		bool shared;
		bool published;

};