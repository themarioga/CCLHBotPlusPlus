#pragma once

#include <iostream>
#include <fstream>
#include <string>

class Util {
	public:
		static void AssertTrue(bool expression, std::string message = "") {
			if (!expression) {
				if (message != "") std::cerr << message << std::endl;
				else std::cerr << "AssertTrue failed." << std::endl;

				std::abort();
			}
		}
		
		static void AssertFalse(bool expression, std::string message = "") {
			if (expression) {
				if (message != "") std::cerr << message << std::endl;
				else std::cerr << "AssertFalse failed." << std::endl;

				std::abort();
			}
		}

		static std::streampos GetFileSize( const char* filePath ) {
			std::streampos fsize = 0;
			std::ifstream file( filePath, std::ios::binary );

			fsize = file.tellg();
			file.seekg( 0, std::ios::end );
			fsize = file.tellg() - fsize;
			file.close();

			return fsize;
		}

		static std::string ReplaceAll(std::string main_string, std::string old_substring, std::string new_substring) {
			size_t index = 0;
			while (true) {
				/* Locate the substring to replace. */
				index = main_string.find(old_substring, index);
				if (index == std::string::npos) break;

				/* Make the replacement. */
				main_string.replace(index, old_substring.length(), new_substring);

				/* Advance index forward so the next iteration doesn't pick it up as well. */
				index += old_substring.length();
			}

			return main_string;
		}
};