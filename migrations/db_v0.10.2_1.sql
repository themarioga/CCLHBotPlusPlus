-- v0.10.2_1 - Add new configuration for pagination and column for round_number

-- Configurations
INSERT INTO configurations VALUES ('dictionaries_per_page', '5');
UPDATE configurations SET conf_value = '0.10.2' WHERE conf_key = 'bot_version';

-- Advanced tables
ALTER TABLE games ADD COLUMN round_number NUMBER DEFAULT 0;