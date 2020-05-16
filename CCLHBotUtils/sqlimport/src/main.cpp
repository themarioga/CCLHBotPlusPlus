#include <iostream>
#include <fstream>
#include <string>
#include "Db.h"

int main(int argc, char* argv[]) {
	Util::AssertFalse(argc != 4, "Utiliza los parametros 'import db_file sql_file' 'bulkimport db_file sql_file' o 'export db_file sql_file'");

	if (strcmp(argv[1], "import") == 0 && argc == 4) {
		Db::Instance()->Connect(argv[2]);
		Db::Instance()->Import(argv[3]);
	} else if (strcmp(argv[1], "bulkimport") == 0 && argc == 4) {
		Db::Instance()->Connect(argv[2]);
		Db::Instance()->BulkImport(argv[3]);
	} else if (strcmp(argv[1], "export") == 0 && argc == 4) {
		Db::Instance()->Connect(argv[2]);
		//ToDo: export
	} else {
		std::cerr << "Utiliza los parametros 'import db_file sql_file' 'bulkimport db_file sql_file' o 'export db_file sql_file'" << std::endl;
	}
	
	return 0;
}