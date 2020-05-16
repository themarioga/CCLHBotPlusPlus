#include "Services/DictionaryService.h"
#include "Base/Db.h"


void DictionaryService::AddDictionaryCollaborator(int64_t user_id, int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("INSERT INTO usersxdictionaries_collaborators VALUES (?, ?, 0, 0)");
		query->bind(1, user_id);
		query->bind(2, dictionary_id);

		if (!query->exec()) throw NoResultsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void DictionaryService::ModifyCollaboratorSetAccepted(int64_t user_id, int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE usersxdictionaries_collaborators SET accepted = 1 WHERE user_id = ? AND dictionary_id=?");
		query->bind(1, user_id);
		query->bind(2, dictionary_id);
		if (!query->exec()) throw NoResultsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void DictionaryService::ModifyCollaboratorSetEditMode(int64_t user_id, int64_t dictionary_id, bool edit_mode) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("UPDATE usersxdictionaries_collaborators SET can_edit = ? WHERE user_id = ? AND dictionary_id=?");
		query->bind(1, edit_mode);
		query->bind(2, user_id);
		query->bind(3, dictionary_id);
		if (!query->exec()) throw NoResultsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void DictionaryService::DeleteDictionaryCollaborator(int64_t user_id, int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM usersxdictionaries_collaborators WHERE user_id = ? AND dictionary_id=?");
		query->bind(1, user_id);
		query->bind(2, dictionary_id);
		if (!query->exec()) throw NoResultsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

void DictionaryService::DeleteAllDictionaryCollaborators(int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("DELETE FROM usersxdictionaries_collaborators WHERE dictionary_id=?");
		query->bind(1, dictionary_id);
		if (!query->exec()) throw NoResultsException();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

int32_t DictionaryService::CountDictionaryCollaborators(int64_t dictionary_id) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT COUNT(*) FROM usersxdictionaries_collaborators WHERE dictionary_id = ?");
		query->bind(1, dictionary_id);
		query->executeStep();

		return query->getColumn(0);
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<User> DictionaryService::GetDictionaryCollaborators(int64_t dictionary_id, int8_t accepted) {
	try {
		std::vector<User> users;

		std::string sql = 
			"SELECT id, name, active FROM users WHERE id"
			" IN (SELECT user_id FROM usersxdictionaries_collaborators WHERE dictionary_id = ?";
		if (accepted != -1) sql += " AND accepted = "+std::to_string(accepted);
		sql += ")";

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(sql);
		query->bind(1, dictionary_id);
		while (query->executeStep()) {
			User user = query->getColumns<User, 3>();
			users.push_back(user);
		}
		
		return users;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

int64_t DictionaryService::CountDictionariesByName(std::string name) {
	try {
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT COUNT(*) FROM dictionaries WHERE name = ?");
		query->bind(1, name);
		query->executeStep();

		return query->getColumn(0);
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

int64_t DictionaryService::GetActiveDictionariesCount(int8_t published, int8_t shared, int64_t user_id, int8_t can_edit) {
	try {
		//Query
		std::string sql = 
			"SELECT COUNT(*) "
			"FROM dictionaries ";

		if (user_id != 0) 
			sql += " INNER JOIN usersxdictionaries_collaborators ON usersxdictionaries_collaborators.dictionary_id = dictionaries.id ";

		sql += " WHERE name IS NOT NULL ";
			
		if (published != -1) sql += " AND published = "+std::to_string(published);

		if (shared != -1) sql += " AND (shared = "+std::to_string(shared);
		
		if (user_id != 0) {
			if (shared != -1) sql += " OR (";
			else sql += " AND (";
			sql += " usersxdictionaries_collaborators.user_id = "+std::to_string(user_id);
			sql += " AND usersxdictionaries_collaborators.accepted = 1 ";
			if (can_edit != -1) sql += " AND usersxdictionaries_collaborators.can_edit = "+std::to_string(can_edit);
			sql += ")";
		}

		if (shared != -1) sql += ")";

		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery(sql);
		query->executeStep();

		return query->getColumn(0);
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<Dictionary> DictionaryService::GetAllDictionaries(int8_t published, int8_t shared, int64_t user_id, int8_t can_edit, int8_t limit, int8_t offset) {
	try {
		std::vector<Dictionary> dictionaries;

		//Query
		std::string sql = 
			"SELECT id, name, creator_id, shared, published "
			"FROM dictionaries ";
		
		if (user_id != 0) 
			sql += " INNER JOIN usersxdictionaries_collaborators ON usersxdictionaries_collaborators.dictionary_id = dictionaries.id ";

		sql += " WHERE name IS NOT NULL ";
			
		if (published != -1) sql += " AND published = "+std::to_string(published);

		if (shared != -1) sql += " AND (shared = "+std::to_string(shared);
		
		if (user_id != 0) {
			if (shared != -1) sql += " OR (";
			else sql += " AND (";
			sql += " usersxdictionaries_collaborators.user_id = "+std::to_string(user_id);
			sql += " AND usersxdictionaries_collaborators.accepted = 1 ";
			if (can_edit != -1) sql += " AND usersxdictionaries_collaborators.can_edit = "+std::to_string(can_edit);
			sql += ")";
		}

		if (shared != -1) sql += ")";
		
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