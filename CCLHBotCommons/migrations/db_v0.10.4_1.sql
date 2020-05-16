-- Replace single quote

-- Version number
UPDATE configurations SET conf_value = '0.10.4' WHERE conf_key = 'bot_version';

-- Version number
UPDATE configurations SET conf_value = '0.1.1' WHERE conf_key = 'dictionaries_bot_version';

-- Replace single quotes
UPDATE cards SET text = REPLACE(text, '"', '''');