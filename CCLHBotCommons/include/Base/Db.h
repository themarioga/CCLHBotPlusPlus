#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>

#include "Util.hpp"
#include "../Exceptions/NoResultsException.h"

class Db {

	public: 
		static Db *Instance() {
			if (!instance)
			instance = new Db();
			return instance;
		}

		//Db methods
		void Connect(std::string);

		//TransactionMethods
		void StartTransaction();
		void CommitTransaction();
		void RollbackTransaction();
		
		//Query Methods
		int ExecuteQuery(std::string);

		std::shared_ptr<SQLite::Statement> CreateQuery(std::string);

		int GetLastInsertedRowId();

		//Easy Query Methods
		template <typename T, int N>
		T GetSingleRow(std::string);
		template <typename T, int N>
		std::vector<T> GetNRows(std::string, int);
		template <typename T, int N>
		std::vector<T> GetAllRows(std::string);

		SQLite::Column GetSingleColumn(std::string);
		SQLite::Column GetSingleColumn(std::string, int);

	protected:
		Db();
		~Db();

	private: 
		static Db *instance;

		SQLite::Database *db;
		SQLite::Transaction *transaction;

};