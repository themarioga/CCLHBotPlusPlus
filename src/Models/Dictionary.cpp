#include "Dictionary.h"
#include "../Db.h"

Dictionary::Dictionary() {
	id = 0;
	name = "";
	creator_id = 0;
	message_id = 0;
	published = false;
}

Dictionary::Dictionary(int64_t id) {
	this->id = id;
	this->name = "";
	this->creator_id = 0;
	this->message_id = 0;
	this->published = false;
}

Dictionary::Dictionary(std::string name, int64_t creator_id, int64_t message_id) {
	this->id = 0;
	this->name = name;
	this->creator_id = creator_id;
	this->message_id = message_id;
	this->published = false;
}

Dictionary::Dictionary(int64_t id, std::string name, int64_t creator_id, int64_t message_id, uint8_t published) {
	this->id = id;
	this->name = name;
	this->creator_id = creator_id;
	this->message_id = message_id;
	this->published = published;
}

Dictionary::~Dictionary() {

}

void Dictionary::Load() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, creator_id, message_id, published FROM dictionaries WHERE id=?");
		query->bind(1, id);
		if (!query->executeStep()) throw NoResultsException();
		Dictionary dictionary = query->getColumns<Dictionary, 5>();

		//Set the dictionary info
		this->name = dictionary.name;
		this->creator_id = dictionary.creator_id;
		this->message_id = dictionary.message_id;
		this->published = dictionary.published;
	} catch (NoResultsException& e) {
		throw DictionaryNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Dictionary::Create() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("INSERT INTO dictionaries (name, creator_id, message_id, creation_date) VALUES (?, ?, ?, ?)");
		query->bind(1, name);
		query->bind(2, creator_id);
		query->bind(3, message_id);
		query->bind(4, Util::GetCurrentDatetime());
		if (!query->exec()) throw NoResultsException();

		this->id = Db::Instance()->GetLastInsertedRowId();
	} catch (NoResultsException& e) {
		throw DictionaryAlreadyExistsException();
	} catch (SQLite::Exception& e) {
		if (std::string(e.what()).find("UNIQUE constraint failed") != std::string::npos) {
			throw DictionaryAlreadyExistsException();
		} else {
			throw UnexpectedException(e.what());
		}
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Dictionary::Delete() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM dictionaries WHERE id=?");
		query->bind(1, id);
		if (!query->exec()) throw NoResultsException();

		//Set the dictionary info
		this->id = 0;
		this->name = "";
		this->creator_id = 0;
		this->message_id = 0;
		this->published = false;
	} catch (NoResultsException& e) {
		throw DictionaryNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Dictionary::SetName(std::string name) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE dictionaries SET name=? WHERE id=?");
		query->bind(1, name);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the dictionary info
		this->name = name;
	} catch (NoResultsException& e) {
		throw DictionaryNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Dictionary::SetCreatorID(int64_t creator_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE dictionaries SET creator_id=? WHERE id=?");
		query->bind(1, creator_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the dictionary info
		this->creator_id = creator_id;
	} catch (NoResultsException& e) {
		throw DictionaryNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Dictionary::SetMessageID(int64_t message_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE dictionaries SET message_id=? WHERE id=?");
		query->bind(1, message_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the dictionary info
		this->message_id = message_id;
	} catch (NoResultsException& e) {
		throw DictionaryNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Dictionary::SetPublished(bool published) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE dictionaries SET published=? WHERE id=?");
		query->bind(1, published);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the dictionary info
		this->published = published;
	} catch (NoResultsException& e) {
		throw DictionaryNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}