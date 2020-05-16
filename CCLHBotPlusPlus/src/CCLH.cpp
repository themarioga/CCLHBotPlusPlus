#include "CCLH.h"

CCLH::CCLH() {
	//Init Bot
	bot = new Bot(ConfigurationService::GetInstance()->GetConfiguration("bot_token"), ConfigurationService::GetInstance()->GetConfiguration("bot_query_separator"));

	//Start listening for queries
	bot->StartQueryListener();

	//Set up commands and callbacks
	SetUpCommands();
    SetUpQueries();
}

CCLH::~CCLH() {
	delete bot;
}

void CCLH::Listen() {
    bot->Listen();
}

void CCLH::SetUpCommands() {
    bot->AddCommandListener("start", [this](TgBot::Message::Ptr message) {
		//Check if we receive the command in private
		if (message->chat->type != TgBot::Chat::Type::Private) {
			bot->sendMessage(message->chat->id, "Por favor envia este comando por un chat privado.");
			return;
		}

		//Create user object
		User user(message->from->id, Bot::GetUsername(message->from));

		try {
			user.Load();

			//Check name and owner and refresh it if necesary
			if (user.GetName() != Bot::GetUsername(message->from)) user.SetName(Bot::GetUsername(message->from));

			if (!user.GetActive()) {
				user.SetActive(true);

				//Send message
				bot->sendMessage(message->chat->id, 
				"¡He reactivado tu usuario!\n"
				"Siempre que necesites el menú inicial puedes escribir /start\n"
				);
			} else {
				//Send message
				bot->sendMessage(message->chat->id, "¡Hola "+user.GetName()+"!\nPara crear una partida usa el comando /create en un grupo.");
			}
		} catch(ApplicationException& e) {
			//Register user in game
			GameLogic::RegisterUser(user, 
				[this, message](){ //Success
					bot->sendMessage(message->chat->id, 
						"¡Bienvenido! Acabo de añadirte a mi base de datos.\n"
						"Siempre que necesites el menú inicial puedes escribir /start\n"
						"¡Gracias por unirte!"
					);
				},
				[this, message](std::string errorMessage){ //Failure
					bot->sendMessage(message->chat->id, Bot::GetUsername(message->from)+": "+errorMessage);
				});
		} catch(UnexpectedException &e) {
			bot->sendMessage(message->chat->id, "Ha ocurrido un error. "+std::string(e.what())); 
		}
	});
    bot->AddCommandListener("create", [this](TgBot::Message::Ptr message) {
		//Check if we receive the command in a group
		if (message->chat->type == TgBot::Chat::Type::Private) {
			bot->sendMessage(message->chat->id, Bot::GetUsername(message->from)+": Por favor envia este comando por un grupo.");
			return;
		}

		//Send a provissional message to the room
		TgBot::Message::Ptr sent_message = bot->sendMessage(message->chat->id, "Un momentito, estoy creando la partida...");

		//Create user object
		User user(message->from->id, Bot::GetUsername(message->from));

		//Create room object
		Room room(message->chat->id, message->chat->title, bot->GetChatCreatorId(message->chat->id));

		//Create game
		GameLogic::CreateGame(user, room, sent_message->messageId, 
			[this, message, sent_message](){ //Success
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Democracia", "create_type_democracia") }));
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Clásico", "create_type_clasico") }));
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Dictadura", "create_type_dictadura") }));
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(message->chat->id)) }));
					
				//Edit previous message
				bot->editMessage(message->chat->id, sent_message->messageId, "¡Ya he creado la partida!\nAhora elige el tipo de juego: ", keyboard);
			},
			[this, message](std::string errorMessage){ //Failure
				bot->sendMessage(message->chat->id, Bot::GetUsername(message->from)+": "+errorMessage);
			});
	});
    bot->AddCommandListener("newdictionary", [this](TgBot::Message::Ptr message) {
		bot->sendMessage(message->chat->id, 
			"La creacion de diccionarios se traslada al bot @cclhdictionariesbot, el cual estará desactivado unos dias.\n\n"
			"Lamento las molestias."
		);
	});
    bot->AddCommandListener("help", [this](TgBot::Message::Ptr message) {
		bot->sendMessage(message->chat->id, 
			"Bienvenido a la ayuda de "+ConfigurationService::GetInstance()->GetConfiguration("bot_name")+" versión "+ConfigurationService::GetInstance()->GetConfiguration("bot_version")+".\n"
			"Puedes consultar la ayuda en el siguiente enlace: http://telegra.ph/Manual-del-bot-Cartas-Contra-la-Humanidad-cclhbot-01-31\n"
			"Disfrutad del bot y... ¡A jugar!\n\n"
			"Creado por "+ConfigurationService::GetInstance()->GetConfiguration("bot_owner_alias")+"."
		);
	});
    bot->AddCommandListener("sendMessage", [this](TgBot::Message::Ptr message) {
		if (std::to_string(message->from->id) == ConfigurationService::GetInstance()->GetConfiguration("bot_owner_id")) {
			//Send message to me
			std::string msgText = Util::ReplaceAll(message->text, "/sendMessage ", "");
			bot->sendMessage(std::stol(ConfigurationService::GetInstance()->GetConfiguration("bot_owner_id")), "Enviando texto: "+msgText);
			//Send message to all users
			UserService userService;
			std::vector<User> users = userService.GetAllUsers();
			for (User user : users) {
				try {
					bot->sendMessage(user.GetID(), msgText);
				} catch (std::runtime_error &e) {
					user.SetActive(false);
					std::cerr << "["+Util::GetCurrentDatetime()+"] Error (sendMessage): " << e.what() << std::endl;
				}
			}
		}
	});
    bot->AddCommandListener("deleteGame", [this](TgBot::Message::Ptr message) {
		if (std::to_string(message->from->id) == ConfigurationService::GetInstance()->GetConfiguration("bot_owner_id")) {
			//Send message to me
			int64_t userID = std::stoi(Util::ReplaceAll(message->text, "/deleteGame ", ""));
			
			bot->sendMessage(message->from->id, "Borrando partida de "+std::to_string(userID));

			User user(userID);
			Game game;
			GameLogic::DeleteGameByCreator(user, game, 
				[&](std::vector< std::pair<int64_t, int64_t> > messageList){ //Success
					bot->sendMessage(message->from->id, "Partida borrada en la sala: "+game.GetName());
					for (std::pair<int64_t, int64_t> messagePair : messageList) {
						//Edit previous message
						bot->editMessage(messagePair.first, messagePair.second, "Partida borrada.");
					}
				},
				[&](std::string errorMessage){ //Failure
					bot->sendMessage(message->from->id, Bot::GetUsername(message->from)+": "+errorMessage);
				});
		}
	});
    bot->AddCommandListener("deleteAllGames", [this](TgBot::Message::Ptr message) {
		if (std::to_string(message->from->id) == ConfigurationService::GetInstance()->GetConfiguration("bot_owner_id")) {
			GameService gameService = GameService();
			std::vector<Game> games = gameService.GetAllGames();
			
			bot->sendMessage(message->from->id, "Se borrarán "+std::to_string(games.size())+" partidas.");

			for (int i = 0; i < games.size(); i++) {
				User creator(games[i].GetCreatorID());
				DeleteGame(creator, games[i]);
			}
			
			bot->sendMessage(message->from->id, "Se han borrado "+std::to_string(games.size())+" partidas.");
		}
	});
}

void CCLH::SetUpQueries() {
    bot->AddCallbackQueryListener("create", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 2 && parameters.size() != 3) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Load the user
		User user(query->from->id, Bot::GetUsername(query->from));

		//Load game
		Game game(query->message->chat->id);

		//Process the command
		if (parameters[0] == "type") {
			//Answer the query
			bot->answerCallbackQuery(query->id, "Seleccionando el modo de juego...");

			//Set game type
			GameLogic::SetGameType(user, game, parameters[1],
				[&](){ //Success
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
						TgButton("1", "create_ncardstowin_1"), 
						TgButton("2", "create_ncardstowin_2"), 
						TgButton("3", "create_ncardstowin_3") 
					}));
					keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
						TgButton("4", "create_ncardstowin_4"), 
						TgButton("5", "create_ncardstowin_5"), 
						TgButton("6", "create_ncardstowin_6") 
					}));
					keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
						TgButton("7", "create_ncardstowin_7"), 
						TgButton("8", "create_ncardstowin_8"), 
						TgButton("9", "create_ncardstowin_9") 
					}));
					keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
						
					//Edit previous message
					bot->editMessage(game.GetID(), game.GetMessageID(), 
                        "¡Perfecto! A continuacion elige el número de cartas negras necesarias para ganar la partida: ",
						keyboard);
				},
				[this, query](std::string errorMessage){ //Failure
					bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
				});
		} else if (parameters[0] == "ncardstowin") {
			//Answer the query
			bot->answerCallbackQuery(query->id, "Seleccionando "+parameters[1]+" como numero de cartas negras necesarias para ganar...");

			//Set the number of players
			GameLogic::SetGameNumberOfCardsToWin(user, game, std::stoi(parameters[1]),
				[&](){ //Success
					GetDictionaries(0, game);
				},
				[this, query](std::string errorMessage){ //Failure
					bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
				});
		} else if (parameters[0] == "dictionary") {
			if (parameters[1] == "nav"){
				//Answer the query
				bot->answerCallbackQuery(query->id, "Cambiando página...");

				//Get the dictionaries by offset
				GetDictionaries(std::stoi(parameters[2]), game);
			} else {
				//Answer the query
				bot->answerCallbackQuery(query->id, "Seleccionando el diccionario de esta partida...");

				//Check if the dictionary exists
				Dictionary dictionary(std::stol(parameters[1]));

				//Set the number of players
				GameLogic::SetGameDictionary(user, game, dictionary,
					[&](){ //Success
						//Create a markup keyboard
						TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Unirse a la partida", "join_"+std::to_string(game.GetID())) }));
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
							
						//Edit previous message
						bot->editMessage(game.GetID(), game.GetMessageID(),
                            "¡Ya has configurado la partida!\n"
							"Ahora invita a otros jugadores a unirse. " 
							"Para eso solo tienen que hacer click en \"Unirse a la partida\".\n"
							"Por ahora se ha unido: \n\n"+user.GetName(),
							keyboard);
					},
					[this, query](std::string errorMessage){ //Failure
						bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
					});
					
				//Send message to player
				TgBot::Message::Ptr message = bot->sendMessage(user.GetID(), "Un momento...");

				//Create player object for creator autojoin	
				Player player(user.GetID(), user.GetName(), game.GetID(), message->messageId, 0);

				//Create player
				GameLogic::CreatePlayer(player, game, 
					[&](){ //Success
						//Create a markup keyboard
						TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

						//If the user is the creator of the game let him delete the game
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));

						//Edit previous player message
						bot->editMessage(player.GetID(), player.GetMessageID(),"Has creado una partida en el grupo "+game.GetName(), keyboard);
					},
					[this, message, query](std::string errorMessage){ //Failure
						bot->sendMessage(message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
					});
			}
		}
	});

	bot->AddCallbackQueryListener("join", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Uniendote a la partida...");

		//Send message
		TgBot::Message::Ptr message = bot->sendMessage(query->from->id, "Uniendote a la partida...");

		//Create game object
		Game game(query->message->chat->id);

		//Create player object for creator autojoin	
		Player player(query->from->id, Bot::GetUsername(query->from), game.GetID(), message->messageId, 0);

		//Create player
		GameLogic::CreatePlayerWithChecks(player, game, 
			[&](std::vector<Player> &players){ //Success
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr player_keyboard(new TgBot::InlineKeyboardMarkup);

				//If the user is the creator of the game let him delete the game
				player_keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Dejar la partida", "leave_"+std::to_string(game.GetID())) }));

				//Edit previous player message
				bot->editMessage(player.GetID(), player.GetMessageID(), "Te has unido a una partida en el grupo "+game.GetName(), player_keyboard);

				//Create a string with players names and add current player's name
				std::string playerNames = GameLogic::GetPlayerNamesFromPlayerArray(players, "", "\n");
				
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
				if (players.size() < std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_max_number_of_players"))) {
					keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Unirse a la partida", "join_"+std::to_string(game.GetID())) }));
				}
				if (players.size() >= std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_min_number_of_players"))) {
					keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Iniciar la partida", "start_"+std::to_string(game.GetID())) }));
				}
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
				
				//If there are still players left
				if (players.size() < std::stoi(ConfigurationService::GetInstance()->GetConfiguration("game_max_number_of_players"))) {
					//Edit previous message
					bot->editMessage(game.GetID(), game.GetMessageID(),
                        "Invita a otros jugadores a unirse. "
						"Para ello solo tienen que hacer click en \"Unirse a la partida\".\n"
						"Cuando todos se hayan unido haced click en \"Iniciar la partida\".\n"
						"Por ahora se han unido:\n\n"+playerNames, 
						keyboard);
				} else {
					//Edit previous message
					bot->editMessage(game.GetID(), game.GetMessageID(),
                        "¡Ya esta todo el mundo! Ahora haz click en \"Iniciar la partida\" para comenzar a jugar. "
						"Van a jugar esta partida: \n"+playerNames,
						keyboard);
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
			});
	});
	
	bot->AddCallbackQueryListener("start", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer callback query
		bot->answerCallbackQuery(query->id, "Iniciando partida...");

		//Create user object
		User user(query->from->id);

		//Create game object
		Game game(query->message->chat->id);
		
		//Start game
		GameLogic::StartGame(user, game, 
			[&](){ //Success
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
				
				//Edit previous message to show ingame message
				bot->editMessage(game.GetID(), game.GetMessageID(), 
                    "¡A jugar!\nAqui tienes algunas acciones adicionales que puedes hacer:",
					keyboard);

				//Tell the dictator that he will be the leader all the game
				if (game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) {
					bot->sendMessage(game.GetPresidentID(), "Eres el dictador en esta partida, lo que significa que no podrás elegir cartas pero serás el unico que vota.");
				}

				StartRound(game);
			},
			[this, query](std::string errorMessage){ //Failure
				bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
				return;
			});
	});

	bot->AddCallbackQueryListener("delete", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Create user object
		User user(query->from->id);

		//Create game object
		Game game(std::stol(parameters[0]));

		//Load game
		try {
			game.Load();
		} catch (std::exception &e) {
			std::cerr << "["+Util::GetCurrentDatetime()+"] Error (Delete): " << e.what() << std::endl;
			bot->sendMessage(query->message->chat->id, e.what());
			return;
		}

		//Check if the user who sends this query is the creator of the game
		if (user.GetID() == game.GetCreatorID()) {
			//Answer the query
			bot->answerCallbackQuery(query->id, "Borrando partida...");

			//Delete the game
			DeleteGame(user, game);
		} else {
			//Answer the query
			bot->answerCallbackQuery(query->id, "Votando para borrar la partida...");

			//Vote to delete the game
			GameLogic::VoteToDeleteGame(user, game, 
				[&](int8_t voteCount){ //Success
					//If at least half of the players have voted
					if (voteCount > game.GetNumberOfPlayers() / 2) {
						//Delete the game
						DeleteGame(user, game);
					}
				},
				[this, query](std::string errorMessage){ //Failure
					bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
				});
		}
	});

	bot->AddCallbackQueryListener("leave", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		bot->answerCallbackQuery(query->id, "Esta caracteristica aun no ha sido implementada");
		
		//ToDo: leave
	});

	bot->AddCallbackQueryListener("card", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Enviando carta...");

		//Create player object
		Player player(query->from->id);

		//Create card object
		Card card(std::stol(parameters[0]));

		//Vote to delete the game
		GameLogic::SelectWhiteCard(player, card, query->message->messageId, 
			[&](Game &game, std::vector<Player> &players, RoundBlackCard &roundBlackCard, std::vector<RoundWhiteCard> &roundWhiteCards){ //Success
				//Check if all players have chosen their cards
				if ((game.GetType() == GameTypeEnum::GAME_DEMOCRACY && players.size() == game.GetNumberOfPlayers())
					|| (game.GetType() == GameTypeEnum::GAME_CLASSIC && players.size() == game.GetNumberOfPlayers() - 1)
					|| (game.GetType() == GameTypeEnum::GAME_DICTATORSHIP && players.size() == game.GetNumberOfPlayers() - 1)) {
					//Get card text
					std::string texts = GameLogic::GetCardsTextFromCardArray(roundWhiteCards, "- ", "\n");

					//Edit previous message to show ingame message
					bot->editMessage(roundBlackCard.game_id, roundBlackCard.message_id,
						"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
						"La carta negra de esta ronda es:\n"+roundBlackCard.card.GetText()+"\n\n"
						"Y estas son las cartas que habeis elegido como respuesta:\n"+texts+"\n"
						"¡Ahora comienza la votacion!");

					//Shuffle cards
					std::random_shuffle(roundWhiteCards.begin(), roundWhiteCards.end());

					//Send vote options
					if (game.GetType() == GameTypeEnum::GAME_DEMOCRACY) {
						//Send vote options to all the players
						for (RoundWhiteCard roundWhiteCard : roundWhiteCards) {
							//Create a markup keyboard
							TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

							//Add vote options to the keyboard
							for (RoundWhiteCard card : roundWhiteCards) {
								keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton(card.card.GetText(), "vote_"+std::to_string(card.card.GetID())) }));
							}

							//Send a message to the user
							TgBot::Message::Ptr message = bot->editMessage(roundWhiteCard.player_id, roundWhiteCard.message_id, 
								"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
								"La carta negra de esta ronda es:\n"+roundBlackCard.card.GetText()+"\n\n"
								"Estas son las cartas que habeis enviado:\n\n"+texts+"\n"
								"¡Ahora vota tu favorita!", 
								keyboard);
						}
					} else if (game.GetType() == GameTypeEnum::GAME_CLASSIC || game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) {
						//Get the dictator
						User leader(game.GetPresidentID());
						leader.Load();

						//Send vote options to all the players
						for (RoundWhiteCard roundWhiteCard : roundWhiteCards) {
							//Send a message to the user
							TgBot::Message::Ptr message = bot->editMessage(roundWhiteCard.player_id, roundWhiteCard.message_id,
								"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
								"La carta negra de esta ronda es:\n"+roundBlackCard.card.GetText()+"\n\n"
								"Estas son las cartas que habeis enviado:\n\n"+texts+"\n"
								"¡Ahora el lider "+leader.GetName()+" votará su favorita favorita!");
						}

						//Create a markup keyboard
						TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
 
						//Add vote options to the keyboard
						for (RoundWhiteCard card : roundWhiteCards) {
							keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton(card.card.GetText(), "vote_"+std::to_string(card.card.GetID())) }));
						}

						//Send a message to the user
						TgBot::Message::Ptr message = bot->sendMessage(game.GetPresidentID(), 
							"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
							"La carta negra de esta ronda es:\n"+roundBlackCard.card.GetText()+"\n\n"
							"Estas son las cartas que han enviado:\n\n"+texts+"\n"
							"¡Ahora vota tu favorita!", 
							keyboard);
					}
				} else {
					//Get playername string from vector
					std::string player_names = GameLogic::GetPlayerNamesFromPlayerArray(players, "", "\n");

					//Edit previous message to show ingame message
					bot->editMessage(roundBlackCard.game_id, roundBlackCard.message_id,
						"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
						"La carta negra de esta ronda es:\n"+roundBlackCard.card.GetText()+"\n\n"
						"Hasta ahora han enviado carta:\n"+player_names+"\n"
						"¡Seguid mandando cartas!");

					//Edit previous player message
					bot->editMessage(player.GetID(), query->message->messageId, "Has elegido la carta: "+card.GetText());
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
			});
	});

	bot->AddCallbackQueryListener("vote", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Enviando carta...");

		//Create player object
		Player player(query->from->id);

		//Create card object
		Card card(std::stol(parameters[0]));

		//Vote to delete the game
		GameLogic::VoteWhiteCard(player, card, query->message->messageId, 
			[&](Game &game, std::vector<Player> &playersThatVoted, RoundBlackCard &roundBlackCard){ //Success
				//Check if everyone have voted
				if ((game.GetType() == GameTypeEnum::GAME_DEMOCRACY && playersThatVoted.size() == game.GetNumberOfPlayers())
				|| (game.GetType() == GameTypeEnum::GAME_CLASSIC && playersThatVoted.size() == 1)
				|| (game.GetType() == GameTypeEnum::GAME_DICTATORSHIP && playersThatVoted.size() == 1)) {
					GameLogic::EndGameRound(game, card, 
						[&](Card &mostVotedCard, Player &mostVotedPlayer, std::vector<RoundWhiteCard> &votes, std::vector<Player> &players) { //Success
							//Player list with points
							std::string names = "";
							for (Player player : players) {
								names += "- " + player.GetName() + " - " + std::to_string(player.GetPoints())+"\n";
							}

							//Edit previous message to show ingame message
							bot->editMessage(game.GetID(), roundBlackCard.message_id,
								"¡Felicidades! "+mostVotedPlayer.GetName()+" ha ganado la carta negra de la ronda numero "+std::to_string(game.GetRoundNumber())+
                                " '"+roundBlackCard.card.GetText()+"' con su carta:\n\n"
								"- "+mostVotedCard.GetText()+"\n\n"
								"Asi han quedado las puntuaciones:\n"+names);

							//We iterate with players
							for (RoundWhiteCard vote : votes){
								//If it's the player who won
								if (vote.player_id == mostVotedPlayer.GetID()) {
									//Edit player's message
									bot->editMessage(vote.player_id, vote.message_id,
										"¡Enhorabuena! Has ganado la carta negra de la ronda numero "+std::to_string(game.GetRoundNumber())+
                                        " '"+roundBlackCard.card.GetText()+"' con tu carta:\n\n"
										"- "+mostVotedCard.GetText());
								} else {
									//Edit player's message
									bot->editMessage(vote.player_id, vote.message_id,
										"¡Vaya! "+mostVotedPlayer.GetName()+" ha ganado la carta negra de la ronda numero "+std::to_string(game.GetRoundNumber())+
                                        " '"+roundBlackCard.card.GetText()+"' con su carta:\n\n"
										"- "+mostVotedCard.GetText());
								}
							}

							//If player have won the game
							if (mostVotedPlayer.GetPoints() == game.GetNumberOfCardsToWin()) {
								//Send a message to the user
								bot->sendMessage(mostVotedPlayer.GetID(), 
									"¡¡¡Felicidades!!!\n\n"
									"¡¡¡Has ganado la partida!!!");

								//Send a message to the game
								bot->sendMessage(game.GetID(), 
									"¡Se acabo!\n\n"
									"El jugador "+mostVotedPlayer.GetName()+" ha ganado la partida");

								//Create user for deletion
								User user(game.GetCreatorID());
								
								//Delete this game
								DeleteGame(user, game);
							} else {
								StartRound(game);
							}
						},
						[this, query](std::string errorMessage){ //Failure
							bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
						});
				} else {
					//Create the message
					std::string game_text = "Ronda "+std::to_string(game.GetRoundNumber())+"\n";
					
					game_text += "La carta negra de esta ronda es:\n"+roundBlackCard.card.GetText()+"\n\n";

					//Get playername string from vector
					std::string player_names = GameLogic::GetPlayerNamesFromPlayerArray(playersThatVoted, "", "\n");

					game_text += "Hasta ahora han votado:\n"+player_names+"\n";

					if (game.GetType() == GameTypeEnum::GAME_CLASSIC) {
						User leader(game.GetPresidentID());
						leader.Load();
						game_text += "\nEl lider de esta ronda es "+leader.GetName()+"\n";
					}
					
					game_text += "\n¡Seguid votando!";

					//Edit previous message to show ingame message
					bot->editMessage(roundBlackCard.game_id, roundBlackCard.message_id, game_text);

					//Edit previous player message
					bot->editMessage(player.GetID(), query->message->messageId, "Has votado la carta: "+card.GetText());
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->sendMessage(query->message->chat->id, Bot::GetUsername(query->from)+": "+errorMessage);
			});
	});
}

void CCLH::StartRound(Game &game) {
	//Send this round black card to the group	
	TgBot::Message::Ptr message = bot->sendMessage(game.GetID(), "Iniciando ronda...");

	//Start round
	GameLogic::StartGameRound(game, message->messageId, 
		[&](Card roundBlackCard, std::map< int64_t, std::vector<Card> > roundPlayerCards){ //Success
			std::string text = "Ronda " + std::to_string(game.GetRoundNumber()) + "\n"
				"La carta negra de esta ronda es: \n" + roundBlackCard.GetText();

			if (game.GetType() == GameTypeEnum::GAME_CLASSIC) {
				User leader(game.GetPresidentID());
				leader.Load();
 				text += "\n\nEl lider de esta ronda es "+leader.GetName();
			}

			//Send this round black card to the group	
			bot->editMessage(game.GetID(), message->messageId, text);

			//Iterate players
			for (std::map< int64_t, std::vector<Card> >::iterator it = roundPlayerCards.begin(); it != roundPlayerCards.end(); ++it) {
				//If you are the president you don't pick cards
				if (it->first == game.GetPresidentID()) {
					//If it's classic we have to warn the current president.
					if (game.GetType() == GameTypeEnum::GAME_CLASSIC) {
						bot->sendMessage(it->first, 
							"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
							"Eres el presidente de esta ronda. Cuando los demas jugadores hayan elegido sus cartas podrás votar.");
					}
				} else {
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					for (Card card : it->second) {
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton(card.GetText(), "card_"+std::to_string(card.GetID())) }));
					}

					//Send a new message with the cards
					bot->sendMessage(it->first, 
						"Ronda "+std::to_string(game.GetRoundNumber())+"\n"
						"La carta negra de esta ronda es: \n" + roundBlackCard.GetText() + "\n\nTus cartas son: ",
                        keyboard);
				}
			}
		},
		[&](std::string errorMessage){ //Failure
			bot->sendMessage(game.GetID(), errorMessage);
		});
}

void CCLH::DeleteGame(User &user, Game &game) {
	GameLogic::DeleteGame(user, game, 
		[&](std::vector< std::pair<int64_t, int64_t> > messageList){ //Success
			for (std::pair<int64_t, int64_t> messagePair : messageList) {
				//Edit previous message
				bot->editMessage(messagePair.first, messagePair.second, "Partida borrada.");
			}
		},
		[&](std::string errorMessage){ //Failure
			bot->sendMessage(game.GetID(), user.GetName()+": "+errorMessage);
		});
}

void CCLH::GetDictionaries(int8_t offset, Game &game) {
	//Load game
	game.Load();

	//Get dictionaries by offset
	GameLogic::GetDictionaries(game.GetCreatorID(), offset, [&](int64_t count, std::vector<Dictionary>& dictionaries){ //Success
		//Dictionaries per page
		int8_t dictionaries_per_page = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_per_page"));

		//Create a markup keyboard
		TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

		for (Dictionary dictionary : dictionaries) {
			keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
				TgButton(dictionary.GetName(), "create_dictionary_"+std::to_string(dictionary.GetID())) 
			}));	
		}

		std::vector<TgButton> button_row;
		if (offset > 0) button_row.push_back(TgButton("⬅️", "create_dictionary_nav_"+std::to_string(offset-dictionaries_per_page)));
		if (count > offset + dictionaries.size()) button_row.push_back(TgButton("➡️", "create_dictionary_nav_"+std::to_string(offset+dictionaries_per_page)));
		if (button_row.size() > 0) keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(button_row));
		
		keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));

		//Edit previous message
		bot->editMessage(game.GetID(), game.GetMessageID(), "¡Genial! Por ultimo elige la bajara con la que quereis jugar la partida: ", keyboard);
	},
	[&](std::string errorMessage){ //Failure
		bot->sendMessage(game.GetID(), errorMessage);
	});
}