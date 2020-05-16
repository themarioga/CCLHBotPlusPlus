#include <iostream>
#include <fstream>
#include "Base/Db.h"
#include "Base/Util.hpp"
#include "CCLH.h"

int main(int argc, char* argv[]) {
	Util::AssertFalse(argc < 2, "Debes pasar como parametro el archivo que contenga la base de datos. Ejemplo: './CCLHBot cclh.db'");

	//Check if Db file exists
	std::ifstream sql_file(argv[1], std::ifstream::binary);
	Util::AssertTrue(sql_file.good(), "No se ha encontrado el archivo especificado.");

	//Start Db
	Db::Instance()->Connect(argv[1]);

	//Create CCLHBot instance
	CCLH cclh;

	//Loop
	cclh.Listen();

	return 0;
}