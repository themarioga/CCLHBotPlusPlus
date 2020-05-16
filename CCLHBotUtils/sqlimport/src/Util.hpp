#pragma once

#include <iostream>
#include <fstream>
#include <string>

class Util {
	public:
		static void AssertTrue(bool expression, std::string message = "") {
			if (!expression){
				if (message != "") std::cerr << message << std::endl;
				else std::cerr << "AssertTrue failed." << std::endl;

				std::abort();
			}
		}
		
		static void AssertFalse(bool expression, std::string message = "") {
			if (expression){
				if (message != "") std::cerr << message << std::endl;
				else std::cerr << "AssertFalse failed." << std::endl;

				std::abort();
			}
		}

		static std::streampos GetFileSize( const char* filePath ){
			std::streampos fsize = 0;
			std::ifstream file( filePath, std::ios::binary );

			fsize = file.tellg();
			file.seekg( 0, std::ios::end );
			fsize = file.tellg() - fsize;
			file.close();

			return fsize;
		}
};