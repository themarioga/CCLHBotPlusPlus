#pragma once

#include <map>

class ConfigurationService {

	public:

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
	
		std::map<std::string, std::string> configurations;

};