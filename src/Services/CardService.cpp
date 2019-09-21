#include "CardService.h"
#include "../Db.h"

CardService::CardService() {

}

CardService::~CardService() {

}

void CardService::AddWhiteCardsToPlayerDeck(int64_t dictionary_id, std::vector<Player>& players, int32_t cardsPerPlayer_quantity) {
	int32_t numberOfWhiteCards = players.size() * cardsPerPlayer_quantity;

	//Query
	std::string q("SELECT id, type, text, dictionary_id FROM cards WHERE type=? AND dictionary_id=? ORDER BY RANDOM() LIMIT "+std::to_string(numberOfWhiteCards));
	std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(q);
	query->bind(1, CardTypeEnum::CARD_WHITE);
	query->bind(2, dictionary_id);

	//Get the cards
	std::vector<Card> cards;
	while (query->executeStep()) {
		Card card = query->getColumns<Card, 4>();
		cards.push_back(card);
	}

	//Check if there are cards
	if (cards.size() == 0) throw DictionaryNotExistsException();

	//Insert the cards
	try {
		std::size_t currentPosition = 0;
		for (Player player : players) {
			std::size_t const nextPosition = currentPosition + cardsPerPlayer_quantity;
			std::vector<Card> cardsPerPlayer(cards.begin()+currentPosition, cards.begin()+nextPosition);

			std::string q("INSERT INTO playersxgamesxcards_whitecards (player_id, game_id, card_id, card_order) VALUES ");
			for (int i = 0; i < cardsPerPlayer.size(); i++) {
				q += ("("+std::to_string(player.GetID())+","+std::to_string(player.GetGameID())+","+std::to_string(cardsPerPlayer[i].GetID())+", "+std::to_string(i)+")");
				if (i != cardsPerPlayer.size() - 1) q += ",";
			}

			std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(q);
			
			if (!query->exec()) throw NoResultsException();
			currentPosition = nextPosition;
		}
	} catch (NoResultsException& e) {
		throw CardAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw CardAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Card> CardService::GetWhiteCardsFromPlayerDeck(int64_t player_id, int32_t whitecards_in_hand) {
	try {
		std::vector<Card> cards;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT cards.id, cards.type, cards.text, cards.dictionary_id "
			"FROM playersxgamesxcards_whitecards INNER JOIN cards ON cards.id = playersxgamesxcards_whitecards.card_id "
			"WHERE playersxgamesxcards_whitecards.player_id = ?"
			"ORDER BY card_order LIMIT ?"
		);
		query->bind(1, player_id);
		query->bind(2, whitecards_in_hand);
		
		while (query->executeStep()) {
			Card card = query->getColumns<Card, 4>();
			cards.push_back(card);
		}

		if (cards.size() == 0) throw NoResultsException();
		
		return cards;
	} catch (NoResultsException& e) {
		throw PlayerNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteAllWhiteCardsFromAllPlayerDecks(int64_t game_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM playersxgamesxcards_whitecards WHERE game_id=?");
		query->bind(1, game_id);
		
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled Exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteAllWhiteCardsFromPlayerDeck(int64_t player_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM playersxgamesxcards_whitecards WHERE player_id=?");
		query->bind(1, player_id);
		
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled Exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteWhiteCardFromPlayerDeck(int64_t player_id, int64_t card_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"DELETE FROM playersxgamesxcards_whitecards "
			"WHERE player_id = ? AND card_id = ?"
		);
		query->bind(1, player_id);
		query->bind(2, card_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::AddWhiteCardToCurrentRound(int64_t player_id, int64_t game_id, int64_t card_id, int64_t message_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"INSERT INTO playersxgamesxcards_roundwhitecards(player_id, game_id, card_id, message_id) "
			"VALUES (?, ?, ?, ?)"
		);
		query->bind(1, player_id);
		query->bind(2, game_id);
		query->bind(3, card_id);
		query->bind(4, message_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw CardAlreadyUsedException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<RoundWhiteCard> CardService::GetWhiteCardsFromCurrentRound(int64_t game_id) {
	try {
		std::vector<RoundWhiteCard> cards;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT cards.id, cards.type, cards.text, cards.dictionary_id, "
			"playersxgamesxcards_roundwhitecards.game_id, playersxgamesxcards_roundwhitecards.player_id, playersxgamesxcards_roundwhitecards.message_id "
			"FROM playersxgamesxcards_roundwhitecards INNER JOIN cards ON cards.id = playersxgamesxcards_roundwhitecards.card_id "
			"WHERE playersxgamesxcards_roundwhitecards.game_id = ?"
		);
		query->bind(1, game_id);
		
		while (query->executeStep()) {
			RoundWhiteCard card = query->getColumns<RoundWhiteCard, 7>();
			cards.push_back(card);
		}

		if (cards.size() == 0) throw NoResultsException();
		
		return cards;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteAllWhiteCardsFromCurrentRound(int64_t game_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM playersxgamesxcards_roundwhitecards WHERE game_id=?");
		query->bind(1, game_id);
		
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled Exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::AddVoteToCurrentRound(int64_t game_id, int64_t player_id, int64_t card_id, int64_t message_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"INSERT INTO playersxgamesxcards_roundvotes(player_id, game_id, card_id, message_id) "
			"VALUES (?, ?, ?, ?)"
		);
		query->bind(1, player_id);
		query->bind(2, game_id);
		query->bind(3, card_id);
		query->bind(4, message_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw CardAlreadyUsedException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

Card CardService::GetMostVotedCardAtCurrentRound(int64_t game_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT cards.id, cards.type, cards.text, cards.dictionary_id, COUNT(card_id) AS value_occurrence "
			"FROM playersxgamesxcards_roundvotes INNER JOIN cards ON cards.id = playersxgamesxcards_roundvotes.card_id "
			"WHERE playersxgamesxcards_roundvotes.game_id = ? "
			"GROUP BY playersxgamesxcards_roundvotes.card_id "
			"ORDER BY value_occurrence DESC "
			"LIMIT 1"
		);
		query->bind(1, game_id);
		if (!query->executeStep()) throw NoResultsException();

		return query->getColumns<Card, 4>();
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<RoundWhiteCard> CardService::GetVotesFromCurrentRound(int64_t game_id) {
	try {
		std::vector<RoundWhiteCard> cards;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT cards.id, cards.type, cards.text, cards.dictionary_id, "
			"playersxgamesxcards_roundvotes.game_id, playersxgamesxcards_roundvotes.player_id, playersxgamesxcards_roundvotes.message_id "
			"FROM playersxgamesxcards_roundvotes INNER JOIN cards ON cards.id = playersxgamesxcards_roundvotes.card_id "
			"WHERE playersxgamesxcards_roundvotes.game_id = ?"
		);
		query->bind(1, game_id);
		
		while (query->executeStep()) {
			RoundWhiteCard card = query->getColumns<RoundWhiteCard, 7>();
			cards.push_back(card);
		}

		if (cards.size() == 0) throw NoResultsException();
		
		return cards;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

int64_t CardService::GetVoteMessageIDFromCurrentRound(int64_t player_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT message_id FROM playersxgamesxcards_roundvotes "
			"WHERE player_id = ?"
		);
		query->bind(1, player_id);
		if (!query->executeStep()) throw NoResultsException();

		return query->getColumn("message_id");
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteAllVotesFromCurrentRound(int64_t game_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM playersxgamesxcards_roundvotes WHERE game_id=?");
		query->bind(1, game_id);
		
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled Exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::AddBlackCardsToGameDeck(int64_t dictionary_id, int64_t game_id, int32_t quantity) {
	//Query
	std::string q("SELECT id, type, text, dictionary_id FROM cards WHERE type=? AND dictionary_id=? ORDER BY RANDOM() LIMIT "+std::to_string(quantity));
	std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(q);
	query->bind(1, CardTypeEnum::CARD_BLACK);
	query->bind(2, dictionary_id);

	//Get the cards
	std::vector<Card> cards;
	while (query->executeStep()) {
		Card card = query->getColumns<Card, 4>();
		cards.push_back(card);
	}

	//Check if there are cards
	if (cards.size() == 0) throw DictionaryNotExistsException();

	//Insert the cards
	try {
		std::string q("INSERT INTO gamesxcards_blackcards (game_id, card_id, card_order) VALUES ");
		for (int i = 0; i < cards.size(); i++) {
			q += ("("+std::to_string(game_id)+","+std::to_string(cards[i].GetID())+", "+std::to_string(i)+")");
			if (i != cards.size() - 1) q += ",";
		}
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(q);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw CardAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw CardAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

Card CardService::GetFirstBlackCardFromGameDeck(int64_t game_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT cards.id, cards.type, cards.text, cards.dictionary_id "
			"FROM gamesxcards_blackcards INNER JOIN cards ON cards.id = gamesxcards_blackcards.card_id "
			"WHERE gamesxcards_blackcards.game_id = ?"
			"ORDER BY card_order LIMIT 1"
		);
		query->bind(1, game_id);
		if (!query->executeStep()) throw NoResultsException();

		return query->getColumns<Card, 4>();
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteBlackCardsFromGameDeck(int64_t game_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM gamesxcards_blackcards WHERE game_id=?");
		query->bind(1, game_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled Exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteBlackCardFromGameDeck(int64_t card_id, int64_t game_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"DELETE FROM gamesxcards_blackcards "
			"WHERE card_id = ? AND game_id = ?"
		);
		query->bind(1, card_id);
		query->bind(2, game_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::AddBlackCardToCurrentRound(int64_t game_id, int64_t card_id, int64_t message_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"INSERT INTO gamesxcards_roundblackcards(game_id, card_id, message_id) "
			"VALUES (?, ?, ?)"
		);
		query->bind(1, game_id);
		query->bind(2, card_id);
		query->bind(3, message_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw CardAlreadyUsedException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

RoundBlackCard CardService::GetBlackCardFromCurrentRound(int64_t game_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT cards.id, cards.type, cards.text, cards.dictionary_id, "
			"gamesxcards_roundblackcards.game_id, gamesxcards_roundblackcards.message_id "
			"FROM gamesxcards_roundblackcards INNER JOIN cards ON cards.id = gamesxcards_roundblackcards.card_id "
			"WHERE gamesxcards_roundblackcards.game_id = ?"
		);
		query->bind(1, game_id);
		if (!query->executeStep()) throw NoResultsException();

		return query->getColumns<RoundBlackCard, 6>();
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void CardService::DeleteBlackCardFromCurrentRound(int64_t game_id) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM gamesxcards_roundblackcards WHERE game_id=?");
		query->bind(1, game_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled Exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Player> CardService::GetPlayersThatSentWhiteCards(int64_t game_id) {
	try {
		std::vector<Player> players;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM playersxgamesxcards_roundwhitecards "
			"INNER JOIN users ON users.id = playersxgamesxcards_roundwhitecards.player_id "
			"INNER JOIN players ON players.user_id = playersxgamesxcards_roundwhitecards.player_id "
			"WHERE playersxgamesxcards_roundwhitecards.game_id = ?"
		);
		query->bind(1, game_id);
		
		while (query->executeStep()) {
			Player player = query->getColumns<Player, 7>();
			players.push_back(player);
		}

		if (players.size() == 0) throw NoResultsException();
		
		return players;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Player> CardService::GetPlayersThatVotedWhiteCards(int64_t game_id) {
	try {
		std::vector<Player> players;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM playersxgamesxcards_roundvotes "
			"INNER JOIN users ON users.id = playersxgamesxcards_roundvotes.player_id "
			"INNER JOIN players ON players.user_id = playersxgamesxcards_roundvotes.player_id "
			"WHERE playersxgamesxcards_roundvotes.game_id = ?"
		);
		query->bind(1, game_id);
		
		while (query->executeStep()) {
			Player player = query->getColumns<Player, 7>();
			players.push_back(player);
		}

		if (players.size() == 0) throw NoResultsException();
		
		return players;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

Player CardService::GetPlayerFromRoundWhiteCardID(int64_t card_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM playersxgamesxcards_roundwhitecards "
			"INNER JOIN users ON users.id = playersxgamesxcards_roundwhitecards.player_id "
			"INNER JOIN players ON players.user_id = playersxgamesxcards_roundwhitecards.player_id "
			"WHERE playersxgamesxcards_roundwhitecards.card_id = ?"
		);
		query->bind(1, card_id);

		//Execute query
		if (!query->executeStep()) throw NoResultsException();
		
		//Create player object
		Player player = query->getColumns<Player, 7>();
		
		return player;
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Card> CardService::GetAllCards() {
	try {
		std::vector<Card> cards;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, active FROM cards");
		while (query->executeStep()) {
			Card card = query->getColumns<Card, 3>();
			cards.push_back(card);
		}
		
		return cards;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Card> CardService::GetAllCardsByType(CardTypeEnum type) {
	try {
		std::vector<Card> cards;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, active FROM cards WHERE type = ?");
		query->bind(1, type);
		while (query->executeStep()) {
			Card card = query->getColumns<Card, 3>();
			cards.push_back(card);
		}
		
		return cards;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}