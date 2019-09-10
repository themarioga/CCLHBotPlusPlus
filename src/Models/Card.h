#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Enums/CardTypeEnum.h"
#include "../Exceptions/Card/CardAlreadyExistsException.h"
#include "../Exceptions/Card/CardNotExistsException.h"
#include "../Exceptions/UnexpectedException.h"

class Card {

	public:
		//Constructor
		Card();
		Card(int64_t);
		Card(uint8_t, std::string, int64_t);

		//Autofill constructor
		Card(int64_t, uint8_t, std::string, int64_t);

		//Destructor
		~Card();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetType(CardTypeEnum);
		void SetText(std::string);
		void SetDictionaryID(int64_t);

		//Getters
		CardTypeEnum SetType() { return type; }
		int64_t GetID() { return id; }
		std::string GetText() { return text; }
		int64_t GetDictionaryID() { return dictionary_id; }

	private:
		int64_t id;
		CardTypeEnum type;
		std::string text;
		int64_t dictionary_id;

};