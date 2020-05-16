#include "Models/Game.h"
#include "Base/Db.h"

Game::Game() : Room() {
	this->creator_id = 0;
	this->message_id = 0;
	this->status = 0;
	this->type = 0;
	this->n_players = 0;
	this->n_cards_to_win = 0;
	this->dictionary_id = 0;
	this->president_id = 0;
	this->round_number = 0;
}

Game::Game(int64_t id) : Room(id) {
	this->creator_id = 0;
	this->message_id = 0;
	this->status = 0;
	this->type = 0;
	this->n_players = 0;
	this->n_cards_to_win = 0;
	this->dictionary_id = 0;
	this->president_id = 0;
	this->round_number = 0;
}

Game::Game( int64_t id, 
			std::string name, 
			int64_t owner_id, 
			int64_t creator_id, 
			int64_t message_id) 
			: Room(id, name, owner_id) {
	this->creator_id = creator_id;
	this->message_id = message_id;
	this->status = GameStatusEnum::GAME_CREATED;
	this->type = 0;
	this->n_players = 0;
	this->n_cards_to_win = 0;
	this->dictionary_id = 0;
	this->president_id = 0;
	this->round_number = 0;
}

Game::Game( int64_t id, 
			std::string name, 
			int64_t owner_id, 
			uint8_t active,
			int64_t creator_id, 
			int64_t message_id, 
			uint8_t status,
			uint8_t type, 
			uint8_t n_players, 
			uint8_t n_cards_to_win, 
			int64_t dictionary_id, 
			int64_t president_id,
			uint8_t round_number) 
			: Room(id, name, owner_id, active) {
	this->creator_id = creator_id;
	this->message_id = message_id;
	this->status = status;
	this->type = type;
	this->n_players = n_players;
	this->n_cards_to_win = n_cards_to_win;
	this->dictionary_id = dictionary_id;
	this->president_id = president_id;
	this->round_number = round_number;
}

Game::~Game() {

}

void Game::Load()  {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"SELECT rooms.id, rooms.name, rooms.owner_id, rooms.active, "
				"games.creator_id, games.message_id, games.status, "
				"games.type, games.n_players, games.n_cards_to_win, "
				"games.dictionary_id, games.president_id, games.round_number "
			"FROM rooms INNER JOIN games ON games.room_id = rooms.id "
			"WHERE rooms.id = ? "
		);
		query->bind(1, id);
		if (!query->executeStep()) throw NoResultsException();
		Game game = query->getColumns<Game, 13>();

		this->name = game.name;
		this->owner_id = game.owner_id;
		this->active = game.active;
		this->creator_id = game.creator_id;
		this->message_id = game.message_id;
		this->status = game.status;
		this->type = game.type;
		this->n_players = game.n_players;
		this->n_cards_to_win = game.n_cards_to_win;
		this->dictionary_id = game.dictionary_id;
		this->president_id = game.president_id;
		this->round_number = game.round_number;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::Create() {
	try {
		try {
			Room::Create();
		} catch (RoomAlreadyExistsException& e) {
			//Controlled exception
		}

		//Update room info if necesary
		if (this->name != name) SetName(name);
		if (this->owner_id != owner_id) SetOwnerID(owner_id);
		
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(
			"INSERT INTO games (room_id, creator_id, message_id, status, creation_date) VALUES (?, ?, ?, ?, ?)"
		);
		query->bind(1, id);
		query->bind(2, creator_id);
		query->bind(3, message_id);
		query->bind(4, status);
		query->bind(5, Util::GetCurrentDatetime());
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw GameAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw GameAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::Delete()  {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM games WHERE room_id = ?");
		query->bind(1, id);
		if (!query->exec()) throw NoResultsException();

		this->id = 0;
		this->name = "";
		this->owner_id = 0;
		this->active = 0;
		this->creator_id = 0;
		this->message_id = 0;
		this->status = 0;
		this->type = 0;
		this->n_players = 0;
		this->n_cards_to_win = 0;
		this->dictionary_id = 0;
		this->president_id = 0;
		this->round_number = 0;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetCreatorID(int64_t creator_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET creator_id=? WHERE room_id=?");
		query->bind(1, creator_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->creator_id = creator_id;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetMessageID(int64_t message_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET message_id=? WHERE room_id=?");
		query->bind(1, message_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->message_id = message_id;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetStatus(GameStatusEnum status) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET status=? WHERE room_id=?");
		query->bind(1, status);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->status = status;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetType(GameTypeEnum type) {
	try {
		//Query 
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET type=? WHERE room_id=?");
		query->bind(1, type);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->type = type;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetNumberOfPlayers(uint8_t n_players) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET n_players=? WHERE room_id=?");
		query->bind(1, n_players);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->n_players = n_players;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetNumberOfCardsToWin(uint8_t n_cards_to_win) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET n_cards_to_win=? WHERE room_id=?");
		query->bind(1, n_cards_to_win);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->n_cards_to_win = n_cards_to_win;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetDictionaryID(int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET dictionary_id=?, status=? WHERE room_id=?");
		query->bind(1, dictionary_id);
		//ToDo: I set the status here so I only need one query, but I think I'll have to find a better way
		query->bind(2, GameStatusEnum::GAME_CONFIGURED);
		query->bind(3, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->dictionary_id = dictionary_id;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetPresidentID(int64_t president_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET president_id=? WHERE room_id=?");
		query->bind(1, president_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->president_id = president_id;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Game::SetRoundNumber(uint8_t round_number) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE games SET round_number=? WHERE room_id=?");
		query->bind(1, round_number);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->round_number = round_number;
	} catch (NoResultsException& e) {
		throw GameNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}
