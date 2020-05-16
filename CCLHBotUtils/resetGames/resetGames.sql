-- DELETE game data

DELETE FROM games;
DELETE FROM players;
DELETE FROM gamesxplayers_votedelete;
DELETE FROM gamesxcards_blackcards;
DELETE FROM gamesxcards_roundblackcards;
DELETE FROM playersxgamesxcards_whitecards;
DELETE FROM playersxgamesxcards_roundwhitecards;
DELETE FROM playersxgamesxcards_roundvotes;