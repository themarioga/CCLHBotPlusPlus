#include "PlayerService.h"
#include "../Db.h"

PlayerService::PlayerService() {

}

PlayerService::~PlayerService() {

}

std::vector<Player> PlayerService::GetPlayersByIDs(std::vector<int64_t> playerIDs) {
	try {
		std::vector<Player> players;

		//Id list to string
		std::string idList = "";
		for (int64_t playerID : playerIDs) {
			idList += std::to_string(playerID) + ",";
		}

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM users INNER JOIN players ON players.user_id = users.id "
			"WHERE players.user_id IN ("+idList+")"
		);

		while (query->executeStep()) {
			Player player = query->getColumns<Player, 7>();
			players.push_back(player);
		}
		
		return players;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void PlayerService::DeletePlayersInGame(int64_t game_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM players WHERE game_id = ?");
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

std::vector<Player> PlayerService::GetPlayersInGame(int64_t game_id) {
	try {
		std::vector<Player> players;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM users INNER JOIN players ON players.user_id = users.id "
			"WHERE players.game_id = ? "
			"ORDER BY join_order ASC");
		query->bind(1, game_id);
		
		while (query->executeStep()) {
			Player player = query->getColumns<Player, 7>();
			players.push_back(player);
		}
		
		return players;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

int32_t PlayerService::GetPlayersInGameCount(int64_t game_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT count(*) as playerCount "
			"FROM players "
			"WHERE players.game_id = ? ");
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

int64_t PlayerService::GetPlayerIndexFromPlayerArray(std::vector<Player>& players, int64_t player_id) {
	for (int i = 0; i < players.size(); i++) {
		if (players[i].GetID() == player_id){
			return i;
		}
	}
	return -1;
}

std::vector<Player> PlayerService::GetAllPlayers() {
	try {
		std::vector<Player> players;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM users INNER JOIN players ON players.user_id = users.id"
			"WHERE active = 1"
		);
		while (query->executeStep()) {
			Player player = query->getColumns<Player, 7>();
			players.push_back(player);
		}
		
		return players;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}