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

		/**
		 * @brief Register a new user on the system
		 * 
		 * @param User user object
		 * @param std::function<void()> success callback
		 * @param std::function<void(std::string)> failure callback
		 */
		static void RegisterUser(User&, std::function<void()>, std::function<void(std::string)>);
		

		static void CreateGame(User&, Room&, int64_t, std::function<void()>, std::function<void(std::string)>);

		static void SetGameType(User&, Game&, std::string, std::function<void()>, std::function<void(std::string)>);

		static void SetGameNumberOfCardsToWin(User&, Game&, int8_t, std::function<void()>, std::function<void(std::string)>);

		static void SetGameDictionary(User&, Game&, Dictionary&, std::function<void()>, std::function<void(std::string)>);

		static void StartGame(User&, Game&, std::function<void()>, std::function<void(std::string)>);

		static void StartGameRound(Game&, int64_t, std::function<void(Card&, std::map< int64_t, std::vector<Card> >&)>, std::function<void(std::string)>);

		static void EndGameRound(Game&, Card&, std::function<void(Card&, Player&, std::vector<RoundWhiteCard>&, std::vector<Player>&)>, std::function<void(std::string)>);

		static void DeleteGame(User&, Game&, std::function<void(std::vector< std::pair<int64_t, int64_t> >&)>, std::function<void(std::string)>);

		static void VoteToDeleteGame(User&, Game&, std::function<void(int8_t)>, std::function<void(std::string)>);

		static void DeleteGameByCreator(User&, Game&, std::function<void(std::vector< std::pair<int64_t, int64_t> >&)>, std::function<void(std::string)>);

		static void LeaveGame(User&, Game&, std::function<void()>, std::function<void(std::string)>);


		static void CreatePlayer(Player&, Game&, std::function<void()>, std::function<void(std::string)>);

		static void CreatePlayerWithChecks(Player&, Game&, std::function<void(std::vector<Player>&)>, std::function<void(std::string)>);

		
		static void SelectWhiteCard(Player&, Card&, int64_t, std::function<void(Game&, std::vector<Player>&, RoundBlackCard&, std::vector<RoundWhiteCard>&)>, std::function<void(std::string)>);
		
		static void VoteWhiteCard(Player&, Card&, int64_t, std::function<void(Game&, std::vector<Player>&, RoundBlackCard&)>, std::function<void(std::string)>);


		static void GetDictionaries(int64_t, int8_t, std::function<void(int64_t, std::vector<Dictionary>&)>, std::function<void(std::string)>);

		static std::string GetPlayerNamesFromPlayerArray(std::vector<Player>&, std::string="", std::string="");

		static std::string GetCardsTextFromCardArray(std::vector<RoundWhiteCard>&, std::string="", std::string="");
	private:

		static std::vector< std::pair<int64_t, int64_t> > GetMessageIDsFromGame(Game&);

		static void DeleteAllGameReferences(Game&);

		static void CreateOrLoadRoom(Room&);

};