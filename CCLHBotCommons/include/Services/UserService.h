#pragma once

#include "../Models/User.h"

class UserService {

	public:

		/**
		 * @brief Get the User By Name
		 * 
		 * @param std::string username
		 * @return User 
		 */
		static User GetUserByName(std::string);

		/**
		 * @brief Get All Users
		 * 
		 * @return std::vector<User> vector of users
		 */
		static std::vector<User> GetAllUsers();

	private:

};