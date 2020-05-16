-- v0.10.1_1 - Add dates and new configurations

-- Configurations
INSERT INTO configurations VALUES ('game_min_number_of_players', '3');
INSERT INTO configurations VALUES ('game_max_number_of_players', '9');
UPDATE configurations SET conf_value = '0.10.1' WHERE conf_key = 'bot_version';

-- Basic tables
ALTER TABLE users ADD register_date TEXT;
ALTER TABLE rooms ADD register_date TEXT;
ALTER TABLE cards ADD creation_date TEXT;
ALTER TABLE dictionaries ADD creation_date TEXT;