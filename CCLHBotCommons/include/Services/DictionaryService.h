#pragma once

#include "../Models/Dictionary.h"
#include "../Models/User.h"
#include "../Exceptions/Dictionary/DictionaryMaxCollaboratorsReachedException.h"
#include "../Exceptions/Dictionary/DictionaryNotCompletedException.h"

class DictionaryService {

	public:

		/**
		 * @brief Add a collab
		 * 
		 * @param user_id
		 * @param dictionary_id
		 */
		static void AddDictionaryCollaborator(int64_t, int64_t);

		/**
		 * @brief Set accepted the dictionary collaborator
		 * 
		 * @param user_id
		 * @param dictionary_id
		 */
		static void ModifyCollaboratorSetAccepted(int64_t, int64_t);

		/**
		 * @brief Set accepted the dictionary collaborator
		 * 
		 * @param user_id
		 * @param dictionary_id
		 * @param edit_mode
		 */
		static void ModifyCollaboratorSetEditMode(int64_t, int64_t, bool);

		/**
		 * @brief Remove the dictionary collaborators
		 * 
		 * @param user_id
		 * @param dictionary_id
		 */
		static void DeleteDictionaryCollaborator(int64_t, int64_t);

		/**
		 * @brief Remove the dictionary collaborators
		 * 
		 * @param dictionary_id
		 */
		static void DeleteAllDictionaryCollaborators(int64_t);

		/**
		 * @brief Count the Dictionary Collaborators
		 * 
		 * @param dictionary_id
		 * @return int32_t
		 */
		static int32_t CountDictionaryCollaborators(int64_t);

		/**
		 * @brief Get the Dictionary Collaborators
		 * 
		 * @param dictionary_id
		 * @param accepted
		 * @return std::vector<User> 
		 */
		static std::vector<User> GetDictionaryCollaborators(int64_t, int8_t = -1);

		/**
		 * @brief Get the Dictionaries Count by name
		 * 
		 * @param dictionary_name
		 * @return int64_t 
		 */
		static int64_t CountDictionariesByName(std::string);


		/**
		 * @brief Get the Active Dictionaries Count object
		 * 
		 * @param published
		 * @param shared
		 * @param user_id
		 * @param can_edit
		 * @return int64_t 
		 */
		static int64_t GetActiveDictionariesCount(int8_t = -1, int8_t = -1, int64_t = 0, int8_t = -1);

		/**
		 * @brief Get All Dictionaries
		 * 
		 * @param published
		 * @param shared
		 * @param user_id
		 * @param can_edit
		 * @param limit
		 * @param offset
		 * @return std::vector<Dictionary> vector of dictionaries
		 */
		static std::vector<Dictionary> GetAllDictionaries(int8_t = -1, int8_t = -1, int64_t = 0, int8_t = -1, int8_t = 0, int8_t = 0);

	private:

};