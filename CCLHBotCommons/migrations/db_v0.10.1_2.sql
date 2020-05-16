-- v0.10.1_1 - Add dates to advanced tables

-- Advanced tables
ALTER TABLE games ADD creation_date TEXT;
ALTER TABLE players ADD join_date TEXT;