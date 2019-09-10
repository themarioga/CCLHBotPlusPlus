#pragma once

#include "../Models/Card.h"
#include "../Models/Player.h"
#include "../Exceptions/Card/CardAlreadyUsedException.h"
#include "../Exceptions/Dictionary/DictionaryNotExistsException.h"
#include "../Exceptions/Game/GameNotExistsException.h"
#include "../Exceptions/Player/PlayerNotExistsException.h"

struct RoundBlackCard {
	Card card;
	int64_t game_id;
	int64_t message_id;

	RoundBlackCard(int64_t id, uint8_t type, std::string text, int64_t dictionary_id, int64_t game_id, int64_t message_id) {
		this->card = Card(id, type, text, dictionary_id);
		this->game_id = game_id;
		this->message_id = message_id;
	}
};

struct RoundWhiteCard {
	Card card;
	int64_t game_id;
	int64_t player_id;
	int64_t message_id;

	RoundWhiteCard(int64_t id, uint8_t type, std::string text, int64_t dictionary_id, int64_t game_id, int64_t player_id, int64_t message_id) {
		this->card = Card(id, type, text, dictionary_id);
		this->game_id = game_id;
		this->player_id = player_id;
		this->message_id = message_id;
	}
};

class CardService {

	public:
		CardService();
		~CardService();

		////////////////////////////////////////////////////////////DECK WHITE CARDS/////////////////////////////////////////////////////////////////

		/**
		 * @brief Add all the white cards to a player's deck
		 * 
		 * @param int64_t dictionary id
		 * @param std::vector<Player>& players
		 * @param int32_t quantity of cards we want to add to each player
		 */
		void AddWhiteCardsToPlayerDeck(int64_t, std::vector<Player>&, int32_t);

		/**
		 * @brief Get the White Cards from the player's deck
		 * 
		 * @param int64_t player id
		 * @param int32_t number of cards in hand
		 * @return std::vector<Card> 
		 */
		std::vector<Card> GetWhiteCardsFromPlayerDeck(int64_t, int32_t);

		/**
		 * @brief Delete all the white cards from all the player's decks
		 * 
		 * @param int64_t game id
		 */
		void DeleteAllWhiteCardsFromAllPlayerDecks(int64_t);

		/**
		 * @brief Delete all the white cards from the player's deck
		 * 
		 * @param int64_t player id
		 */
		void DeleteAllWhiteCardsFromPlayerDeck(int64_t);

		/**
		 * @brief Delete a white card from the player's deck
		 * 
		 * @param int64_t player id
		 * @param int64_t card id
		 */
		void DeleteWhiteCardFromPlayerDeck(int64_t, int64_t);
		
		////////////////////////////////////////////////////////////ROUND WHITE CARDS/////////////////////////////////////////////////////////////////

		/**
		 * @brief Add a white card to current round of the game
		 * 
		 * @param int64_t player id
		 * @param int64_t game id
		 * @param int64_t card id
		 * @param int64_t message id
		 */
		void AddWhiteCardToCurrentRound(int64_t, int64_t, int64_t, int64_t);

		/**
		 * @brief Get the White Cards from the current round of the game
		 * 
		 * @param int64_t game id
		 * @return std::vector<Card> 
		 */
		std::vector<RoundWhiteCard> GetWhiteCardsFromCurrentRound(int64_t);
		
		/**
		 * @brief Delete all the white cards from the current round of the game
		 * 
		 * @param int64_t game id
		 */
		void DeleteAllWhiteCardsFromCurrentRound(int64_t);
		
		////////////////////////////////////////////////////////////ROUND VOTES/////////////////////////////////////////////////////////////////

		/**
		 * @brief Add a new vote
		 * 
		 * @param int64_t game id
		 * @param int64_t player id
		 * @param int64_t card id
		 * @param int64_t message id
		 */
		void AddVoteToCurrentRound(int64_t, int64_t, int64_t, int64_t);

		/**
		 * @brief Get the most voted card in the current round of the game
		 * 
		 * @param int64_t game id
		 * @return Card
		 */
		Card GetMostVotedCardAtCurrentRound(int64_t);
		
		/**
		 * @brief Get the Message ID From Current Round
		 * 
		 * @param int64_t player id
		 * @return int64_t message id
		 */
		int64_t GetVoteMessageIDFromCurrentRound(int64_t);

		/**
		 * @brief Get the cards voted in the current round of the game
		 * 
		 * @param int64_t game id
		 * @return std::vector<Card> 
		 */
		std::vector<RoundWhiteCard> GetVotesFromCurrentRound(int64_t);

		/**
		 * @brief Delete all the votes from the current round of the game
		 * 
		 * @param int64_t game id
		 */
		void DeleteAllVotesFromCurrentRound(int64_t);
		
		////////////////////////////////////////////////////////////DECK BLACK CARDS/////////////////////////////////////////////////////////////////
		
		/**
		 * @brief Add all the black cards to a game
		 * 
		 * @param int64_t dictionary id
		 * @param int64_t game id
		 * @param int32_t quantity of cards we want to add
		 */
		void AddBlackCardsToGameDeck(int64_t, int64_t, int32_t);

		/**
		 * @brief Get the First Black Card from a game
		 * 
		 * @param int64_t game id
		 * @return Card 
		 */
		Card GetFirstBlackCardFromGameDeck(int64_t);

		/**
		 * @brief Delete all the black cards from a game
		 * 
		 * @param int64_t game id
		 */
		void DeleteBlackCardsFromGameDeck(int64_t);

		/**
		 * @brief Delete the first black card from a game
		 * 
		 * @param int64_t card id
		 * @param int64_t game id
		 */
		void DeleteBlackCardFromGameDeck(int64_t, int64_t);
		
		////////////////////////////////////////////////////////////ROUND BLACK CARDS/////////////////////////////////////////////////////////////////

		/**
		 * @brief Add a black card to current round of the game
		 * 
		 * @param int64_t game id
		 * @param int64_t card id
		 * @param int64_t message id
		 */
		void AddBlackCardToCurrentRound(int64_t, int64_t, int64_t);

		/**
		 * @brief Get the Black Card from the current round of the game
		 * 
		 * @param int64_t game id
		 * @return Card
		 */
		RoundBlackCard GetBlackCardFromCurrentRound(int64_t);

		/**
		 * @brief Delete a black card from current round
		 * 
		 * @param int64_t game id
		 */
		void DeleteBlackCardFromCurrentRound(int64_t);
		
		////////////////////////////////////////////////////////////PLAYERS/////////////////////////////////////////////////////////////////

		/**
		 * @brief Get the Players that sent white cards in the current round
		 * 
		 * @param int64_t game id
		 * @return std::vector<Card> 
		 */
		std::vector<Player> GetPlayersThatSentWhiteCards(int64_t);

		/**
		 * @brief Get the Players that voted white cards in the current round
		 * 
		 * @param int64_t game id
		 * @return std::vector<Card> 
		 */
		std::vector<Player> GetPlayersThatVotedWhiteCards(int64_t);

		/**
		 * @brief Get the Player From Round White Card ID
		 * 
		 * @param int64_t card id
		 * @return Player 
		 */
		Player GetPlayerFromRoundWhiteCardID(int64_t);
		
		////////////////////////////////////////////////////////////UTILS/////////////////////////////////////////////////////////////////
		
		/**
		 * @brief Get All Cards
		 * 
		 * @return std::vector<Card> vector of cards
		 */
		std::vector<Card> GetAllCards();
		
		/**
		 * @brief Get All Cards By Type
		 * 
		 * @param CardTypeEnum card type
		 * @return std::vector<Card> vector of cards
		 */
		std::vector<Card> GetAllCardsByType(CardTypeEnum);

	private:

};