#include "ConfigurationService.h"
#include "../Db.h"

ConfigurationService::ConfigurationService() {
	//Query
	std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT * FROM configurations");
	while (query->executeStep()) 
	{
		configurations.insert(std::pair<std::string, std::string>(query->getColumn(0), query->getColumn(1)));
	}
}

ConfigurationService::~ConfigurationService() {
	configurations.clear();
}

std::string ConfigurationService::GetConfiguration(std::string key) {
	try {
		return configurations.at(key);
	} catch (const std::out_of_range& e) {
		std::cerr << "No se ha encontrado la configuracion '"+key+"'" << std::endl;
		std::exit(0);
	}
}

std::map<std::string, std::string> ConfigurationService::GetAllConfigurations() {
	return configurations;
}