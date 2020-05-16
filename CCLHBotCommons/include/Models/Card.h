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

		//Copy constructor
		Card(const Card &);

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
		CardTypeEnum GetType() const { return type; }
		int64_t GetID() const { return id; }
		std::string GetText() const { return text; }
		int64_t GetDictionaryID() const { return dictionary_id; }

		//Ifstream
    	friend std::ostream& operator<<(std::ostream& os, const Card& dt);

	private:
		int64_t id;
		CardTypeEnum type;
		std::string text;
		int64_t dictionary_id;

};