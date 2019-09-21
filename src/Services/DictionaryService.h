#pragma once

#include "../Models/Dictionary.h"

class DictionaryService {

	public:

		DictionaryService();
		~DictionaryService();

		/**
		 * @brief Get the Active Dictionaries Count object
		 * 
		 * @return int64_t 
		 */
		int64_t GetActiveDictionariesCount();

		/**
		 * @brief Get All Dictionaries
		 * 
		 * @param
		 * @param
		 * @param
		 * @return std::vector<Dictionary> vector of dictionaries
		 */
		std::vector<Dictionary> GetAllDictionaries(int8_t = -1, int8_t = 0, int8_t = 0);

	private:

};