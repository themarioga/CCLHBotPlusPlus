#pragma once

#include "../Models/Game.h"
#include "../Exceptions/Player/PlayerAlreadyVoteDeleteException.h"

class GameService {

	public:

		GameService();
		~GameService();

		/**
		 * @brief Register a vote to delete a game
		 * 
		 * @param int64_t game_id
		 * @param int64_t player_id
		 */
		void VoteDelete(int64_t, int64_t);
		
		/**
		 * @brief Get the quantity of votes to delete a game
		 * 
		 * @param int64_t game_id
		 */
		static int32_t GetVoteDeletePlayerCount(int64_t);

		/**
		 * @brief Delete all the votes to delete a game
		 * 
		 * @param int64_t game_id
		 */
		void ClearVoteDelete(int64_t);

		/**
		 * @brief Get All Games
		 * 
		 * @return std::vector<Game> vector of games
		 */
		std::vector<Game> GetAllGames();
		
	private:

};