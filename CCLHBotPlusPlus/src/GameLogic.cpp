#include "GameLogic.h"

void GameLogic::RegisterUser(User &user, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Create a new user
		user.Create();
		
		//If success
		successCallback();
	} catch (UserAlreadyExistsException& e) {
		try {
			//Load the user
			user.Load();
		} catch (UserNotActiveException& e) {
			//If it's not active
			user.SetActive(true);

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

void GameLogic::SetGameNumberOfCardsToWin(User &user, Game &game, int8_t numberOfCardsToWin, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Check if this query has already been done
		if (game.GetNumberOfCardsToWin() != 0) return;

		//Set the type to game
		game.SetNumberOfCardsToWin(numberOfCardsToWin);
		
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
		std::vector<Player> players = PlayerService::GetPlayersInGame(game.GetID()); 
		if (players.size() < std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_min_number_of_players"))) throw GameNotFilledException();
		
		//Change game status
		game.SetStatus(GameStatusEnum::GAME_STARTED);

		//Set game number of players
		game.SetNumberOfPlayers(players.size());

		//Calculate quantities
		const int32_t numberOfBlackCards = game.GetNumberOfCardsToWin() * game.GetNumberOfPlayers();
		const int32_t cardsInHand = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_whitecards_in_hand"));
		const int32_t cardsPerPlayer = game.GetNumberOfCardsToWin() * cardsInHand;

		//Get and add black cards to the game
		CardService::AddBlackCardsToGameDeck(game.GetDictionaryID(), game.GetID(), numberOfBlackCards);

		//Get and add white cards to the game
		CardService::AddWhiteCardsToPlayerDeck(game.GetDictionaryID(), players, cardsPerPlayer);

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

void GameLogic::StartGameRound(Game &game, int64_t blackCardMessageID, std::function<void(Card&, std::map< int64_t, std::vector<Card> >&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//Increase round number
		game.SetRoundNumber(game.GetRoundNumber() + 1);

		//Get a black card for the current round
		Card roundBlackCard = CardService::GetFirstBlackCardFromGameDeck(game.GetID());

		//Add it to the database
		CardService::AddBlackCardToCurrentRound(game.GetID(), roundBlackCard.GetID(), blackCardMessageID);

		//Delete the card from the deck
		CardService::DeleteBlackCardFromGameDeck(roundBlackCard.GetID(), game.GetID());
		
		//Get players
		std::vector<Player> players = PlayerService::GetPlayersInGame(game.GetID());

		//If game type is classic
		if (game.GetType() == GameTypeEnum::GAME_CLASSIC){
			//Get current president
			int64_t playerIndex = PlayerService::GetPlayerIndexFromPlayerArray(players, game.GetPresidentID());

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
			std::vector<Card> cards = CardService::GetWhiteCardsFromPlayerDeck(player.GetID(), std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_whitecards_in_hand")));
			
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

void GameLogic::EndGameRound(Game &game, Card &card, std::function<void(Card&, Player&, std::vector<RoundWhiteCard>&, std::vector<Player>&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Get the most voted card
		Card mostVotedCard = CardService::GetMostVotedCardAtCurrentRound(game.GetID());
		
		//Find the propietary of the most voted card
		Player mostVotedPlayer = CardService::GetPlayerFromRoundWhiteCardID(game.GetID(), mostVotedCard.GetID());

		//Get most voted player points
		int8_t pointsAfterWin = mostVotedPlayer.GetPoints() + 1;

		//Set winner player new points
		mostVotedPlayer.SetPoints(pointsAfterWin);

		//Get current round votes
		std::vector<RoundWhiteCard> votes = CardService::GetVotesFromCurrentRound(game.GetID());

		//Delete round cards
		CardService::DeleteAllWhiteCardsFromCurrentRound(game.GetID());

		//Delete round votes
		CardService::DeleteAllVotesFromCurrentRound(game.GetID());

		//Delete the used black card
		CardService::DeleteBlackCardFromCurrentRound(game.GetID());
		
		//Get players
		std::vector<Player> players = PlayerService::GetPlayersInGame(game.GetID());

		//If success
		successCallback(mostVotedCard, mostVotedPlayer, votes, players);
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

void GameLogic::DeleteGame(User &user, Game &game, std::function<void(std::vector< std::pair<int64_t, int64_t> >&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load game
		game.Load();

		//And check if the user who sends this query is the creator of the game
		if (user.GetID() != game.GetCreatorID()) throw PlayerNotEnoughPermissionsException();

		//Create return vector
		std::vector< std::pair<int64_t, int64_t> > messageIDs = GetMessageIDsFromGame(game);

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

void GameLogic::DeleteGameByCreator(User &user, Game &game, std::function<void(std::vector< std::pair<int64_t, int64_t> >&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Load the user
		user.Load();

		//Load game
		game = GameService::GetGameByCreatorID(user.GetID());

		//Create return vector
		std::vector< std::pair<int64_t, int64_t> > messageIDs = GetMessageIDsFromGame(game);

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
		GameService::VoteDelete(game.GetID(), user.GetID());

		//If success
		successCallback(GameService::GetVoteDeletePlayerCount(game.GetID()));
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

void GameLogic::LeaveGame(User &user, Game &game, std::function<void()> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Check user
		user.Load();

		//Load game
		game.Load();

		//Load player
		Player player(user.GetID());
		player.Load();

		//Leave game
		player.Delete();

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
		User user(player.GetID(), player.GetName());
		user.Load();

		//Load game
		game.Load();

		//Check the status of the game
		if (game.GetStatus() == GameStatusEnum::GAME_CREATED) throw GameNotConfiguredException();
		else if (game.GetStatus() == GameStatusEnum::GAME_STARTED) throw GameAlreadyStartedException();

		//Check the number of players in the game
		std::vector<Player> playersInGame = PlayerService::GetPlayersInGame(game.GetID()); 
		if (playersInGame.size() == std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_max_number_of_players"))) throw GameAlreadyFilledException();

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
		CardService::DeleteWhiteCardFromPlayerDeck(player.GetID(), card.GetID());
		
		//Use white card
		CardService::AddWhiteCardToCurrentRound(player.GetID(), game.GetID(), card.GetID(), messageID);

		//Get the players that have sent card
		std::vector<Player> players = CardService::GetPlayersThatSentWhiteCards(game.GetID());

		//Send black card to group
		RoundBlackCard blackCard = CardService::GetBlackCardFromCurrentRound(game.GetID());

		//Get this round cards
		std::vector<RoundWhiteCard> whiteCards = CardService::GetWhiteCardsFromCurrentRound(game.GetID());
			
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
		CardService::AddVoteToCurrentRound(game.GetID(), player.GetID(), card.GetID(), messageID);

		//Get the players who have voted in this round
		std::vector<Player> players = CardService::GetPlayersThatVotedWhiteCards(game.GetID());

		//Send black card to group
		RoundBlackCard roundBlackCard = CardService::GetBlackCardFromCurrentRound(game.GetID());
			
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

void GameLogic::GetDictionaries(int64_t creator_id, int8_t offset, std::function<void(int64_t, std::vector<Dictionary>&)> successCallback, std::function<void(std::string)> failureCallback) {
	try {
		//Get dictionary count
		int64_t count = DictionaryService::GetActiveDictionariesCount(true, true, creator_id);
		
		//Get dictionaries
		std::vector<Dictionary> dictionaries = DictionaryService::GetAllDictionaries(true, true, creator_id, -1, std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_per_page")), offset);
			
		//If success
		successCallback(count, dictionaries);
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
	CardService::DeleteAllVotesFromCurrentRound(game.GetID());

	//Delete current round black card
	CardService::DeleteBlackCardFromCurrentRound(game.GetID());

	//Delete current round white cards
	CardService::DeleteAllWhiteCardsFromCurrentRound(game.GetID());

	//Delete black cards
	CardService::DeleteBlackCardsFromGameDeck(game.GetID());

	//Delete white cards
	CardService::DeleteAllWhiteCardsFromAllPlayerDecks(game.GetID());

	//Delete votes for deletion
	GameService::ClearVoteDelete(game.GetID());
	
	//Delete playeres in game
	PlayerService::DeletePlayersInGame(game.GetID());

	//Delete the game
	game.Delete();
}

std::vector< std::pair<int64_t, int64_t> > GameLogic::GetMessageIDsFromGame(Game &game) {
	//Create return vector
	std::vector< std::pair<int64_t, int64_t> > messageIDs;

	//Add game's message
	messageIDs.push_back(std::pair<int64_t, int64_t>(game.GetID(), game.GetMessageID()));

	//Get the black cards
	RoundBlackCard blackcard = CardService::GetBlackCardFromCurrentRound(game.GetID());
	messageIDs.push_back(std::pair<int64_t, int64_t>(blackcard.game_id, blackcard.message_id));
	
	//Get all the players in the game
	std::vector<Player> players = PlayerService::GetPlayersInGame(game.GetID());

	//Add all player's mesages
	for (Player player : players) {
		messageIDs.push_back(std::pair<int64_t, int64_t>(player.GetID(), player.GetMessageID()));
	}

	//Get the white cards
	std::vector<RoundWhiteCard> whitecards = CardService::GetWhiteCardsFromCurrentRound(game.GetID());

	for (RoundWhiteCard card : whitecards) {
		messageIDs.push_back(std::pair<int64_t, int64_t>(card.player_id, card.message_id));
	}

	//Get the votes
	std::vector<RoundWhiteCard> votes = CardService::GetVotesFromCurrentRound(game.GetID());

	for (RoundWhiteCard vote : votes) {
		messageIDs.push_back(std::pair<int64_t, int64_t>(vote.player_id, vote.message_id));
	}

	return messageIDs;
}

std::string GameLogic::GetPlayerNamesFromPlayerArray(std::vector<Player>& players, std::string pre, std::string post) {
	std::string names = "";
	for (Player player : players) {
		names += pre + player.GetName() + post;
	}
	return names;
}

std::string GameLogic::GetCardsTextFromCardArray(std::vector<RoundWhiteCard>& cards, std::string pre, std::string post) {
	std::string texts = "";
	for (RoundWhiteCard card : cards) {
		texts += pre + card.card.GetText() + post;
	}
	return texts;
}