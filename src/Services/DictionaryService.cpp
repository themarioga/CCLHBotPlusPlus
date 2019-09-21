#include "DictionaryService.h"
#include "../Db.h"

DictionaryService::DictionaryService() {

}

DictionaryService::~DictionaryService() {

}

int64_t DictionaryService::GetActiveDictionariesCount() {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT COUNT(*) FROM dictionaries WHERE published = 1");
		query->executeStep();

		return query->getColumn(0);
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Dictionary> DictionaryService::GetAllDictionaries(int8_t published, int8_t limit, int8_t offset) {
	try {
		std::vector<Dictionary> dictionaries;

		//Query
		std::string sql = 
			"SELECT id, name, creator_id, message_id, published "
			"FROM dictionaries";
		if (published != -1) sql += " WHERE published = "+std::to_string(published);
		if (limit != 0) sql += " LIMIT "+std::to_string(limit)+" OFFSET "+std::to_string(offset);

		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(sql);
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