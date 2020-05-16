#pragma once

#include <cstdint>
#include "Card.h"
#include "Room.h"
#include "../Enums/GameTypeEnum.h"
#include "../Enums/GameStatusEnum.h"
#include "../Exceptions/Game/GameAlreadyExistsException.h"
#include "../Exceptions/Game/GameNotExistsException.h"
#include "../Exceptions/Game/GameAlreadyFilledException.h"
#include "../Exceptions/Game/GameNotFilledException.h"
#include "../Exceptions/Game/GameAlreadyStartedException.h"
#include "../Exceptions/Game/GameNotConfiguredException.h"
#include "../Exceptions/UnexpectedException.h"

class Game : public Room {

	public: 
		//Constructor
		Game();
		Game(int64_t);
		Game(int64_t, std::string, int64_t, int64_t, int64_t);
		
		//Autofill constructor
		Game(int64_t, std::string, int64_t, uint8_t, int64_t, int64_t, uint8_t, uint8_t, uint8_t, uint8_t, int64_t, int64_t, uint8_t);

		//Destructor
		~Game();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetCreatorID(int64_t);
		void SetMessageID(int64_t);
		void SetStatus(GameStatusEnum);
		void SetType(GameTypeEnum);
		void SetNumberOfPlayers(uint8_t);
		void SetNumberOfCardsToWin(uint8_t);
		void SetDictionaryID(int64_t);
		void SetPresidentID(int64_t);
		void SetRoundNumber(uint8_t);

		//Getters
		int64_t GetCreatorID() const { return creator_id; }
		int64_t GetMessageID() const { return message_id; }
		uint8_t GetStatus() const { return status; }
		uint8_t GetType() const { return type; }
		uint8_t GetNumberOfPlayers() const { return n_players; }
		uint8_t GetNumberOfCardsToWin() const { return n_cards_to_win; }
		int64_t GetDictionaryID() const { return dictionary_id; }
		int64_t GetPresidentID() const { return president_id; }
		uint8_t GetRoundNumber() const { return round_number; }

	private:
		int64_t creator_id;
		int64_t message_id;
		uint8_t status;
		uint8_t type;
		uint8_t n_players;
		uint8_t n_cards_to_win;
		int64_t dictionary_id;
		int64_t president_id;
		uint8_t round_number;

};
