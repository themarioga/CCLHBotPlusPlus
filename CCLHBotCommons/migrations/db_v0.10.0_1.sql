-- v0.10.0_1 - First version of database (basic tables)

--Bot config

DROP TABLE IF EXISTS configurations;
CREATE TABLE IF NOT EXISTS configurations (
	conf_key		TEXT		NOT NULL,
	conf_value		TEXT		NOT NULL,
	PRIMARY KEY (conf_key)
);

INSERT INTO configurations VALUES 
('bot_token', ''),
('bot_name', 'Cartas contra la humanidad'),
('bot_alias', '@cclhbot'),
('bot_version', '0.10.0'),
('bot_owner_id', ''),
('bot_owner_alias', '@themarioga'),
('bot_query_separator', '_')
('game_whitecards_in_hand', '5'),
('dictionaries_min_whitecards', '405'),
('dictionaries_min_blackcards', '50'),;

-- Basic tables

DROP TABLE IF EXISTS users;
CREATE TABLE IF NOT EXISTS users (
	id				INTEGER		NOT NULL,
	name			TEXT		NOT NULL,
	active			INTEGER		DEFAULT 1,
	PRIMARY KEY (id)
);

CREATE INDEX users_idx_id ON users(id);

DROP TABLE IF EXISTS rooms;
CREATE TABLE IF NOT EXISTS rooms (
	id				INTEGER		NOT NULL,
	name			TEXT		NOT NULL,
	owner_id		INTEGER		NOT NULL,
	active			INTEGER		DEFAULT 1,
	PRIMARY KEY (id),
	FOREIGN KEY (owner_id) REFERENCES users(id)
);

CREATE INDEX rooms_idx_id ON rooms(id);

DROP TABLE IF EXISTS dictionaries;
CREATE TABLE IF NOT EXISTS dictionaries (
	id				INTEGER		NOT NULL,
	name			TEXT		NOT NULL	UNIQUE,
	creator_id		INTEGER		NOT NULL,
	published		INTEGER		DEFAULT 0,
	PRIMARY KEY (id),
	FOREIGN KEY (creator_id) REFERENCES users(id)
);

CREATE INDEX dictionaries_idx_id ON dictionaries(id);

DROP TABLE IF EXISTS cards;
CREATE TABLE IF NOT EXISTS cards (
	id				INTEGER		NOT NULL,
	text			TEXT		NOT NULL,
	type			INTEGER		NOT NULL,
	dictionary_id	INTEGER		NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (text, type, dictionary_id),
	FOREIGN KEY (dictionary_id) REFERENCES dictionaries(id)
);

CREATE INDEX cards_idx_id ON cards(id);