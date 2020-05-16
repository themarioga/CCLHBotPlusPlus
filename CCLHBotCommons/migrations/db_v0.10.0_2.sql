-- v0.10.0_1 - First version of database (advanced tables)

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
	FOREIGN KEY(game_id) REFERENCES games(room_id)
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
	FOREIGN KEY (game_id) REFERENCES games(room_id),
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
	FOREIGN KEY (game_id) REFERENCES games(room_id),
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
	FOREIGN KEY (game_id) REFERENCES games(room_id),
	FOREIGN KEY (card_id) REFERENCES cards(id)
);