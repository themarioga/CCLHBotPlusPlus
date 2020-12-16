#include <iostream>
#include <fstream>
#include "Base/Db.h"
#include "Base/Util.hpp"
#include "CCLHDictionaries.h"

int main(int argc, char* argv[]) {
	Util::AssertFalse(argc < 2, "Debes pasar como parametro el archivo que contenga la base de datos. Ejemplo: './CCLHBotDictionaries cclhdictionaries.db'");

	//Check if Db file exists
	std::ifstream sql_file(argv[1], std::ifstream::binary);
	Util::AssertTrue(sql_file.good(), "No se ha encontrado el archivo especificado.");

	//Check if we want webhooks
	bool webhooks = false;
	if (argc == 3 && strcmp(argv[2], "true") == 0) webhooks = true;

	//Start Db
	Db::Instance()->Connect(argv[1]);

	//Create Bot instance
	CCLHDictionaries cclhdictionaries;
	
	//Loop
	cclhdictionaries.Listen(webhooks);

	return 0;
}