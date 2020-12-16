#include <iostream>
#include <fstream>
#include "Base/Db.h"
#include "Base/Util.hpp"
#include "echo.h"

int main(int argc, char* argv[]) {
	Util::AssertFalse(argc < 3, "Debes pasar como parametro el archivo que contenga la base de datos y el tipo de bot (cclh o dictionaries). Ejemplo: './echobot cclh.db cclh'");

	//Check if Db file exists
	std::ifstream sql_file(argv[1], std::ifstream::binary);
	Util::AssertTrue(sql_file.good(), "No se ha encontrado el archivo especificado.");
	
	//Set the bot name
	std::string botname = std::string(argv[2]);

	//Check if we want webhooks
	bool webhooks = false;
	if (argc == 4 && strcmp(argv[3], "true") == 0) webhooks = true;

	//Start Db
	Db::Instance()->Connect(argv[1]);

	//Create CCLHBot instance
	Echo echo(botname);

	//Loop
	echo.Listen(webhooks);

	return 0;
}