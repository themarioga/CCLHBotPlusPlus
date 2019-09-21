#include "Room.h"
#include "../Db.h"

Room::Room() {
	id = 0;
	name = "";
	owner_id = 0;
	active = false;
}

Room::Room(int64_t id) {
	this->id = id;
	this->name = "";
	this->owner_id = 0;
	this->active = false;
}

Room::Room(int64_t id, std::string name, int64_t owner_id) {
	this->id = id;
	this->name = name;
	this->owner_id = owner_id;
	this->active = true;
}

Room::Room(int64_t id, std::string name, int64_t owner_id, uint8_t active) {
	this->id = id;
	this->name = name;
	this->owner_id = owner_id;
	this->active = active;
}

Room::~Room() {

}

void Room::Load() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, owner_id, active FROM rooms WHERE id=?");
		query->bind(1, this->id);
		if (!query->executeStep()) throw NoResultsException();
		Room room = query->getColumns<Room, 4>();

		//Check if it's active
		if (!room.active) throw RoomNotActiveException();

		//Check name and owner and refresh it if necesary
		if (this->name != "" && this->name != room.name) SetName(this->name);
		if (this->owner_id != 0 && this->owner_id != room.owner_id) SetOwnerID(this->owner_id);

		//Set the Room info
		this->name = room.name;
		this->owner_id = room.owner_id;
		this->active = room.active;
	} catch (NoResultsException& e) {
		throw RoomNotExistsException();
	} catch (RoomNotActiveException &e) {
		throw RoomNotActiveException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Room::Create() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("INSERT INTO rooms (id, name, owner_id, register_date) VALUES (?, ?, ?, ?)");
		query->bind(1, id);
		query->bind(2, name);
		query->bind(3, owner_id);
		query->bind(4, Util::GetCurrentDatetime());
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw RoomAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw RoomAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Room::Delete() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM rooms WHERE id=?");
		query->bind(1, id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->id = 0;
		this->name = "";
		this->owner_id = 0;
		this->active = false;
	} catch (NoResultsException& e) {
		throw RoomNotExistsException();
	} catch (RoomNotActiveException &e) {
		throw RoomNotActiveException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Room::SetName(std::string name) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE rooms SET name=? WHERE id=?");
		query->bind(1, name);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->name = name;
	} catch (NoResultsException& e) {
		throw RoomNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Room::SetOwnerID(int64_t owner_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE rooms SET owner_id=? WHERE id=?");
		query->bind(1, owner_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the Room info
		this->owner_id = owner_id;
	} catch (NoResultsException& e) {
		throw RoomNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Room::SetActive(bool active) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE rooms SET active=? WHERE id=?");
		query->bind(1, active);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the user info
		this->active = active;
	} catch (NoResultsException& e) {
		throw RoomNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}