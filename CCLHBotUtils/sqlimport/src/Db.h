#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <SQLiteCpp/SQLiteCpp.h>

#include "Util.hpp"
#include "Exceptions/NoResultsException.h"

class Db {

	public: 
		static Db *Instance() {
			if (!instance)
			instance = new Db();
			return instance;
		}

		//Db methods
		void Connect(std::string);
		void Import(std::string);
		void BulkImport(std::string);

	protected:
		Db();
		~Db();

	private: 
		static Db *instance;

		SQLite::Database *db;

};