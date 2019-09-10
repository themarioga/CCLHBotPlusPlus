#include "Bot.h"

#include <tgbot/tools/StringTools.h>

Bot::Bot() {
	bot = new TgBot::Bot(configurationService.GetConfiguration("bot_token"));
}

Bot::~Bot() {
	delete bot;
}

void Bot::Listen() {
	while (true) {
		try {
			std::clog << "Iniciando bot con username: " << bot->getApi().getMe()->username.c_str() << std::endl;

			TgBot::TgLongPoll longPoll(*bot);

			std::clog << "Comenzando longPoll" << std::endl;
			while (true) {
				try {
					longPoll.start();
				} catch (std::runtime_error& e) {
					std::cerr << "Error (listen): " << e.what() << std::endl;
				}
			}
		} catch (std::runtime_error& e) {
			std::cerr << "Error (longPoll): " << e.what() << std::endl;
		}
	}
}

void Bot::StartQueryListener() {
	bot->getEvents().onCallbackQuery([this](TgBot::CallbackQuery::Ptr query) {
		TgBot::Message::Ptr message = query->message;
		
		//Parse the query
		std::vector<std::string> result = StringTools::split(query->data, configurationService.GetConfiguration("bot_query_separator")[0]);
		if (!result.size()) {
			bot->getApi().sendMessage(query->from->id, "Error: Comando no válido.");
			std::cerr << "Error: Comando no válido." << std::endl;
			return;
		}

		//Search the query in the db
		std::map< std::string, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)> >::iterator it;
		it = queries.find(result[0]);
		if (it != queries.end()) {
			//Call the asociated function
			result.erase(result.begin());
			it->second(result, query);
		} else {
			//Show error message
			bot->getApi().sendMessage(query->from->id, "Error: Comando no válido.");
			std::cerr << "Error: Comando no válido." << std::endl;
			return;
		}
	});
}

void Bot::SetUpCommands() {
	bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
		//Check if we receive the command in private
		if (message->chat->type != TgBot::Chat::Type::Private) {
			bot->getApi().sendMessage(message->chat->id, "Por favor envia este comando por un chat privado.");
			std::exit(0);
		}

		//Create user object
		User user(message->from->id, this->GetUsername(message->from));

		//Register user in game
		gameLogic.RegisterUser(user, 
			[this, message](){ //Success
				bot->getApi().sendMessage(message->chat->id, 
					"¡Bienvenido! Acabo de añadirte a mi base de datos.\n"
					"Puedes añadirme a un grupo y usar el comando \"/create\" para crear una partida.\n\n"
					"¡Gracias por unirte!"
				);
			},
			[this, message](std::string errorMessage){ //Failure
				bot->getApi().sendMessage(message->chat->id, errorMessage);
			});
	});

	bot->getEvents().onCommand("create", [this](TgBot::Message::Ptr message) {
		//Check if we receive the command in a group
		if (message->chat->type == TgBot::Chat::Type::Private) {
			bot->getApi().sendMessage(message->chat->id, "Por favor envia este comando por un grupo.");
			std::exit(0);
		}

		//Send a provissional message to the room
		TgBot::Message::Ptr sent_message = bot->getApi().sendMessage(message->chat->id, "Un momentito, estoy creando la partida...");

		//Create user object
		User user(message->from->id, GetUsername(message->from));

		//Create room object
		Room room(message->chat->id, message->chat->title, GetChatCreatorId(message->chat->id));

		//Create game
		gameLogic.CreateGame(user, room, sent_message->messageId, 
			[this, message, sent_message](){ //Success
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
				keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Democracia", "create_type_democracia") }));
				keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Clásico", "create_type_clasico") }));
				keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Dictadura", "create_type_dictadura") }));
				keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(message->chat->id)) }));
					
				//Edit previous message
				bot->getApi().editMessageText("¡Ya he creado la partida!\nAhora elige el tipo de juego: ", message->chat->id, sent_message->messageId, "", "", false, keyboard);
			},
			[this, message](std::string errorMessage){ //Failure
				bot->getApi().sendMessage(message->chat->id, errorMessage);
			});
	});

	AddCallbackQueryListener("create", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 2) {
			bot->getApi().answerCallbackQuery(query->id, "Comando no válido");
		}

		//Load the user
		User user(query->from->id, GetUsername(query->from));

		//Load game
		Game game(query->message->chat->id);

		//Process the command
		if (parameters[0] == "type") {
			//Answer the query
			bot->getApi().answerCallbackQuery(query->id, "Seleccionando "+parameters[1]+" como modo de juego...");

			//Set game type
			gameLogic.SetGameType(user, game, parameters[1],
				[&](){ //Success
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("3", "create_nplayers_3"), TgButton("4", "create_nplayers_4") }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("5", "create_nplayers_5"), TgButton("6", "create_nplayers_6") }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("7", "create_nplayers_7"), TgButton("8", "create_nplayers_8") }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("9", "create_nplayers_9") }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
						
					//Edit previous message
					bot->getApi().editMessageText("¡Perfecto! Ahora elige el número de jugadores: ", game.GetID(), game.GetMessageID(), "", "", false, keyboard);
				},
				[this, query](std::string errorMessage){ //Failure
					bot->getApi().sendMessage(query->message->chat->id, errorMessage);
				});
		} else if (parameters[0] == "nplayers") {
			//Answer the query
			bot->getApi().answerCallbackQuery(query->id, "Seleccionando "+parameters[1]+" como número de jugadores...");

			//Set the number of players
			gameLogic.SetGameNumberOfPlayers(user, game, std::stoi(parameters[1]),
				[&](){ //Success
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { 
						TgButton("1", "create_ncardstowin_1"), 
						TgButton("2", "create_ncardstowin_2"), 
						TgButton("3", "create_ncardstowin_3") 
					}));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { 
						TgButton("4", "create_ncardstowin_4"), 
						TgButton("5", "create_ncardstowin_5"), 
						TgButton("6", "create_ncardstowin_6") 
					}));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { 
						TgButton("7", "create_ncardstowin_7"), 
						TgButton("8", "create_ncardstowin_8"), 
						TgButton("9", "create_ncardstowin_9") 
					}));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
						
					//Edit previous message
					bot->getApi().editMessageText("¡De acuerdo! A continuacion elige el número de cartas negras necesarias para ganar la partida: ", game.GetID(), game.GetMessageID(), 
						"", "", false, keyboard);
				},
				[this, query](std::string errorMessage){ //Failure
					bot->getApi().sendMessage(query->message->chat->id, errorMessage);
				});
		} else if (parameters[0] == "ncardstowin") {
			//Answer the query
			bot->getApi().answerCallbackQuery(query->id, "Seleccionando "+parameters[1]+" como numero de cartas negras necesarias para ganar...");

			//Set the number of players
			gameLogic.SetGameNumberOfCardsToWin(user, game, std::stoi(parameters[1]),
				[&](std::vector<Dictionary> &dictionaries){ //Success
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

					for (Dictionary dictionary : dictionaries) {
						keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { 
							TgButton(dictionary.GetName(), "create_dictionary_"+std::to_string(dictionary.GetID())) 
						}));	
					}
						
					//Edit previous message
					bot->getApi().editMessageText("¡Genial! Por ultimo elige la bajara con la que quereis jugar la partida: ", game.GetID(), game.GetMessageID(), "", "", false, keyboard);
				},
				[this, query](std::string errorMessage){ //Failure
					bot->getApi().sendMessage(query->message->chat->id, errorMessage);
				});
		} else if (parameters[0] == "dictionary") {
			//Answer the query
			bot->getApi().answerCallbackQuery(query->id, "Seleccionando "+parameters[1]+" como el diccionario de esta partida...");

			//Check if the dictionary exists
			Dictionary dictionary(std::stol(parameters[1]));

			//Set the number of players
			gameLogic.SetGameDictionary(user, game, dictionary,
				[&](){ //Success
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Unirse a la partida", "join_"+std::to_string(game.GetID())) }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
						
					//Edit previous message
					bot->getApi().editMessageText("¡Ya has configurado la partida!\n"
						"Ahora invita a otros jugadores a unirse. " 
						"Para eso solo tienen que hacer click en \"Unirse a la partida\".\n"
						"Por ahora se ha unido: \n\n"+user.GetName(), game.GetID(), game.GetMessageID(), 
						"", "", false, keyboard);
				},
				[this, query](std::string errorMessage){ //Failure
					bot->getApi().sendMessage(query->message->chat->id, errorMessage);
				});
				
			//Send message to player
			TgBot::Message::Ptr message = bot->getApi().sendMessage(user.GetID(), "Un momento...");

			//Create player object for creator autojoin	
			Player player(user.GetID(), user.GetName(), game.GetID(), message->messageId, 0);

			//Create player
			gameLogic.CreatePlayer(player, game, 
				[&](){ //Success
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

					//If the user is the creator of the game let him delete the game
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));

					//Edit previous player message
					bot->getApi().editMessageText("Has creado una partida en el grupo "+game.GetName(), player.GetID(), player.GetMessageID(),
						"", "", false, keyboard);
				},
				[this, message](std::string errorMessage){ //Failure
					bot->getApi().sendMessage(message->chat->id, errorMessage);
				});
		}
	});

	AddCallbackQueryListener("join", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->getApi().answerCallbackQuery(query->id, "Comando no válido");
		}

		//Answer the query
		bot->getApi().answerCallbackQuery(query->id, "Uniendote a la partida...");

		//Send message
		TgBot::Message::Ptr message = bot->getApi().sendMessage(query->from->id, "Uniendote a la partida...");

		//Create game object
		Game game(query->message->chat->id);

		//Create player object for creator autojoin	
		Player player(query->from->id, GetUsername(query->from), game.GetID(), message->messageId, 0);

		//Create player
		gameLogic.CreatePlayerWithChecks(player, game, 
			[&](std::vector<Player> &players){ //Success
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

				//If the user is the creator of the game let him delete the game
				keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Dejar la partida", "leave_"+std::to_string(game.GetID())) }));

				//Edit previous player message
				bot->getApi().editMessageText("Te has unido a una partida en el grupo "+game.GetName(), player.GetID(), player.GetMessageID(),
					"", "", false, keyboard);

				//Create a string with players names and add current player's name
				std::string playerNames = GetPlayerNamesFromPlayerArray(players, "", "\n");

				//If there are still players left
				if (players.size() < game.GetNumberOfPlayers()) {
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Unirse a la partida", "join_"+std::to_string(game.GetID())) }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
						
					//Edit previous message
					bot->getApi().editMessageText("Invita a otros jugadores a unirse. "
						"Para ello solo tienen que hacer click en \"Unirse a la partida\".\n"
						"Por ahora se han unido:\n\n"+playerNames, game.GetID(), game.GetMessageID(), 
						"", "", false, keyboard);
				} else {
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Iniciar la partida", "start_"+std::to_string(game.GetID())) }));
					keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
						
					//Edit previous message
					bot->getApi().editMessageText("¡Ya esta todo el mundo! Ahora haz click en \"Iniciar la partida\" para comenzar a jugar. "
						"Van a jugar esta partida: \n"+playerNames, game.GetID(), game.GetMessageID(), 
						"", "", false, keyboard);
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->getApi().sendMessage(query->message->chat->id, errorMessage);
			});
	});
	
	AddCallbackQueryListener("start", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->getApi().answerCallbackQuery(query->id, "Comando no válido");
		}

		//Answer callback query
		bot->getApi().answerCallbackQuery(query->id, "Iniciando partida...");

		//Create user object
		User user(query->from->id);

		//Create game object
		Game game(query->message->chat->id);
		
		//Start game
		gameLogic.StartGame(user, game, 
			[&](){ //Success
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
				keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar la partida", "delete_"+std::to_string(game.GetID())) }));
				
				//Edit previous message to show ingame message
				bot->getApi().editMessageText("¡A jugar!\nAqui tienes algunas acciones adicionales que puedes hacer:", game.GetID(), game.GetMessageID(), 
						"", "", false, keyboard);

				//Tell the dictator that he will be the leader all the game
				if (game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) {
					bot->getApi().sendMessage(game.GetPresidentID(), "Eres el dictador en esta partida, lo que significa que no podrás elegir cartas pero serás el unico que vota.");
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->getApi().sendMessage(query->message->chat->id, errorMessage);
			});

		StartRound(game);
	});

	AddCallbackQueryListener("delete", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->getApi().answerCallbackQuery(query->id, "Comando no válido");
		}

		//Create user object
		User user(query->from->id);

		//Create game object
		Game game(std::stol(parameters[0]));

		//Load game
		try {
			game.Load();
		} catch (std::exception &e) {
			std::cerr << "Error (Delete): " << e.what() << std::endl;
			bot->getApi().sendMessage(query->message->chat->id, e.what());
			return;
		}

		//Check if the user who sends this query is the creator of the game
		if (user.GetID() == game.GetCreatorID()) {
			//Answer the query
			bot->getApi().answerCallbackQuery(query->id, "Borrando partida...");

			//Delete the game
			DeleteGame(user, game);
		} else {
			//Answer the query
			bot->getApi().answerCallbackQuery(query->id, "Votando para borrar la partida...");

			//Vote to delete the game
			gameLogic.VoteToDeleteGame(user, game, 
				[&](int8_t voteCount){ //Success
					//If at least half of the players have voted
					if (voteCount > game.GetNumberOfPlayers() / 2) {
						//Delete the game
						DeleteGame(user, game);
					}
				},
				[this, query](std::string errorMessage){ //Failure
					bot->getApi().sendMessage(query->message->chat->id, errorMessage);
				});
		}
	});

	AddCallbackQueryListener("leave", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		bot->getApi().answerCallbackQuery(query->id, "Esta caracteristica aun no ha sido implementada");
		
		//ToDo: leave
	});

	AddCallbackQueryListener("card", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->getApi().answerCallbackQuery(query->id, "Comando no válido");
		}

		//Answer the query
		bot->getApi().answerCallbackQuery(query->id, "Enviando carta...");

		//Create player object
		Player player(query->from->id);

		//Create card object
		Card card(std::stol(parameters[0]));

		//Vote to delete the game
		gameLogic.SelectWhiteCard(player, card, query->message->messageId, 
			[&](Game &game, std::vector<Player> &players, RoundBlackCard &roundBlackCard, std::vector<RoundWhiteCard> &roundWhiteCards){ //Success
				//Check if all players have chosen their cards
				if ((game.GetType() == GameTypeEnum::GAME_DEMOCRACY && players.size() == game.GetNumberOfPlayers())
					|| (game.GetType() == GameTypeEnum::GAME_CLASSIC && players.size() == game.GetNumberOfPlayers() - 1)
					|| (game.GetType() == GameTypeEnum::GAME_DICTATORSHIP && players.size() == game.GetNumberOfPlayers() - 1)) {
					//Get card text
					std::string texts = GetCardsTextFromCardArray(roundWhiteCards, "- ", "\n");

					//Edit previous message to show ingame message
					bot->getApi().editMessageText(
						"¡Listo! La carta negra de esta ronda era:\n"+roundBlackCard.card.GetText()+"\n\n"
						"Y estas son las cartas que habeis elegido como respuesta:\n"+texts+"\n"
						"¡Ahora comienza la votacion!", roundBlackCard.game_id, roundBlackCard.message_id);

					//Send vote options
					if (game.GetType() == GameTypeEnum::GAME_DEMOCRACY) {
						//Send vote options to all the players
						for (RoundWhiteCard roundWhiteCard : roundWhiteCards) {
							//Create a markup keyboard
							TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

							//Add vote options to the keyboard
							for (RoundWhiteCard card : roundWhiteCards) {
								keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton(card.card.GetText(), "vote_"+std::to_string(card.card.GetID())) }));
							}

							//Send a message to the user
							TgBot::Message::Ptr message = bot->getApi().editMessageText(
								"¡Listo! La carta negra de esta ronda era:\n"+roundBlackCard.card.GetText()+"\n\n"
								"Estas son las cartas que habeis enviado:\n\n"+texts+"\n"
								"¡Ahora vota tu favorita!", 
								roundWhiteCard.player_id, roundWhiteCard.message_id, 
								"", "", false, keyboard);
						}
					} else if (game.GetType() == GameTypeEnum::GAME_CLASSIC || game.GetType() == GameTypeEnum::GAME_DICTATORSHIP) {
						//Send vote options to all the players
						for (RoundWhiteCard roundWhiteCard : roundWhiteCards) {
							//Send a message to the user
							TgBot::Message::Ptr message = bot->getApi().editMessageText(
								"¡Listo! La carta negra de esta ronda era:\n"+roundBlackCard.card.GetText()+"\n\n"
								"Estas son las cartas que habeis enviado:\n\n"+texts+"\n"
								"¡Ahora el lider votará su favorita favorita!", 
								roundWhiteCard.player_id, roundWhiteCard.message_id);
						}

						//Create a markup keyboard
						TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
 
						//Add vote options to the keyboard
						for (RoundWhiteCard card : roundWhiteCards) {
							keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton(card.card.GetText(), "vote_"+std::to_string(card.card.GetID())) }));
						}

						//Send a message to the user
						TgBot::Message::Ptr message = bot->getApi().sendMessage(game.GetPresidentID(), 
							"¡Listo! La carta negra de esta ronda era:\n"+roundBlackCard.card.GetText()+"\n\n"
							"Estas son las cartas que han enviado:\n\n"+texts+"\n"
							"¡Ahora vota tu favorita!", 
							false, 0, keyboard);
					}
				} else {
					//Get playername string from vector
					std::string player_names = GetPlayerNamesFromPlayerArray(players, "", "\n");

					//Edit previous message to show ingame message
					bot->getApi().editMessageText(
						"La carta negra de esta ronda era:\n"+roundBlackCard.card.GetText()+"\n\n"
						"Hasta ahora han enviado carta:\n"+player_names+"\n"
						"¡Seguid mandando cartas!", roundBlackCard.game_id, roundBlackCard.message_id);

					//Edit previous player message
					bot->getApi().editMessageText("Has elegido la carta: "+card.GetText(), player.GetID(), query->message->messageId);
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->getApi().sendMessage(query->message->chat->id, errorMessage);
			});
	});

	AddCallbackQueryListener("vote", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1) {
			bot->getApi().answerCallbackQuery(query->id, "Comando no válido");
		}

		//Answer the query
		bot->getApi().answerCallbackQuery(query->id, "Enviando carta...");

		//Create player object
		Player player(query->from->id);

		//Create card object
		Card card(std::stol(parameters[0]));

		//Vote to delete the game
		gameLogic.VoteWhiteCard(player, card, query->message->messageId, 
			[&](Game &game, std::vector<Player> &playersThatVoted, RoundBlackCard &roundBlackCard){ //Success
				//Check if everyone have voted
				if ((game.GetType() == GameTypeEnum::GAME_DEMOCRACY && playersThatVoted.size() == game.GetNumberOfPlayers())
				|| (game.GetType() == GameTypeEnum::GAME_CLASSIC && playersThatVoted.size() == 1)
				|| (game.GetType() == GameTypeEnum::GAME_DICTATORSHIP && playersThatVoted.size() == 1)) {
					gameLogic.EndGameRound(game, card, 
						[&](Card &mostVotedCard, Player &mostVotedPlayer, std::vector<RoundWhiteCard> &votes) { //Success
							//Edit previous message to show ingame message
							bot->getApi().editMessageText(
								"¡Felicidades! "+mostVotedPlayer.GetName()+" ha ganado la carta negra de esta ronda '"+roundBlackCard.card.GetText()+"' con su carta:\n\n"
								"- "+mostVotedCard.GetText(), game.GetID(), roundBlackCard.message_id);

							//We iterate with players
							for (RoundWhiteCard vote : votes){
								//If it's the player who won
								if (vote.player_id == mostVotedPlayer.GetID()) {
									//Edit player's message
									bot->getApi().editMessageText(
										"¡Enhorabuena! Has ganado la carta negra de esta ronda '"+roundBlackCard.card.GetText()+"' con tu carta:\n\n"
										"- "+mostVotedCard.GetText(), vote.player_id, vote.message_id);
								} else {
									//Edit player's message
									bot->getApi().editMessageText(
										"¡Vaya! "+mostVotedPlayer.GetName()+" ha ganado la carta negra de esta ronda '"+roundBlackCard.card.GetText()+"' con su carta:\n\n"
										"- "+mostVotedCard.GetText(), vote.player_id, vote.message_id);
								}
							}

							//If player have won the game
							if (mostVotedPlayer.GetPoints() == game.GetNumberOfCardsToWin()) {
								//Send a message to the user
								bot->getApi().sendMessage(mostVotedPlayer.GetID(), 
									"¡¡¡Felicidades!!!\n\n"
									"¡¡¡Has ganado la partida!!!");

								//Send a message to the game
								bot->getApi().sendMessage(game.GetID(), 
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
							bot->getApi().sendMessage(query->message->chat->id, errorMessage);
						});
				} else {
					//Get playername string from vector
					std::string player_names = GetPlayerNamesFromPlayerArray(playersThatVoted, "", "\n");

					//Edit previous message to show ingame message
					bot->getApi().editMessageText(
						"La carta negra de esta ronda era:\n"+roundBlackCard.card.GetText()+"\n\n"
						"Hasta ahora han votado:\n"+player_names+"\n"
						"¡Seguid votando!", roundBlackCard.game_id, roundBlackCard.message_id);

					//Edit previous player message
					bot->getApi().editMessageText("Has votado la carta: "+card.GetText(), player.GetID(), query->message->messageId);
				}
			},
			[this, query](std::string errorMessage){ //Failure
				bot->getApi().sendMessage(query->message->chat->id, errorMessage);
			});
	});
	bot->getEvents().onCommand("newdictionary", [this](TgBot::Message::Ptr message) {
		bot->getApi().sendMessage(message->chat->id, 
			"La creacion de diccionarios se traslada al bot @cclhdictionariesbot, el cual estará desactivado unos dias.\n\n"
			"Lamento las molestias."
		);
	});
	bot->getEvents().onCommand("help", [this](TgBot::Message::Ptr message) {
		bot->getApi().sendMessage(message->chat->id, 
			"Bienvenido a la ayuda de "+configurationService.GetConfiguration("bot_name")+" versión "+configurationService.GetConfiguration("bot_version")+".\n"
			"Puedes consultar la ayuda en el siguiente enlace: http://telegra.ph/Manual-del-bot-Cartas-Contra-la-Humanidad-cclhbot-01-31\n"
			"Disfrutad del bot y... ¡A jugar!\n\n"
			"Creado por "+configurationService.GetConfiguration("bot_owner_alias")+"."
		);
	});
	bot->getEvents().onCommand("sendMessage", [this](TgBot::Message::Ptr message) {
		if (std::to_string(message->from->id) == configurationService.GetConfiguration("bot_owner_id")) {
			//Send message to me
			std::string msgText = Util::ReplaceAll(message->text, "/sendMessage ", "");
			bot->getApi().sendMessage(std::stol(configurationService.GetConfiguration("bot_owner_id")), "Enviando texto: "+msgText);
			//Send message to all users
			UserService userService;
			std::vector<User> users = userService.GetAllUsers();
			for (User user : users) {
				try {
					bot->getApi().sendMessage(user.GetID(), msgText);
				} catch (std::runtime_error &e) {
					user.SetActive(false);
					std::cerr << "Error (sendMessage): " << e.what() << std::endl;
				}
			}
		}
	});
}

void Bot::StartRound(Game &game) {
	//Send this round black card to the group	
	TgBot::Message::Ptr message = bot->getApi().sendMessage(game.GetID(), "Iniciando ronda...");

	//Start round
	gameLogic.StartGameRound(game, message->messageId, 
		[&](Card roundBlackCard, std::map< int64_t, std::vector<Card> > roundPlayerCards){ //Success
			//Send this round black card to the group	
			bot->getApi().editMessageText("La carta negra de esta ronda es: \n" + roundBlackCard.GetText(), game.GetID(), message->messageId);

			//Iterate players
			for (std::map< int64_t, std::vector<Card> >::iterator it = roundPlayerCards.begin(); it != roundPlayerCards.end(); ++it) {
				//If you are the president you don't pick cards
				if (it->first == game.GetPresidentID()) {
					//If it's classic we have to warn the current president.
					if (game.GetType() == GameTypeEnum::GAME_CLASSIC) {
						bot->getApi().sendMessage(it->first, "Eres el presidente de esta ronda. Cuando los demas jugadores hayan elegido sus cartas podrás votar.");
					}
				} else {
					//Create a markup keyboard
					TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
					for (Card card : it->second) {
						keyboard->inlineKeyboard.push_back(GetKeyboardRow(std::vector<TgButton> { TgButton(card.GetText(), "card_"+std::to_string(card.GetID())) }));
					}

					//Send a new message with the cards
					bot->getApi().sendMessage(it->first, "La carta negra de esta ronda es: \n" + roundBlackCard.GetText() + "\n\nTus cartas son: ", false, 0, keyboard);
				}
			}
		},
		[&](std::string errorMessage){ //Failure
			bot->getApi().sendMessage(game.GetID(), errorMessage);
		});
}

void Bot::DeleteGame(User &user, Game &game) {
	gameLogic.DeleteGame(user, game, 
		[&](std::vector< std::pair<int64_t, int64_t> > messageList){ //Success
			for (std::pair<int64_t, int64_t> messagePair : messageList) {
				//Edit previous message
				bot->getApi().editMessageText("Partida borrada.", messagePair.first, messagePair.second);
			}
		},
		[&](std::string errorMessage){ //Failure
			bot->getApi().sendMessage(game.GetID(), errorMessage);
		});
}

//////////////////////Telegram methods////////////////////////

void Bot::AddCallbackQueryListener(std::string query, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)> function) {
	queries.emplace(std::pair< std::string, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)> >(query, function));
}

std::string Bot::GetUsername(TgBot::User::Ptr user) {
	return user->firstName + " " + user->lastName + " (@"+user->username+")";
}

int64_t Bot::GetChatCreatorId(int64_t chat_id) {
	std::vector<TgBot::ChatMember::Ptr> admins = bot->getApi().getChatAdministrators(chat_id);
	for (int i = 0; i < admins.size(); i++) {
		if (admins[i]->status == "creator") return admins[i]->user->id;
	}

	throw UnexpectedException("No se ha encontrado al creador del grupo");
}

std::vector<int64_t> Bot::GetChatAdminsitratorIds(int64_t chat_id) {
	std::vector<TgBot::ChatMember::Ptr> admins = bot->getApi().getChatAdministrators(chat_id);
	std::vector<int64_t> admins_ids(admins.size());
	for (int i = 0; i < admins.size(); i++) {
		admins_ids.push_back(admins[i]->user->id);
	}

	return admins_ids;
}

std::vector<TgBot::InlineKeyboardButton::Ptr> Bot::GetKeyboardRow(const std::vector<TgButton>& buttons) {
	std::vector<TgBot::InlineKeyboardButton::Ptr> row;
	for (TgButton button : buttons) {
		TgBot::InlineKeyboardButton::Ptr checkButton(new TgBot::InlineKeyboardButton);
		checkButton->text = button.text;
		checkButton->callbackData = button.callback;
		row.push_back(checkButton);
	}

	return row;
}

std::string Bot::GetPlayerNamesFromPlayerArray(std::vector<Player>& players, std::string pre, std::string post) {
	std::string names = "";
	for (Player player : players) {
		names += pre + player.GetName() + post;
	}
	return names;
}

std::string Bot::GetCardsTextFromCardArray(std::vector<RoundWhiteCard>& cards, std::string pre, std::string post) {
	std::string texts = "";
	for (RoundWhiteCard card : cards) {
		texts += pre + card.card.GetText() + post;
	}
	return texts;
}