-- Replace single quote

-- Version number
UPDATE configurations SET conf_value = '0.10.5' WHERE conf_key = 'bot_version';

-- Rename configs
UPDATE configurations SET conf_key = 'cclh_bot_token' WHERE conf_key = 'bot_token';
UPDATE configurations SET conf_key = 'cclh_bot_name' WHERE conf_key = 'bot_name';
UPDATE configurations SET conf_key = 'cclh_bot_alias' WHERE conf_key = 'bot_alias';
UPDATE configurations SET conf_key = 'cclh_bot_version' WHERE conf_key = 'bot_version';
UPDATE configurations SET conf_key = 'cclh_bot_owner_id' WHERE conf_key = 'bot_owner_id';
UPDATE configurations SET conf_key = 'cclh_bot_owner_alias' WHERE conf_key = 'bot_owner_alias';
UPDATE configurations SET conf_key = 'cclh_bot_query_separator' WHERE conf_key = 'bot_query_separator';

-- Bots config
INSERT INTO configurations VALUES 
('cclh_bot_webhook_url', ''),
('cclh_bot_webhook_port', '8080'),
('dictionaries_bot_webhook_url', ''),
('dictionaries_bot_webhook_port', '8080');
