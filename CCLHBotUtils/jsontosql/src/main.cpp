#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "json.hpp"

using json = nlohmann::json;

int dictionaryIndex = 1;
std::map<std::string, int64_t> dictionaryIDs;

bool fileExists (const std::string& name) {
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

std::string processLine(std::string type, json row) {
	std::string sql_sentence;
	if (type == "users") {
		int64_t id = row["user_id"];
		std::string name = row["username"];

		replaceAll(name, "'", "''");

		sql_sentence = "INSERT INTO users (id, name) VALUES("+std::to_string(id)+", '"+name+"');\n";
	} else if (type == "dictionaries") {
		int64_t id = dictionaryIndex++;
		std::string name = row["name"];
		int64_t creator_id = row["creator_uid"];
		int64_t message_id = row["msg_id"];
		uint8_t finished = row["finished"];
		
		replaceAll(name, "'", "''");

		dictionaryIDs.emplace(std::pair<std::string, int64_t>(row["_id"]["$oid"], id));

		sql_sentence = "INSERT INTO dictionaries (id, name, creator_id, message_id, published) VALUES("+std::to_string(id)+", '"+name+"', "+std::to_string(creator_id)+", "+std::to_string(message_id)+", "+std::to_string(finished)+");\n";
	} else if (type == "whitecards") {
		std::string text = row["card_text"];
		int64_t dictionary_id = 0;
		std::map<std::string, int64_t>::iterator it = dictionaryIDs.find(row["dictionary_id"]["$oid"]);
		if (it != dictionaryIDs.end()){
			dictionary_id = it->second;
		}
		
		replaceAll(text, "'", "''");

		sql_sentence = "INSERT INTO cards (text, type, dictionary_id) VALUES('"+text+"', 1, "+std::to_string(dictionary_id)+");\n";
	} else if (type == "blackcards") {
		std::string text = row["card_text"];
		int64_t dictionary_id = 0;
		std::map<std::string, int64_t>::iterator it = dictionaryIDs.find(row["dictionary_id"]["$oid"]);
		if (it != dictionaryIDs.end()){
			dictionary_id = it->second;
		}
		
		replaceAll(text, "'", "''");

		sql_sentence = "INSERT INTO cards (text, type, dictionary_id) VALUES('"+text+"', 2, "+std::to_string(dictionary_id)+");\n";
	}

	return sql_sentence;
}

void proccessJSON(std::string type) {
	//Check if json file exists
	if (!fileExists(type + ".json")) {
		std::cerr << "Error [code 1]: no se ha podido encontrar el fichero '" + type + ".json' junto al ejecutable." << std::endl;
		std::exit(0);
	}

	//Read and parse json file
	std::ifstream json_file(type + ".json", std::ifstream::in);
	json j;
	json_file >> j;
	
	//Check if the json file is correct
	if (j.type() != json::value_t::array) {
		std::cerr << "Error [code 2]: el formato del archivo json es incorrecto (" + std::string(j.type_name()) + "), todos los objetos deben ir dentro de un array." << std::endl;
		std::exit(0);
	}

	//Create the buffer string
	std::string sql_sentences;

	//Add a delete instruction
	if (type == "users") {
		sql_sentences += "-- Users\n";
		sql_sentences += "DELETE FROM users;\n";
	} else if (type == "dictionaries") {
		sql_sentences += "-- Dictionaries\n";
		sql_sentences += "DELETE FROM dictionaries;\n";
	} else if (type == "whitecards") {
		sql_sentences += "-- WhiteCards\n";
		sql_sentences += "DELETE FROM cards WHERE type = 1;\n";
	} else if (type == "blackcards") {
		sql_sentences += "-- BlackCards\n";
		sql_sentences += "DELETE FROM cards WHERE type = 2;\n";
	}

	//Process the json file object by object
	for (auto& element : j) {
		std::string sentence = processLine(type.c_str(), element);

		//Only insert the line if it's not duplicated
		std::size_t found = sql_sentences.find(sentence);
  		if (found == std::string::npos) sql_sentences += sentence;
	}

	//Write sql
	std::ofstream sql_file(type+".sql", std::ifstream::trunc);
	sql_file << sql_sentences;
}

int main(int argc, char* argv[]) {	
	//Process json files
	proccessJSON("users");
	proccessJSON("dictionaries");
	proccessJSON("blackcards");
	proccessJSON("whitecards");

	return 0;
}