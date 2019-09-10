#pragma once

#include "../Models/Dictionary.h"

class DictionaryService {

	public:

		DictionaryService();
		~DictionaryService();

		/**
		 * @brief Get All Dictionaries
		 * 
		 * @return std::vector<Dictionary> vector of dictionaries
		 */
		std::vector<Dictionary> GetAllDictionaries();

		/**
		 * @brief Get All Active Dictionaries
		 * 
		 * @return std::vector<Dictionary> vector of dictionaries
		 */
		std::vector<Dictionary> GetAllActiveDictionaries();

	private:

};