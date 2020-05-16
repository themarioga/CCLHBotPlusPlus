#include "BotLogic.h"

void BotLogic::RegisterUser(User &user, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Create a new user
		user.Create();
		
		//If success
		successCallback();
	} catch (UserAlreadyExistsException& e) {
		try {
			//Load the user
			user.Load();
		} catch (UserNotActiveException& e) {
			//If it's not active
			user.SetActive(true);

			//If success
			successCallback();
		} catch (ApplicationException& e) {
			failureCallback(e.what());
		} catch (UnexpectedException& e) {
			failureCallback(e.what());
			std::cerr << "Error: " << e.what() << std::endl;
		} catch (std::runtime_error& e) {
			failureCallback(e.what());
			std::cerr << "Error: " << e.what() << std::endl;
		}
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void BotLogic::RegisterDictionary(Dictionary &dictionary, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Check if the name is not duplicated
		if (DictionaryService::CountDictionariesByName(dictionary.GetName()) > 0) 
			throw DictionaryAlreadyExistsException();

		//Create a new dictionary
		dictionary.Create();

		//Add the collab
		DictionaryService::AddDictionaryCollaborator(dictionary.GetCreatorID(), dictionary.GetID());

		//If Success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void BotLogic::GetDictionaries(uint64_t user_id, uint8_t offset, std::function<void(int64_t, std::vector<Dictionary>&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Get dictionary count
		int64_t count = DictionaryService::GetActiveDictionariesCount(-1, -1, user_id, true);
		
		//Get dictionaries
		std::vector<Dictionary> dictionaries = DictionaryService::GetAllDictionaries(-1, -1, user_id, true, std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_per_page")), offset);
			
		//If success
		successCallback(count, dictionaries);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}