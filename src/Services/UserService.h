#pragma once

#include "../Models/User.h"

class UserService {

	public:

		UserService();
		~UserService();

		/**
		 * @brief Get All Users
		 * 
		 * @return std::vector<User> vector of users
		 */
		std::vector<User> GetAllUsers();

	private:

};