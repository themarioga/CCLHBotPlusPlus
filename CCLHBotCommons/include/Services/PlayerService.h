#pragma once

#include "../Models/Player.h"
#include "../Exceptions/Game/GameNotExistsException.h"

class PlayerService {

	public:

		/**
		 * @brief Get the Players From Array ID object
		 * 
		 * @std::vector<int64_t> vector with player ids
		 * @return std::vector<Player> vector with players
		 */
		static std::vector<Player> GetPlayersByIDs(std::vector<int64_t>);

		/**
		 * @brief Delete all the players in a game
		 * 
		 * @param int64_t game_id
		 */
		static void DeletePlayersInGame(int64_t);

		/**
		 * @brief Get the Players In Game
		 * 
		 * @param int64_t game id
		 * @return std::vector<Player> 
		 */
		static std::vector<Player> GetPlayersInGame(int64_t);

		/**
		 * @brief Get the Count of Players In Game 
		 * 
		 * @param int64_t game id
		 * @return int32_t 
		 */
		static int32_t GetPlayersInGameCount(int64_t);

		/**
		 * @brief Get the Player From Array object
		 * 
		 * @std::vector<Player> vector with players
		 * @int64_t playerID
		 * @return Player
		 */
		static int64_t GetPlayerIndexFromPlayerArray(std::vector<Player>&, int64_t);

		/**
		 * @brief Get All Players
		 * 
		 * @return std::vector<Player> vector of players
		 */
		static std::vector<Player> GetAllPlayers();
		
	private:

};