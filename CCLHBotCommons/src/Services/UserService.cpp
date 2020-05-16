#include "Services/UserService.h"
#include "Base/Db.h"

User UserService::GetUserByName(std::string name) {
	try {
		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, active FROM users WHERE name LIKE '%' || ? || '%'");
		query->bind(1, name);
		if (!query->executeStep()) {
			std::cerr << query->getExpandedSQL() << std::endl;
			throw NoResultsException();
		}

		return query->getColumns<User, 3>();
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}

std::vector<User> UserService::GetAllUsers() {
	try {
		std::vector<User> users;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, active FROM users WHERE active = 1");
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