#include "Card.h"
#include "../Db.h"

Card::Card() {
	id = 0;
	text = "";
	type = (CardTypeEnum)0;
	dictionary_id = 0;
}

Card::Card(int64_t id) {
	this->id = id;
	this->text = "";
	this->type = (CardTypeEnum)0;
	this->dictionary_id = 0;
}

Card::Card(uint8_t type, std::string text, int64_t dictionary_id) {
	this->id = 0;
	this->type = (CardTypeEnum)type;
	this->text = text;
	this->dictionary_id = dictionary_id;
}

Card::Card(int64_t id, uint8_t type, std::string text, int64_t dictionary_id) {
	this->id = id;
	this->type = (CardTypeEnum)type;
	this->text = text;
	this->dictionary_id = dictionary_id;
}

Card::~Card() {

}

void Card::Load() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, type, text, dictionary_id FROM cards WHERE id=?");
		query->bind(1, id);
		if (!query->executeStep()) throw NoResultsException();
		Card card = query->getColumns<Card, 4>();

		//Set the card info
		this->type = card.type;
		this->text = card.text;
		this->dictionary_id = card.dictionary_id;
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Card::Create() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("INSERT INTO cards (type, text, dictionary_id) VALUES (?, ?, ?)");
		query->bind(1, type);
		query->bind(2, text);
		query->bind(3, dictionary_id);
		if (!query->exec()) throw NoResultsException();

		this->id = Db::Instance()->GetLastInsertedRowId();
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

void Card::Delete() {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM cards WHERE id=?");
		query->bind(1, id);
		if (!query->exec()) throw NoResultsException();

		//Set the card info
		this->id = 0;
		this->type = (CardTypeEnum)0;
		this->text = "";
		this->dictionary_id = 0;
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Card::SetType(CardTypeEnum type) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE cards SET type=? WHERE id=?");
		query->bind(1, type);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the card info
		this->type = type;
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Card::SetText(std::string text) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE cards SET text=? WHERE id=?");
		query->bind(1, text);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the card info
		this->text = text;
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void Card::SetDictionaryID(int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE cards SET dictionary_id=? WHERE id=?");
		query->bind(1, dictionary_id);
		query->bind(2, this->id);
		if (!query->exec()) throw NoResultsException();

		//Set the card info
		this->dictionary_id = dictionary_id;
	} catch (NoResultsException& e) {
		throw CardNotExistsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}