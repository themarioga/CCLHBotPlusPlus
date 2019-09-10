-- v0.1 - First version of database

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
	message_id		INTEGER		NOT NULL,
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

-- Tables with inheritance

DROP TABLE IF EXISTS games;
CREATE TABLE IF NOT EXISTS games (
	room_id			INTEGER		NOT NULL,
	creator_id		INTEGER		NOT NULL,
	message_id		INTEGER		NOT NULL,
	status			INTEGER		NOT NULL,
	type			INTEGER,
	n_players		INTEGER,
	n_cards_to_win	INTEGER,
	dictionary_id	INTEGER,
	president_id	INTEGER,
	PRIMARY KEY (room_id),
	FOREIGN KEY (room_id) REFERENCES rooms(id),
	FOREIGN KEY (creator_id) REFERENCES users(id),
	FOREIGN KEY (dictionary_id) REFERENCES dictionaries(id)
);

CREATE INDEX games_idx_id ON games(room_id);

DROP TABLE IF EXISTS players;
CREATE TABLE IF NOT EXISTS players (
	user_id			INTEGER		NOT NULL,
	game_id			INTEGER		NOT NULL,
	message_id		INTEGER		NOT NULL,
	join_order		INTEGER		NOT NULL,
	points			INTEGER		DEFAULT 0,
	PRIMARY KEY (user_id),
	FOREIGN KEY(user_id) REFERENCES users(id),
	FOREIGN KEY(game_id) REFERENCES games(id)
);

CREATE INDEX players_idx_id ON players(user_id);

-- Cross-reference tables

DROP TABLE IF EXISTS gamesxplayers_votedelete;
CREATE TABLE IF NOT EXISTS gamesxplayers_votedelete (
	player_id		INTEGER		NOT NULL,
	game_id			INTEGER		NOT NULL,
	PRIMARY KEY (player_id, game_id),
	FOREIGN KEY (player_id) REFERENCES players(user_id),
	FOREIGN KEY (game_id) REFERENCES games(room_id)
);

DROP TABLE IF EXISTS gamesxcards_blackcards;
CREATE TABLE IF NOT EXISTS gamesxcards_blackcards (
	game_id			INTEGER		NOT NULL,
	card_id			INTEGER		NOT NULL,
	card_order		INTEGER		NOT NULL,
	PRIMARY KEY (game_id, card_id),
	FOREIGN KEY (game_id) REFERENCES games(room_id),
	FOREIGN KEY (card_id) REFERENCES cards(id)
);

DROP TABLE IF EXISTS gamesxcards_roundblackcards;
CREATE TABLE IF NOT EXISTS gamesxcards_roundblackcards (
	game_id			INTEGER		NOT NULL,
	card_id			INTEGER		NOT NULL,
	message_id		INTEGER		NOT NULL,
	PRIMARY KEY (game_id),
	FOREIGN KEY (game_id) REFERENCES games(game_id),
	FOREIGN KEY (card_id) REFERENCES cards(id)
);

DROP TABLE IF EXISTS playersxgamesxcards_whitecards;
CREATE TABLE IF NOT EXISTS playersxgamesxcards_whitecards (
	player_id		INTEGER		NOT NULL,
	game_id			INTEGER		NOT NULL,
	card_id			INTEGER		NOT NULL,
	card_order		INTEGER		NOT NULL,
	PRIMARY KEY (player_id, card_id),
	FOREIGN KEY (player_id) REFERENCES players(user_id),
	FOREIGN KEY (game_id) REFERENCES games(room_id),
	FOREIGN KEY (card_id) REFERENCES cards(id)
);

DROP TABLE IF EXISTS playersxgamesxcards_roundwhitecards;
CREATE TABLE IF NOT EXISTS playersxgamesxcards_roundwhitecards (
	player_id		INTEGER		NOT NULL,
	game_id			INTEGER		NOT NULL,
	card_id			INTEGER		NOT NULL,
	message_id		INTEGER		NOT NULL,
	PRIMARY KEY (player_id),
	FOREIGN KEY (player_id) REFERENCES players(user_id),
	FOREIGN KEY (game_id) REFERENCES games(game_id),
	FOREIGN KEY (card_id) REFERENCES cards(id)
);

DROP TABLE IF EXISTS playersxgamesxcards_roundvotes;
CREATE TABLE IF NOT EXISTS playersxgamesxcards_roundvotes (
	player_id		INTEGER		NOT NULL,
	game_id			INTEGER		NOT NULL,
	card_id			INTEGER		NOT NULL,
	message_id		INTEGER		NOT NULL,
	PRIMARY KEY (player_id),
	FOREIGN KEY (player_id) REFERENCES players(user_id),
	FOREIGN KEY (game_id) REFERENCES games(game_id),
	FOREIGN KEY (card_id) REFERENCES cards(id)
);