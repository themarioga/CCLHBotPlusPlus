#include "UserService.h"
#include "../Db.h"

UserService::UserService() {

}

UserService::~UserService() {

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