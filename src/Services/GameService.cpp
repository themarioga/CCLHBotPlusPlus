#include "GameService.h"
#include "../Db.h"

GameService::GameService() {

}

GameService::~GameService() {

}

void GameService::VoteDelete(int64_t game_id, int64_t player_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"INSERT INTO gamesxplayers_votedelete (game_id, player_id) VALUES (?, ?)"
		);
		query->bind(1, game_id);
		query->bind(2, player_id);
		
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw PlayerAlreadyVoteDeleteException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw PlayerAlreadyVoteDeleteException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

int32_t GameService::GetVoteDeletePlayerCount(int64_t game_id)  {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT count(*) as votes FROM gamesxplayers_votedelete WHERE game_id = ? "
		);
		query->bind(1, game_id);
		if (!query->executeStep()) throw NoResultsException();

		return query->getColumn(0);
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void GameService::ClearVoteDelete(int64_t game_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM gamesxplayers_votedelete WHERE game_id = ?");
		query->bind(1, game_id);

		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		//Controlled exception
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Game> GameService::GetAllGames() {
	try {
		std::vector<Game> games;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT rooms.id, rooms.name, rooms.owner_id, rooms.active, "
				"games.creator_id, games.message_id, games.status, "
				"games.type, games.n_players, games.n_cards_to_win, games.dictionary_id, games.president_id "
			"FROM rooms INNER JOIN games ON games.room_id = rooms.id"
		);
		while (query->executeStep()) {
			Game game = query->getColumns<Game, 12>();
			games.push_back(game);
		}
	
		return games;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}