#include "Player.h"
#include "../Db.h"
#include "../Exceptions/Game/GameNotExistsException.h"

Player::Player() : User() {
	game_id = 0;
	message_id = 0;
	join_order = 0;
	points = 0;
}

Player::Player(int64_t id) : User(id) {
	game_id = 0;
	message_id = 0;
	join_order = 0;
	points = 0;
}

Player::Player(int64_t id, std::string name, int64_t game_id, int64_t message_id, int32_t join_order) : User(id, name) {
	this->game_id = game_id;
	this->message_id = message_id;
	this->join_order = join_order;
	this->points = 0;
}

Player::Player(int64_t id, std::string name, uint8_t active, int64_t game_id, int64_t message_id, int32_t join_order, int32_t points) : User(id, name, active) {
	this->game_id = game_id;
	this->message_id = message_id;
	this->join_order = join_order;
	this->points = points;
}

Player::~Player() {

}

void Player::Load() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT users.id, users.name, users.active, "
				"players.game_id, players.message_id, players.join_order, players.points "
			"FROM users INNER JOIN players ON players.user_id = users.id "
			"WHERE users.id = ? "
		);
		query->bind(1, id);
		if (!query->executeStep()) throw NoResultsException();
		Player player = query->getColumns<Player, 7>();

		this->name = player.name;
		this->active = player.active;
		this->game_id = player.game_id;
		this->message_id = player.message_id;
		this->points = player.points;
		this->join_order = player.join_order;
	} catch (NoResultsException& e) {
		throw PlayerNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Player::Create() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"INSERT INTO players (user_id, game_id, message_id, join_order, join_date) VALUES (?, ?, ?, ?, ?)"
		);
		query->bind(1, id);
		query->bind(2, game_id);
		query->bind(3, message_id);
		query->bind(4, join_order);
		query->bind(5, Util::GetCurrentDatetime());
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw PlayerAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw PlayerAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Player::Delete() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM players WHERE user_id = ?");
		query->bind(1, id);
		if (!query->exec()) throw NoResultsException();

		this->id = 0;
		this->name = "";
		this->active = 0;
		this->game_id = 0;
		this->message_id = 0;
		this->points = 0;
		this->join_order = 0;
	} catch (NoResultsException& e) {
		throw PlayerNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Player::SetMessageID(int64_t message_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE players SET message_id=? WHERE user_id=?");
		query->bind(1, message_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->message_id = message_id;
	} catch (NoResultsException& e) {
		throw PlayerNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Player::SetPoints(int32_t points) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE players SET points=? WHERE user_id=?");
		query->bind(1, points);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->points = points;
	} catch (NoResultsException& e) {
		throw PlayerNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Player::SetJoinOrder(int32_t join_order) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE players SET join_order=? WHERE user_id=?");
		query->bind(1, join_order);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->join_order = join_order;
	} catch (NoResultsException& e) {
		throw PlayerNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}