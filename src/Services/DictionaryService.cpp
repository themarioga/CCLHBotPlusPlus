#include "DictionaryService.h"
#include "../Db.h"

DictionaryService::DictionaryService() {

}

DictionaryService::~DictionaryService() {

}

std::vector<Dictionary> DictionaryService::GetAllDictionaries() {
	try {
		std::vector<Dictionary> dictionaries;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, creator_id, message_id, published FROM dictionaries");
		while (query->executeStep()) {
			Dictionary dictionary = query->getColumns<Dictionary, 5>();
			dictionaries.push_back(dictionary);
		}
		
		return dictionaries;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Dictionary> DictionaryService::GetAllActiveDictionaries() {
	try {
		std::vector<Dictionary> dictionaries;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, creator_id, message_id, published FROM dictionaries WHERE published = 1");
		while (query->executeStep()) {
			Dictionary dictionary = query->getColumns<Dictionary, 5>();
			dictionaries.push_back(dictionary);
		}
		
		return dictionaries;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}