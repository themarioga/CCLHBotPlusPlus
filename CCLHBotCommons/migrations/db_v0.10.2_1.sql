-- v0.10.2_1 - Add new configuration for pagination

-- Configurations
INSERT INTO configurations VALUES ('dictionaries_per_page', '5');
UPDATE configurations SET conf_value = '0.10.2' WHERE conf_key = 'bot_version';