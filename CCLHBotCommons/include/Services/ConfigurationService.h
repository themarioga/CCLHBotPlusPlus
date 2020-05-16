#pragma once

#include <map>

class ConfigurationService {

	public:

		static ConfigurationService *GetInstance() {
			if (instance == NULL)
				instance = new ConfigurationService();

			return instance;
		}

		ConfigurationService();
		~ConfigurationService();

		/**
		 * @brief Get the Configuration object
		 * 
		 * @param std::string configuration key
		 * @return std::string configuration value
		 */
		std::string GetConfiguration(std::string);

		/**
		 * @brief Get All Configurations
		 * 
		 * @return std::vector<Configuration> vector of configurations
		 */
		std::map<std::string, std::string> GetAllConfigurations();

	private:
	
  		static ConfigurationService *instance;
		std::map<std::string, std::string> configurations;

};