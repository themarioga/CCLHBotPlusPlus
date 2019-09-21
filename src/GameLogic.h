#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include "Models/User.h"
#include "Models/Room.h"
#include "Models/Player.h"
#include "Models/Game.h"
#include "Models/Card.h"
#include "Models/Dictionary.h"

#include "Services/UserService.h"
#include "Services/RoomService.h"
#include "Services/PlayerService.h"
#include "Services/GameService.h"
#include "Services/CardService.h"
#include "Services/DictionaryService.h"
#include "Services/ConfigurationService.h"

class GameLogic {

	public:

		GameLogic();
		~GameLogic();

		/**
		 * @brief Register a new user on the system
		 * 
		 * @param User user object
		 * @param std::function<void()> success callback
		 * @param std::function<void(std::string)> failure callback
		 */
		void RegisterUser(User&, std::function<void()>, std::function<void(std::string)>);
		

		void CreateGame(User&, Room&, int64_t, std::function<void()>, std::function<void(std::string)>);

		void SetGameType(User&, Game&, std::string, std::function<void()>, std::function<void(std::string)>);

		void SetGameNumberOfCardsToWin(User&, Game&, int8_t, std::function<void()>, std::function<void(std::string)>);

		void SetGameDictionary(User&, Game&, Dictionary&, std::function<void()>, std::function<void(std::string)>);

		void StartGame(User&, Game&, std::function<void()>, std::function<void(std::string)>);

		void StartGameRound(Game&, int64_t, std::function<void(Card&, std::map< int64_t, std::vector<Card> >&)>, std::function<void(std::string)>);

		void EndGameRound(Game&, Card&, std::function<void(Card&, Player&, std::vector<RoundWhiteCard>&, std::vector<Player>&)>, std::function<void(std::string)>);

		void DeleteGame(User&, Game&, std::function<void(std::vector< std::pair<int64_t, int64_t> >&)>, std::function<void(std::string)>);

		void VoteToDeleteGame(User&, Game&, std::function<void(int8_t)>, std::function<void(std::string)>);


		void CreatePlayer(Player&, Game&, std::function<void()>, std::function<void(std::string)>);

		void CreatePlayerWithChecks(Player&, Game&, std::function<void(std::vector<Player>&)>, std::function<void(std::string)>);

		
		void SelectWhiteCard(Player&, Card&, int64_t, std::function<void(Game&, std::vector<Player>&, RoundBlackCard&, std::vector<RoundWhiteCard>&)>, std::function<void(std::string)>);
		
		void VoteWhiteCard(Player&, Card&, int64_t, std::function<void(Game&, std::vector<Player>&, RoundBlackCard&)>, std::function<void(std::string)>);


		void GetDictionaries(int8_t, std::function<void(int64_t, std::vector<Dictionary>&)>, std::function<void(std::string)>);

	private:

		CardService cardService;
		ConfigurationService configurationService;
		DictionaryService dictionaryService;
		GameService gameService;
		PlayerService playerService;
		RoomService roomService;
		UserService userService;

		void DeleteAllGameReferences(Game&);

		void CreateOrLoadRoom(Room&);

};