#include "GameLogic.h"

GameLogic::GameLogic() {

}

GameLogic::~GameLogic() {

}

void GameLogic::RegisterUser(User &user, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Create a new user
		user.Create();
		
		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::CreateGame(User &user, Room &room, int64_t messageID, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Create or load room
		CreateOrLoadRoom(room);

		//Load user
		user.Load();
		
		//Create game
		Game game(room.GetID(), room.GetName(), room.GetOwnerID(), user.GetID(), messageID);
		game.Create();

		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::SetGameType(User &user, Game &game, std::string type, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Check if this query has already been done
		if (game.GetType() != 0) return;

		//Set the type to game
		if (type == "democracia") game.SetType(GameTypeEnum::GAME_DEMOCRACY);
		else if (type == "clasico") game.SetType(GameTypeEnum::GAME_CLASSIC);
		else if (type == "dictadura") game.SetType(GameTypeEnum::GAME_DICTATORSHIP);
		else throw UnexpectedException("Comando no válido.");
		
		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::SetGameNumberOfPlayers(User &user, Game &game, int8_t numberOfPlayers, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Check if this query has already been done
		if (game.GetNumberOfPlayers() != 0) return;

		//Set the type to game
		game.SetNumberOfPlayers(numberOfPlayers);
		
		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::SetGameNumberOfCardsToWin(User &user, Game &game, int8_t numberOfCardsToWin, std::function<void(std::vector<Dictionary>&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Check if this query has already been done
		if (game.GetNumberOfCardsToWin() != 0) return;

		//Set the type to game
		game.SetNumberOfCardsToWin(numberOfCardsToWin);
		
		//Get dictionaries
		std::vector<Dictionary> dictionaries = dictionaryService.GetAllActiveDictionaries();
		
		//If success
		successCallback(dictionaries);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::SetGameDictionary(User &user, Game &game, Dictionary &dictionary, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Check if this query has already been done
		if (game.GetDictionaryID() != 0) return;

		//Load dictionary
		dictionary.Load();

		//Set the type to game
		game.SetDictionaryID(dictionary.GetID());

		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::StartGame(User &user, Game &game, std::function<void()> successCallback, std::function<void(std::string)> failureCallback){
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Check the status of the game
		if (game.GetStatus() == GameStatusEnum::GAME_CREATED) throw GameNotConfiguredException();
		else if (game.GetStatus() == GameStatusEnum::GAME_STARTED) throw GameAlreadyStartedException();

		//Check the number of players in the game
		std::vector<Player> players = playerService.GetPlayersInGame(game.GetID()); 
		if (players.size() < game.GetNumberOfPlayers()) throw GameNotFilledException();
		
		//Change game status
		game.SetStatus(GameStatusEnum::GAME_STARTED);

		//Calculate quantities
		const int32_t numberOfBlackCards = game.GetNumberOfCardsToWin() * game.GetNumberOfPlayers();
		const int32_t cardsInHand = std::stoi(configurationService.GetConfiguration("game_whitecards_in_hand"));
		const int32_t cardsPerPlayer = game.GetNumberOfCardsToWin() * cardsInHand;

		//Get and add black cards to the game
		cardService.AddBlackCardsToGameDeck(game.GetDictionaryID(), game.GetID(), numberOfBlackCards);

		//Get and add white cards to the game
		cardService.AddWhiteCardsToPlayerDeck(game.GetDictionaryID(), players, cardsPerPlayer);

		//Check if game is classic or dictator
		if (game.GetType() == GameTypeEnum::GAME_CLASSIC || game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) {
			game.SetPresidentID(game.GetCreatorID());
		}

		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::StartGameRound(Game &game, int64_t blackCardMessageID, std::function<void(Card, std::map< int64_t, std::vector<Card> >)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//Get a black card for the current round
		Card roundBlackCard = cardService.GetFirstBlackCardFromGameDeck(game.GetID());

		//Add it to the database
		cardService.AddBlackCardToCurrentRound(game.GetID(), roundBlackCard.GetID(), blackCardMessageID);

		//Delete the card from the deck
		cardService.DeleteBlackCardFromGameDeck(roundBlackCard.GetID(), game.GetID());
		
		//Get players
		std::vector<Player> players = playerService.GetPlayersInGame(game.GetID());

		//If game type is classic
		if (game.GetType() == GameTypeEnum::GAME_CLASSIC){
			//Get current president
			int64_t playerIndex = playerService.GetPlayerIndexFromPlayerArray(players, game.GetPresidentID());

			//Set next president by order
			if (playerIndex != -1) {
				if (playerIndex + 1 < players.size()) {
					game.SetPresidentID(players[playerIndex + 1].GetID());
				} else {
					game.SetPresidentID(players[0].GetID());
				}
			}
		}

		//Create map
		std::map< int64_t, std::vector<Card> > cardsPerPlayer;

		//Send white cards to every player
		for (Player player : players) {
			//Get player cards
			std::vector<Card> cards = cardService.GetWhiteCardsFromPlayerDeck(player.GetID(), std::stoi(configurationService.GetConfiguration("game_whitecards_in_hand")));
			
			//Insert player cards in the map
			cardsPerPlayer[player.GetID()] = cards;
		}

		//If success
		successCallback(roundBlackCard, cardsPerPlayer);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::EndGameRound(Game &game, Card &card, std::function<void(Card &card, Player &player, std::vector<RoundWhiteCard> &roundWhiteCards)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Get the most voted card
		Card mostVotedCard = cardService.GetMostVotedCardAtCurrentRound(game.GetID());
		
		//Find the propietary of the most voted card
		Player mostVotedPlayer = cardService.GetPlayerFromRoundWhiteCardID(card.GetID());

		//Get most voted player points
		int8_t pointsAfterWin = mostVotedPlayer.GetPoints() + 1;

		//Set winner player new points
		mostVotedPlayer.SetPoints(pointsAfterWin);

		//Get current round votes
		std::vector<RoundWhiteCard> votes = cardService.GetVotesFromCurrentRound(game.GetID());

		//Delete round cards
		cardService.DeleteAllWhiteCardsFromCurrentRound(game.GetID());

		//Delete round votes
		cardService.DeleteAllVotesFromCurrentRound(game.GetID());

		//Delete the used black card
		cardService.DeleteBlackCardFromCurrentRound(game.GetID());

		//If success
		successCallback(mostVotedCard, mostVotedPlayer, votes);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::DeleteGame(User &user, Game &game, std::function<void(std::vector< std::pair<int64_t, int64_t> >)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Create return vector
		std::vector< std::pair<int64_t, int64_t> > messageIDs;

		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Add game's message
		messageIDs.push_back(std::pair<int64_t, int64_t>(game.GetID(), game.GetMessageID()));
		
		//Get all the players in the game
		std::vector<Player> players = playerService.GetPlayersInGame(game.GetID());

		//Add all player's mesages
		for (Player player : players) {
			messageIDs.push_back(std::pair<int64_t, int64_t>(player.GetID(), player.GetMessageID()));
		}

		//ToDo: añadir los mensajes de las cartas y demás

		//Delete the all the game content
		DeleteAllGameReferences(game);

		//If success
		successCallback(messageIDs);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::VoteToDeleteGame(User &user, Game &game, std::function<void(int8_t)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//Add vote
		gameService.VoteDelete(game.GetID(), user.GetID());

		//If success
		successCallback(gameService.GetVoteDeletePlayerCount(game.GetID()));
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::CreatePlayer(Player &player, Game &game, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Create player for this game
		player.Create();

		//If success
		successCallback();
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::CreatePlayerWithChecks(Player &player, Game &game, std::function<void(std::vector<Player>&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Check user
		User user(player.GetID());
		user.Load();

		//Load game
		game.Load();

		//Check the status of the game
		if (game.GetStatus() == GameStatusEnum::GAME_CREATED) throw GameNotConfiguredException();
		else if (game.GetStatus() == GameStatusEnum::GAME_STARTED) throw GameAlreadyStartedException();

		//Check the number of players in the game
		std::vector<Player> playersInGame = playerService.GetPlayersInGame(game.GetID()); 
		if (playersInGame.size() == game.GetNumberOfPlayers()) throw GameAlreadyFilledException();

		//Create player for this game
		player.Create();
			
		//Add new player to the list
		playersInGame.push_back(player);

		//If success
		successCallback(playersInGame);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::SelectWhiteCard(Player &player, Card &card, int64_t messageID, std::function<void(Game&, std::vector<Player>&, RoundBlackCard&, std::vector<RoundWhiteCard>&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load player
		player.Load();

		//Load game
		Game game(player.GetGameID());
		game.Load();

		//Check the status of the game
		if (game.GetStatus() != GameStatusEnum::GAME_STARTED) throw GameNotConfiguredException();

		//Check if the player can send card
		if ((game.GetType() == GameTypeEnum::GAME_CLASSIC || game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) 
			&& player.GetID() == game.GetPresidentID()) throw UnexpectedException("El presidente no puede elegir carta");

		//Load card
		card.Load();
		
		//Delete card from deck
		cardService.DeleteWhiteCardFromPlayerDeck(player.GetID(), card.GetID());
		
		//Use white card
		cardService.AddWhiteCardToCurrentRound(player.GetID(), game.GetID(), card.GetID(), messageID);

		//Get the players that have sent card
		std::vector<Player> players = cardService.GetPlayersThatSentWhiteCards(game.GetID());

		//Send black card to group
		RoundBlackCard blackCard = cardService.GetBlackCardFromCurrentRound(game.GetID());

		//Get this round cards
		std::vector<RoundWhiteCard> whiteCards = cardService.GetWhiteCardsFromCurrentRound(game.GetID());
			
		//If success
		successCallback(game, players, blackCard, whiteCards);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::VoteWhiteCard(Player &player, Card &card, int64_t messageID, std::function<void(Game&, std::vector<Player>&, RoundBlackCard&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load player
		player.Load();
		
		//Load card
		card.Load();

		//Load game
		Game game(player.GetGameID());
		game.Load();

		//Check the status of the game
		if (game.GetStatus() != GameStatusEnum::GAME_STARTED) throw GameNotConfiguredException();
		
		//Check if the player can vote card
		if ((game.GetType() == GameTypeEnum::GAME_CLASSIC || game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) 
			&& player.GetID() != game.GetPresidentID()) throw UnexpectedException("Solo el presidente puede elegir carta");

		//Register the vote
		cardService.AddVoteToCurrentRound(game.GetID(), player.GetID(), card.GetID(), messageID);

		//Get the players who have voted in this round
		std::vector<Player> players = cardService.GetPlayersThatVotedWhiteCards(game.GetID());

		//Send black card to group
		RoundBlackCard roundBlackCard = cardService.GetBlackCardFromCurrentRound(game.GetID());
			
		//If success
		successCallback(game, players, roundBlackCard);
	} catch (ApplicationException& e) {
		failureCallback(e.what());
	} catch (UnexpectedException& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (std::runtime_error& e) {
		failureCallback(e.what());
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

void GameLogic::CreateOrLoadRoom(Room &room) {
	try{
		room.Create();
	} catch (RoomAlreadyExistsException &e) {
		room.Load();
	}
}

void GameLogic::DeleteAllGameReferences(Game &game) {
	//Delete current round votes
	cardService.DeleteAllVotesFromCurrentRound(game.GetID());

	//Delete current round black card
	cardService.DeleteBlackCardFromCurrentRound(game.GetID());

	//Delete current round white cards
	cardService.DeleteAllWhiteCardsFromCurrentRound(game.GetID());

	//Delete black cards
	cardService.DeleteBlackCardsFromGameDeck(game.GetID());

	//Delete white cards
	cardService.DeleteAllWhiteCardsFromAllPlayerDecks(game.GetID());

	//Delete votes for deletion
	gameService.ClearVoteDelete(game.GetID());
	
	//Delete playeres in game
	playerService.DeletePlayersInGame(game.GetID());

	//Delete the game
	game.Delete();
}