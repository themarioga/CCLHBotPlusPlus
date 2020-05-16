-- Add collabs

DROP TABLE IF EXISTS usersxdictionaries_collaborators;
CREATE TABLE IF NOT EXISTS usersxdictionaries_collaborators (
	user_id		INTEGER		NOT NULL,
	dictionary_id	INTEGER		NOT NULL,
    can_edit        INTEGER     DEFAULT 0,
    accepted        INTEGER     DEFAULT 0,
	PRIMARY KEY (user_id, dictionary_id),
	FOREIGN KEY (user_id) REFERENCES users(id),
	FOREIGN KEY (dictionary_id) REFERENCES dictionaries(id)
);

-- Add creators to collab table
INSERT INTO usersxdictionaries_collaborators SELECT creator_id, id, 1, 1 FROM dictionaries;