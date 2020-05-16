#include "Base/Db.h"

Db *Db::instance = 0;

Db::Db() {
	db = nullptr;
	transaction = nullptr;
}

Db::~Db() {
	delete db;
	db = nullptr;
	
	if (transaction != nullptr) {
		delete transaction;
		transaction = nullptr;
	}
}

//Db methods
void Db::Connect(std::string db_path) {
	try {
		std::clog << "Iniciando base de datos: " << db_path << std::endl;

		db = new SQLite::Database(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

		std::clog << "Se ha iniciado la base de datos: " << db_path << std::endl;
		
		db->exec("PRAGMA journal_mode=WAL");
		db->exec("PRAGMA synchronous=NORMAL");

		std::clog << "Se ha configurado la base de datos." << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Connect error: " << e.what() << std::endl;
		std::abort();
	}
}

//Transaction Methods
void Db::StartTransaction() {
	if (transaction == nullptr)
		transaction = new SQLite::Transaction(*db);
}

void Db::CommitTransaction() {
	if (transaction != nullptr) {
		transaction->commit();
		delete transaction;
		transaction = nullptr;
	}
}

void Db::RollbackTransaction() {
	if (transaction != nullptr) {
		delete transaction;
		transaction = nullptr;
	}
}

//Execute Query Methods
int Db::ExecuteQuery(std::string query) {
	return db->exec(query);
}

std::shared_ptr<SQLite::Statement> Db::CreateQuery(std::string query) {
	return std::shared_ptr<SQLite::Statement>(new SQLite::Statement(*db, query));
}

//Process Query Methods
template <typename T, int N>
T Db::GetSingleRow(std::string query) {
	SQLite::Statement statement(*db, query);
	if (!statement.executeStep()) throw std::runtime_error("La consulta no ha producido resultados");
	
	return statement.getColumns<T, N>();
}

template <typename T, int N>
std::vector<T> Db::GetNRows(std::string query, int numRows) {
	SQLite::Statement statement(*db, query);

	std::vector<T> ret(numRows);

	if (!statement.executeStep()) throw std::runtime_error("La consulta no ha producido resultados");
	for (int i = 0; i < numRows && statement.hasRow(); i++) {
		ret.push_back(statement.getColumns<T, N>());
		
  	statement.executeStep();
	}

	return ret;
}

template <typename T, int N>
std::vector<T> Db::GetAllRows(std::string query) {
	SQLite::Statement statement(*db, query);

	std::vector<T> ret;

	if (!statement.executeStep()) throw std::runtime_error("La consulta no ha producido resultados");
	while (statement.hasRow()) {
		ret.push_back(statement.getColumns<T, N>());
		
  	statement.executeStep();
	}

	return ret;
}

SQLite::Column Db::GetSingleColumn(std::string query) {
	SQLite::Statement statement(*db, query);
	if (!statement.executeStep()) throw std::runtime_error("La consulta no ha producido resultados");
	return statement.getColumn(0);
}

SQLite::Column Db::GetSingleColumn(std::string query, int n) {
	SQLite::Statement statement(*db, query);
	if (!statement.executeStep()) throw std::runtime_error("La consulta no ha producido resultados");
	return statement.getColumn(n);
}

int Db::GetLastInsertedRowId() {
	return db->getLastInsertRowid();
}
