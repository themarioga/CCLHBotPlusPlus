#pragma once

#include "../Models/Player.h"
#include "../Exceptions/Game/GameNotExistsException.h"

class PlayerService {

	public:
		
		PlayerService();
		~PlayerService();

		/**
		 * @brief Get the Players From Array ID object
		 * 
		 * @std::vector<int64_t> vector with player ids
		 * @return std::vector<Player> vector with players
		 */
		std::vector<Player> GetPlayersByIDs(std::vector<int64_t>);

		/**
		 * @brief Delete all the players in a game
		 * 
		 * @param int64_t game_id
		 */
		void DeletePlayersInGame(int64_t);

		/**
		 * @brief Get the Players In Game
		 * 
		 * @param int64_t game id
		 * @return std::vector<Player> 
		 */
		std::vector<Player> GetPlayersInGame(int64_t);

		/**
		 * @brief Get the Count of Players In Game 
		 * 
		 * @param int64_t game id
		 * @return int32_t 
		 */
		int32_t GetPlayersInGameCount(int64_t);

		/**
		 * @brief Get the Player From Array object
		 * 
		 * @std::vector<Player> vector with players
		 * @int64_t playerID
		 * @return Player
		 */
		int64_t GetPlayerIndexFromPlayerArray(std::vector<Player>&, int64_t);

		/**
		 * @brief Get All Players
		 * 
		 * @return std::vector<Player> vector of players
		 */
		std::vector<Player> GetAllPlayers();
		
	private:

};