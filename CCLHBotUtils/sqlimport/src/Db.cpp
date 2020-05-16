#include "Db.h"

Db *Db::instance = 0;

Db::Db() {
	db = nullptr;
}

Db::~Db() {
	delete db;
	db = nullptr;
}

//Db methods
void Db::Connect(std::string db_path) {
	try
	{
		std::clog << "Iniciando base de datos: " << db_path << std::endl;

		db = new SQLite::Database(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

		std::clog << "Se ha iniciado la base de datos: " << db_path << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Connect error: " << e.what() << std::endl;
		std::abort();
	}
}

void Db::Import(std::string sql_file) {
	std::ifstream file_stream(sql_file, std::ifstream::binary);
	Util::AssertTrue(file_stream.good(), "No se ha encontrado el archivo SQL especificado.");
	
	int fileLine = 0;

	try {
		std::clog << "Importando el archivo: " << sql_file << std::endl;

		for (std::string line; std::getline(file_stream, line); ++fileLine) {
			db->exec(line);
		}
		
		std::clog << "Se ha importado el archivo " << sql_file << " a la base de datos." << std::endl;
	} catch (SQLite::Exception& e) {
		std::cerr << "Import SQL error in line " << (fileLine + 1) << ": " << e.what() << std::endl;
		std::abort();
	} catch (std::exception& e) {
		std::cerr << "Import IO error: " << e.what() << std::endl;
		std::abort();
	}
}

void Db::BulkImport(std::string sql_file) {
	std::ifstream file_stream(sql_file, std::ifstream::binary);
	Util::AssertTrue(file_stream.good(), "No se ha encontrado el archivo SQL especificado.");

	try {
		std::clog << "Importando el archivo: " << sql_file << std::endl;

		std::ostringstream sql_stream;
		sql_stream << file_stream.rdbuf();
		file_stream.close();

		db->exec(sql_stream.str());
		
		std::clog << "Se ha importado el archivo " << sql_file << " a la base de datos." << std::endl;
	} catch (SQLite::Exception& e) {
		std::cerr << "Import SQL error: " << e.what() << std::endl;
		std::abort();
	} catch (std::exception& e) {
		std::cerr << "Import IO error: " << e.what() << std::endl;
		std::abort();
	}
}