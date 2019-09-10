#include "User.h"
#include "../Db.h"

User::User() {
	id = 0;
	name = "";
	active = false;
}

User::User(int64_t id) {
	this->id = id;
	this->name = "";
	this->active = false;
}

User::User(int64_t id, std::string name) {
	this->id = id;
	this->name = name;
	this->active = true;
}

User::User(int64_t id, std::string name, uint8_t active) {
	this->id = id;
	this->name = name;
	this->active = active;
}

User::~User() {

}

void User::Load() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, active FROM users WHERE id=?");
		query->bind(1, id);
		if (!query->executeStep()) throw NoResultsException();
		User user = query->getColumns<User, 3>();

		//Check if it's active
		if (!user.active) throw UserNotActiveException();
		
		//Check name and owner and refresh it if necesary
		if (this->name != "" && this->name != user.name) SetName(this->name);

		//Set the user info
		this->name = user.name;
		this->active = user.active;
	} catch (NoResultsException& e) {
		throw UserNotExistsException();
	} catch (UserNotActiveException &e) {
		throw UserNotActiveException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void User::Create() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("INSERT INTO users (id, name) VALUES (?, ?)");
		query->bind(1, id);
		query->bind(2, name);
		if (!query->exec()) throw NoResultsException();
	} catch (NoResultsException& e) {
		throw UserAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw UserAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void User::Delete() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM users WHERE id=?");
		query->bind(1, id);
		if (!query->exec()) throw NoResultsException();

		//Set the user info
		this->id = 0;
		this->name = "";
		this->active = false;
	} catch (NoResultsException& e) {
		throw UserNotExistsException();
	} catch (UserNotActiveException &e) {
		throw UserNotActiveException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void User::SetName(std::string name) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE users SET name=? WHERE id=?");
		query->bind(1, name);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the user info
		this->name = name;
	} catch (NoResultsException& e) {
		throw UserNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void User::SetActive(bool active) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE users SET active=? WHERE id=?");
		query->bind(1, active);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the user info
		this->active = active;
	} catch (NoResultsException& e) {
		throw UserNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}