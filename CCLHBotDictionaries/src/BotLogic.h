#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#include "Models/User.h"
#include "Models/Card.h"
#include "Models/Dictionary.h"

#include "Services/UserService.h"
#include "Services/CardService.h"
#include "Services/DictionaryService.h"
#include "Services/ConfigurationService.h"

class BotLogic {

	public:

		/**
		 * @brief Register a new user on the system
		 * 
		 * @param User user object
		 * @param std::function<void()> success callback
		 * @param std::function<void(std::string)> failure callback
		 */
		static void RegisterUser(User&, std::function<void()>, std::function<void(std::string)>);

		/**
		 * @brief Register a new dictionary on the system
		 * 
		 * @param Dictionary user object
		 * @param std::function<void()> success callback
		 * @param std::function<void(std::string)> failure callback
		 */
		static void RegisterDictionary(Dictionary&, std::function<void()>, std::function<void(std::string)>);

		/**
		 * @brief Get the Dictionaries object
		 * 
		 * @param uint64_t offset
		 * @param uint8_t offset
		 * @param std::function<void()> success callback
		 * @param std::function<void(std::string)> failure callback
		 */
		static void GetDictionaries(uint64_t, uint8_t, std::function<void(int64_t, std::vector<Dictionary>&)>, std::function<void(std::string)>);

	private:

};