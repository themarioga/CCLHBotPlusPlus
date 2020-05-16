-- v0.10.3_1 - First version of database

-- Version number
UPDATE configurations SET conf_value = '0.10.3' WHERE conf_key = 'bot_version';

-- Dictionaries bot config
INSERT INTO configurations VALUES 
('dictionaries_bot_token', ''),
('dictionaries_bot_name', 'Diccionarios para CCLHBot'),
('dictionaries_bot_alias', '@cclhdictionariesbot'),
('dictionaries_bot_version', '0.1.0'),
('dictionaries_bot_owner_id', ''),
('dictionaries_bot_owner_alias', '@themarioga'),
('dictionaries_bot_query_separator', '_'),
('dictionaries_max_collaborators', '10');

-- Modify dictionary table
ALTER TABLE dictionaries ADD COLUMN shared NUMBER DEFAULT 0;