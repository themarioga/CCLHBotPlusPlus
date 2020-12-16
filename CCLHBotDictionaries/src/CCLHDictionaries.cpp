#include "CCLHDictionaries.h"
#include "Services/CardService.h"
#include "Services/UserService.h"

#include <tgbot/tools/StringTools.h>

CCLHDictionaries::CCLHDictionaries() {

	//Init Bot
	bot = new Bot(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_token"), ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_query_separator"));

	//Start listening for queries
	bot->StartQueryListener();

	//Start listening for messages
	bot->StartMessageListener();

	//Set up bot
	SetUpCommands();
	SetUpQueries();
	
}

CCLHDictionaries::~CCLHDictionaries() {
	delete bot;
}

void CCLHDictionaries::Listen(bool webhooks) {
	if (webhooks) {
		bot->ListenWebHook(
			ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_webhook_url"),
			std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_webhook_port"))
		);
	} else {
    	bot->ListenLongPoll();
	}
}

void CCLHDictionaries::SetUpCommands() {
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
				bot->sendMessage(message->chat->id, "¡Hola "+user.GetName()+"!\n¿Que deseas hacer?", GetStartMenu());
			}
		} catch(ApplicationException& e) {
			//Register user in game
			BotLogic::RegisterUser(user, 
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
	bot->AddCommandListener("help", [this](TgBot::Message::Ptr message) {
		bot->sendMessage(message->chat->id, 
			"Bienvenido a la ayuda de "+ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_name")+" versión "+ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_version")+".\n"
			"Puedes consultar la ayuda en el siguiente enlace: http://telegra.ph/Manual-del-bot-Cartas-Contra-la-Humanidad-cclhbot-01-31\n"
			"Disfrutad del bot y... ¡A jugar!\n\n"
			"Creado por "+ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_owner_alias")+"."
		);
	});
	bot->AddCommandListener("sendMessage", [this](TgBot::Message::Ptr message) {
		if (std::to_string(message->from->id) == ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_owner_id")) {
			//Send message to me
			std::string msgText = Util::ReplaceAll(message->text, "/sendMessage ", "");
			bot->sendMessage(std::stol(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_owner_id")), "Enviando texto: "+msgText);
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
}

void CCLHDictionaries::SetUpQueries() {
	bot->AddCallbackQueryListener("create", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() != 1 || parameters[0] != "start") {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Iniciando creación de diccionario...");

		//Edit previous message
		TgBot::Message::Ptr response = bot->sendMessage(query->from->id, "Excelente, ¿que nombre quieres ponerle?", std::make_shared<TgBot::ForceReply>());

		//Listen for the reply
		bot->AddReplyListener(response->chat->id, response->messageId, [this, query](TgBot::Message::Ptr responseMsg){
			std::string dictionary_name = responseMsg->text;

			//Replace quotes
			std::replace(dictionary_name.begin(), dictionary_name.end(), '"', '\'');

			//Create the dictionary
			Dictionary dictionary(dictionary_name, responseMsg->chat->id);

			//Register it in the DB
			BotLogic::RegisterDictionary(dictionary, 
				[this, responseMsg](){ //Success
					bot->sendMessage(responseMsg->chat->id, 
					"De acuerdo, tu diccionario ser llamará: "+responseMsg->text+"\n"
					"Para añadir cartas accede a el desde el menú 'Editar'.",
					GetStartMenu());
				},
				[this, responseMsg](std::string errorMessage){ //Failure
					bot->sendMessage(responseMsg->chat->id, Bot::GetUsername(responseMsg->from)+": "+errorMessage);
				});

				//Remove listener
				bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
		});
	});

	bot->AddCallbackQueryListener("start", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Edit message
		bot->editMessage(query->from->id, query->message->messageId, "Hola, ¿que acción quieres hacer a continuación?", GetStartMenu());
	});

	bot->AddCallbackQueryListener("view", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() < 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Procesando...");

		if (parameters[0] == "start"){ //////////////SELECT THE DICTIONARY FOR EDIT////////////////
			GetDictionariesForView(query->from->id, query->message->messageId, 0);
		} else if (parameters[0] == "nav") { //////////////PAGINATION FOR THE DICTIONARY SELECTION////////////////
			GetDictionariesForView(query->from->id, query->message->messageId, std::stoi(parameters[1]));
		} else if (parameters[0] == "menu") { //////////////EDIT MENU////////////////
			try {
				Dictionary dictionary(std::stoi(parameters[1]));
				dictionary.Load();

				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, 
					"Estas viendo el diccionario '"+dictionary.GetName()+"', ¿que acción quieres hacer?", GetViewDictionaryMenu(std::stoi(parameters[1])));
			} catch(ApplicationException &e) {
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "Error: "+std::string(e.what()));
			} catch(UnexpectedException &e) {
				bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
			}
		} else if (parameters[0] == "info") {
			try {
				Dictionary dictionary(std::stoi(parameters[1]));
				dictionary.Load();

				User creator(dictionary.GetCreatorID());
				creator.Load();

				//Edit message
				bot->editMessage(query->from->id, query->message->messageId,
					"Nombre: "+dictionary.GetName()+"\n"
					"Privacidad: "+(dictionary.IsShared() ? "público" : "privado")+"\n"
					"Publicado: '"+(dictionary.IsPublished() ? "si" : "no")+"'\n"
					"Creado por: "+creator.GetName()+"\n"
					"¿que acción quieres hacer a continuación?", GetViewDictionaryMenu(std::stoi(parameters[1])));
			} catch(ApplicationException &e) {
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "Error: "+std::string(e.what()));
			} catch(UnexpectedException &e) {
				bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
			}
		} else if (parameters[0] == "cards") {
			if (parameters.size() == 2) {
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "Estas viendo las cartas, ¿que quieres hacer?", GetViewCardMenu(std::stoi(parameters[1])));
			} else if (parameters.size() == 3) {
				try {
					Dictionary dictionary(std::stoi(parameters[2]));
					dictionary.Load();

					//Borramos el mensaje previo
					bot->deleteMessage(query->from->id, query->message->messageId);

					if (parameters[1] == "white") {
						//Initial text
						std::string initial_text = "Estas son las cartas blancas de tu diccionario: \n";

						//Send cards
						SendCardList(CardTypeEnum::CARD_WHITE, dictionary, query->from->id, initial_text);
					} else if (parameters[1] == "black") {
						//Initial text
						std::string initial_text = "Estas son las cartas negras de tu diccionario: \n";

						//Send cards
						SendCardList(CardTypeEnum::CARD_BLACK, dictionary, query->from->id, initial_text);
					}

					//Send message
					bot->sendMessage(query->from->id, "Estas viendo las cartas, ¿que quieres hacer?", GetViewDictionaryMenu(std::stoi(parameters[2])));
				} catch(ApplicationException &e) {
					//Edit message
					bot->editMessage(query->from->id, query->message->messageId, "Error: "+std::string(e.what()));
				} catch(UnexpectedException &e) {
					bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
				}
			}
		} else if (parameters[0] == "collabs") {
			if (parameters.size() != 2) {
				bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
				return;
			}

			//Get collaborators
			std::vector<User> collaborators = DictionaryService::GetDictionaryCollaborators(std::stoi(parameters[1]));

			std::string text = "Estos son los colaboradores de este diccionario.\n";
			for (int i = 0; i < collaborators.size(); i++) {
				text += collaborators[i].GetName()+"\n";
			}
			text += "¿Que quieres hacer a continuación?";

			//Edit message
			bot->editMessage(query->from->id, query->message->messageId, text, GetViewDictionaryMenu(std::stoi(parameters[1])));
		}
	});

	bot->AddCallbackQueryListener("edit", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() < 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Procesando...");

		if (parameters[0] == "start"){ //////////////SELECT THE DICTIONARY FOR EDIT////////////////
			GetDictionariesForEdit(query->from->id, query->message->messageId, 0);
		} else if (parameters[0] == "nav") { //////////////PAGINATION FOR THE DICTIONARY SELECTION////////////////
			GetDictionariesForEdit(query->from->id, query->message->messageId, std::stoi(parameters[1]));
		} else if (parameters[0] == "menu") { //////////////EDIT MENU////////////////
			//Edit message
			bot->editMessage(query->from->id, query->message->messageId, "Estas editando el diccionario, ¿que acción quieres hacer?", GetEditDictionaryMenu(std::stoi(parameters[1])));
		} else if (parameters[0] == "chname") { //////////////EDIT NAME////////////////
			//Delete previous message
			bot->deleteMessage(query->from->id, query->message->messageId);

			//Send new message
			TgBot::Message::Ptr message = bot->sendMessage(query->from->id, "Excelente, ¿que nombre quieres ponerle?", std::make_shared<TgBot::ForceReply>());
			
			//Get dictionary id
			int64_t dictionary_id = std::stoi(parameters[1]);

			//Listen for the reply
			bot->AddReplyListener(message->chat->id, message->messageId, [this, message, dictionary_id](TgBot::Message::Ptr responseMsg){
				try {
					Dictionary dictionary(dictionary_id);
					dictionary.Load();

					dictionary.SetName(responseMsg->text);

					//Delete previous message
					bot->deleteMessage(message->chat->id, message->messageId);

					//Send message
					bot->sendMessage(responseMsg->chat->id, "¡Hecho! He modificado el nombre a '"+responseMsg->text+"'.\n¿Que acción quieres hacer ahora?", GetEditDictionaryMenu(dictionary_id));
				} catch (ApplicationException& e) {
					//Send message
					bot->sendMessage(responseMsg->chat->id, "Se ha producido un error. " + std::string(e.what()));
				} catch(UnexpectedException &e) {
					bot->sendMessage(responseMsg->chat->id, "Ha ocurrido un error. "+std::string(e.what())); 
				}

				//Remove listener
				bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
			});
		} else if (parameters[0] == "chpriv") { //////////////EDIT PRIVACY////////////////
			if (parameters[1] == "pub" || parameters[1] == "priv") {
				try {
					//Get dictionary id
					int64_t dictionary_id = std::stoi(parameters[2]);

					Dictionary dictionary(dictionary_id);
					dictionary.Load();

					if (parameters[1] == "pub") {
						dictionary.SetShared(true);
						dictionary.SetPublished(false);
					} else {
						dictionary.SetShared(false);
					}

					//Edit message
					bot->editMessage(query->from->id, query->message->messageId, "Recibido, he modificado la privacidad.\n¿Que acción quieres hacer ahora?", GetEditDictionaryMenu(dictionary_id));
				} catch (ApplicationException& e) {
					//Edit message
					bot->editMessage(query->from->id, query->message->messageId, "Se ha producido un error. " + std::string(e.what()));
				} catch(UnexpectedException &e) {
					bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
				}
			} else {
				//Create a markup keyboard
				TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Público (requiere aprobación)", "edit_chpriv_pub_"+parameters[1]) }));
				keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Privado", "edit_chpriv_priv_"+parameters[1]) }));
					
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "¡De acuerdo!\n\n¿Como quieres que sea?", keyboard);
			}
		} else if (parameters[0] == "cards") { //////////////EDIT CARDS////////////////
			if (parameters.size() == 2) {
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "Estas gestionando las cartas, ¿que quieres hacer?", GetEditCardMenu(std::stoi(parameters[1])));
			} else if (parameters.size() == 3) {
				try {
					Dictionary dictionary(std::stoi(parameters[2]));
					dictionary.Load();

					if (parameters[1] == "addwhite") { /////ADD WHITE CARD/////
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);
						
						//Send new message
						TgBot::Message::Ptr message = bot->sendMessage(query->from->id, 
						"Estas añadiendo una carta blanca, ahora escribe el texto.\n\n"
						"AVISO: Si quieres añadir mas de una carta a la vez pon un texto en cada linea.", 
						std::make_shared<TgBot::ForceReply>());
			
						//Add card process
						AddCardProcess(CardTypeEnum::CARD_WHITE, dictionary, message);
					} else if (parameters[1] == "addblack") { /////ADD BLACK CARD/////
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						//Send new message
						TgBot::Message::Ptr message = bot->sendMessage(query->from->id, 
						"Estas añadiendo una carta negra, ahora escribe el texto.\n\n"
						"AVISO: Si quieres añadir mas de una carta a la vez pon un texto en cada linea.", 
						std::make_shared<TgBot::ForceReply>());
			
						//Add card process
						AddCardProcess(CardTypeEnum::CARD_BLACK, dictionary, message);
					} else if (parameters[1] == "editwhite") { /////EDIT WHITE CARD/////
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						//Message text
						std::string text = "Estas son las cartas blancas del diccionario, para editar una escribe su numero.\n\n";

						//Send Message
						TgBot::Message::Ptr message = SendCardList(CardTypeEnum::CARD_WHITE, dictionary, query->from->id, text, true);

						//Edit card process
						EditCardProcess(dictionary, message);
					} else if (parameters[1] == "editblack") {
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						//Get cards
						std::string text = "Estas son las cartas negras del diccionario, para editar una escribe su numero.\n\n";

						//Send Message
						TgBot::Message::Ptr message = SendCardList(CardTypeEnum::CARD_BLACK, dictionary, query->from->id, text, true);

						//Edit card process
						EditCardProcess(dictionary, message);
					} else if (parameters[1] == "delwhite") {
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						//Get cards
						std::string text = "Estas son las cartas blancas del diccionario, para borrar una escribe su numero.\n\n";

						//Send Message
						TgBot::Message::Ptr message = SendCardList(CardTypeEnum::CARD_WHITE, dictionary, query->from->id, text, true);

						//Delete card process
						DeleteCardProcess(dictionary, message);
					} else if (parameters[1] == "delblack") {
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						//Get cards
						std::string text = "Estas son las cartas negras del diccionario, para borrar una escribe su numero.\n\n";

						//Send Message
						TgBot::Message::Ptr message = SendCardList(CardTypeEnum::CARD_BLACK, dictionary, query->from->id, text, true);

						//Delete card process
						DeleteCardProcess(dictionary, message);
					}
				} catch(ApplicationException &e) {
					bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
				} catch(UnexpectedException &e) {
					bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
				}
			}
		} else if (parameters[0] == "collabs") {
			if (parameters.size() == 2) {
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "Estas gestionando los colaboradores, ¿que quieres hacer?", GetEditCollaboratorsMenu(std::stoi(parameters[1])));
			} else if (parameters.size() == 3) {
				try {
					Dictionary dictionary(std::stoi(parameters[2]));
					dictionary.Load();

					if (parameters[1] == "add") {
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						//Get max collaborators
						int32_t max_collabs = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_max_collaborators"));

						//Get current collaborators
						int32_t current_collabs = DictionaryService::CountDictionaryCollaborators(dictionary.GetID());

						//Check if we exceeded the number
						if (current_collabs >= max_collabs) throw DictionaryMaxCollaboratorsReachedException();
						
						//Send new message
						TgBot::Message::Ptr message = bot->sendMessage(query->from->id, 
						"Estas añadiendo un colaborador, escribe su apodo (ya sabes, @loquesea) o su id de usuario (puede consultarlo en @userinfobot).", 
						std::make_shared<TgBot::ForceReply>());
			
						//Listen for the reply
						bot->AddReplyListener(message->chat->id, message->messageId, [this, message, dictionary](TgBot::Message::Ptr responseMsg){
							//Delete previous message
							bot->deleteMessage(responseMsg->chat->id, responseMsg->replyToMessage->messageId);

							//Get the user
							User user;
							if (Util::is_number(responseMsg->text)) {
								user = User(std::stol(responseMsg->text));
								user.Load();
							} else {
								user = UserService::GetUserByName(responseMsg->text);
							}

							//Add the collab
							DictionaryService::AddDictionaryCollaborator(user.GetID(), dictionary.GetID());

							//Create a markup keyboard
							TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
							keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Si", "collabs_accept_"+std::to_string(dictionary.GetID())) }));
							keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("No", "collabs_reject_"+std::to_string(dictionary.GetID())) }));
					
							//Send messages
							bot->sendMessage(responseMsg->chat->id, "Se ha enviado la solicitud de colaborador a '"+user.GetName()+"'. ¿Que deseas hacer a continuación?", GetEditDictionaryMenu(dictionary.GetID()));
							bot->sendMessage(user.GetID(), 
								"Te han invitado a colaborar con el diccionario '"+dictionary.GetName()+"'.\n"
								"¿Te parece bien?", keyboard);

							//Remove listener
							bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
						});
					} else if (parameters[1] == "edit") {
						//Create a markup keyboard
						TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Dar permisos de edición", "edit_collabs_allow_"+std::to_string(dictionary.GetID())) }));
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Retirar permisos de edición", "edit_collabs_deny_"+std::to_string(dictionary.GetID())) }));
						
						//Edit message
						bot->editMessage(query->from->id, query->message->messageId, "Estas editando los permisos de los colaboradores, ¿que quieres hacer?", keyboard);
					} else if (parameters[1] == "allow" || parameters[1] == "deny") {
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);

						std::string action = parameters[1];

						//Send new message
						TgBot::Message::Ptr message;
						if (action == "allow") {
							message = bot->sendMessage(query->from->id, 
								"Estas dando permisos de edición a un colaborador, escribe su apodo (ya sabes, @loquesea) o su id de usuario (puede consultarlo en @userinfobot).", 
								std::make_shared<TgBot::ForceReply>());
						} else if (action == "deny") {
							message = bot->sendMessage(query->from->id, 
								"Estas quitando permisos de edición a un colaborador, escribe su apodo (ya sabes, @loquesea) o su id de usuario (puede consultarlo en @userinfobot).", 
								std::make_shared<TgBot::ForceReply>());
						} else {
							bot->sendMessage(query->from->id, "Error inesperado");
							return;
						}
			
						//Listen for the reply
						bot->AddReplyListener(message->chat->id, message->messageId, [this, message, dictionary, action](TgBot::Message::Ptr responseMsg){
							//Delete previous message
							bot->deleteMessage(responseMsg->chat->id, responseMsg->replyToMessage->messageId);

							//Get the user
							User user;
							if (Util::is_number(responseMsg->text)) {
								user = User(std::stol(responseMsg->text));
								user.Load();
							} else {
								user = UserService::GetUserByName(responseMsg->text);
							}

							if (action == "allow") {
								DictionaryService::ModifyCollaboratorSetEditMode(user.GetID(), dictionary.GetID(), true);

								//Send messages
								bot->sendMessage(responseMsg->from->id, 
									"Has dado permisos de edición a '"+user.GetName()+"'.\n\n"
									"¿Que deseas hacer a continuación", GetEditDictionaryMenu(dictionary.GetID()));
								bot->sendMessage(user.GetID(), "Te han otorgado permisos de edición para el diccionario '"+dictionary.GetName()+"'");
							} else if (action == "deny") {
								DictionaryService::ModifyCollaboratorSetEditMode(user.GetID(), dictionary.GetID(), false);

								//Send messages
								bot->sendMessage(responseMsg->from->id, 
									"Has dado permisos de edición a '"+user.GetName()+"'.\n\n"
									"¿Que deseas hacer a continuación", GetEditDictionaryMenu(dictionary.GetID()));
								bot->sendMessage(user.GetID(), "Te han rescindido los permisos de edición para el diccionario '"+dictionary.GetName()+"'");
							}

							//Remove listener
							bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
						});
					} else if (parameters[1] == "del") {
						//Delete previous message
						bot->deleteMessage(query->from->id, query->message->messageId);
						
						//Send new message
						TgBot::Message::Ptr message = bot->sendMessage(query->from->id, 
						"Estas añadiendo un colaborador, escribe su apodo (ya sabes, @loquesea) o su id de usuario (puede consultarlo en @userinfobot).", 
						std::make_shared<TgBot::ForceReply>());

						//Listen for the reply
						bot->AddReplyListener(message->chat->id, message->messageId, [this, message, dictionary](TgBot::Message::Ptr responseMsg){
							//Delete previous message
							bot->deleteMessage(responseMsg->chat->id, responseMsg->replyToMessage->messageId);

							//Get the user
							User user;
							if (Util::is_number(responseMsg->text)) {
								user = User(std::stol(responseMsg->text));
								user.Load();
							} else {
								user = UserService::GetUserByName(responseMsg->text);
							}

							if (user.GetID() != dictionary.GetCreatorID()) {
								//Delete the collaborator
								DictionaryService::DeleteDictionaryCollaborator(user.GetID(), dictionary.GetID());
									
								//Send message
								bot->sendMessage(responseMsg->chat->id, "Se ha eliminado al colaborador '"+user.GetName()+"'. ¿Que deseas hacer a continuación?", GetEditDictionaryMenu(dictionary.GetID()));

								//Send message to the collab
								bot->sendMessage(user.GetID(), "Has sido eliminado como colaborador del diccionario '"+dictionary.GetName()+"'");
							} else {
								//Send error message
								bot->sendMessage(responseMsg->chat->id, "No se puede eliminar al creador del diccionario. ¿Que deseas hacer a continuación?", GetEditDictionaryMenu(dictionary.GetID()));
							}

							//Remove listener
							bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
						});
					}
				} catch(ApplicationException &e) {
					bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
				} catch(UnexpectedException &e) {
					bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
				}
			}
		} else if (parameters[0] == "publish") {
			try {
				Dictionary dictionary(std::stoi(parameters[1]));
				dictionary.Load();

				if (dictionary.IsPublished()) throw DictionaryAlreadyExistsException();

				int min_black_cards = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_min_blackcards"));
				int min_white_cards = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_min_whitecards"));

				if (CardService::CountCards(CardTypeEnum::CARD_BLACK, dictionary.GetID()) < min_black_cards) {
					throw DictionaryNotCompletedException(CardTypeEnum::CARD_BLACK, min_black_cards);
				}

				if (CardService::CountCards(CardTypeEnum::CARD_WHITE, dictionary.GetID()) < min_white_cards) {
					throw DictionaryNotCompletedException(CardTypeEnum::CARD_WHITE, min_white_cards);
				}

				if (!dictionary.IsShared()) {
					dictionary.SetPublished(true);

					//Edit message
					bot->editMessage(query->from->id, query->message->messageId, 
					"¡Enhorabuena!\nHas publicado el diccionario.\n\n"
					"¿Que quieres hacer ahora?", GetEditDictionaryMenu(dictionary.GetID()));
				} else {
					try {
						User user(query->from->id);
						user.Load();

						//Edit message
						bot->editMessage(query->from->id, query->message->messageId, 
						"¡Enhorabuena!\nTu diccionario ha sido enviado a revisión, cuando sea aprobado te llegará un mensaje.\n\n"
						"¿Que quieres hacer ahora?", GetEditDictionaryMenu(dictionary.GetID()));

						//Get the bot owner id
						int64_t owner_id = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_owner_id"));

						//Create a markup keyboard
						TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Aceptar", "publish_accept_"+parameters[1]) }));
						keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Rechazar", "publish_reject_"+parameters[1]) }));
					
						//Send message to the owner
						bot->sendMessage(owner_id, 
							"El usuario "+std::to_string(user.GetID())+" "+user.GetName()+" ha solicitado"
							" aprobar su diccionario '"+std::to_string(dictionary.GetID())+" "+dictionary.GetName()+"'", 
							keyboard);

						//Send message to the owner
						bot->sendMessage(owner_id, 
							"Este es el diccionario: ", 
							GetViewDictionaryMenu(dictionary.GetID()));
					} catch(ApplicationException &e) {
						bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
					} catch(UnexpectedException &e) {
						bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
					}
				}
			} catch(ApplicationException &e) {
				bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
			} catch(UnexpectedException &e) {
				bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
			}
		}
	});

	bot->AddCallbackQueryListener("delete", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() < 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Procesando...");

		if (parameters[0] == "start"){ //////////////SELECT THE DICTIONARY FOR EDIT////////////////
			GetDictionariesForDelete(query->from->id, query->message->messageId, 0);
		} else if (parameters[0] == "nav") { //////////////PAGINATION FOR THE DICTIONARY SELECTION////////////////
			GetDictionariesForDelete(query->from->id, query->message->messageId, std::stoi(parameters[1]));
		} else if (parameters[0] == "confirm") { //////////////EDIT MENU////////////////
			//Create a markup keyboard
			TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
			keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Si", "delete_accept_"+parameters[1]) }));
			keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("No", "start") }));
					
			//Edit message
			bot->editMessage(query->from->id, query->message->messageId, "Vas a borrar el diccionario, ¿estas seguro de que quieres borrarlo?", keyboard);
		} else if (parameters[0] == "accept") {
			try {
				Dictionary dictionary(std::stoi(parameters[1]));
				dictionary.Load();

				//Check if the dictionary is of that user property
				if (dictionary.GetCreatorID() != query->from->id) throw DictionaryNotExistsException();

				//Delete dictionary cards
				CardService::DeleteAllCards(dictionary.GetID());

				//Delete dictionary collaborators
				DictionaryService::DeleteAllDictionaryCollaborators(dictionary.GetID());

				//Delete the dictionary
				dictionary.Delete();
				
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, "Diccionario borrado.\n\n¿Que quieres hacer ahora?", GetStartMenu());
			} catch (ApplicationException &e) {
				//Edit message
				bot->editMessage(query->from->id, query->message->messageId, 
					"Error: "+std::string(e.what()));
			} catch(UnexpectedException &e) {
				bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
			}
		}
	});

	bot->AddCallbackQueryListener("publish", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() < 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Get the bot owner id
		int64_t owner_id = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_owner_id"));

		//Check if the user is the owner
		if (query->from->id != owner_id) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+":  Usuario no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Procesando...");

		try {
			Dictionary dictionary(std::stoi(parameters[1]));
			dictionary.Load();
		
			if (parameters[0] == "accept"){
				dictionary.SetPublished(true);

				bot->sendMessage(owner_id, "Diccionario publicado");
				bot->sendMessage(dictionary.GetCreatorID(), "¡Tu diccionario ha sido aprobado! Ahora todos pueden jugar con él.");
			} else if (parameters[0] == "reject") {
				bot->sendMessage(owner_id, "Diccionario rechazado");
				bot->sendMessage(dictionary.GetCreatorID(), 
				"Vaya, tu diccionario ha sido rechazado. Revisa que todas las cartas tengan sentido. "
				"Si tienes alguna duda pregunta a "+ConfigurationService::GetInstance()->GetConfiguration("dictionaries_bot_owner_id"));
			}
		} catch(ApplicationException &e) {
			//Edit message
			bot->editMessage(query->from->id, query->message->messageId, 
				"Error: "+std::string(e.what()));
		} catch(UnexpectedException &e) {
			bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
		}
	});

	bot->AddCallbackQueryListener("collabs", [this](std::vector<std::string> parameters, TgBot::CallbackQuery::Ptr query) {
		//Check if the query is correct
		if (parameters.size() < 1) {
			bot->answerCallbackQuery(query->id, Bot::GetUsername(query->from)+": Comando no válido");
			return;
		}

		//Answer the query
		bot->answerCallbackQuery(query->id, "Procesando...");

		try {
			//Load the dictionary
			Dictionary dictionary(std::stoi(parameters[1]));
			dictionary.Load();

			//Load the user
			User user(query->from->id);
			user.Load();

			if (parameters[0] == "accept") {
				//Set accepted to true
				DictionaryService::ModifyCollaboratorSetAccepted(user.GetID(), dictionary.GetID());

				//Send messages
				bot->editMessage(query->from->id, query->message->messageId, "Colaboración aceptada, ahora podrás gestionar y/o utilizar el diccionario.");
				bot->sendMessage(dictionary.GetCreatorID(), 
					"El usuario '"+user.GetName()+"' ha aceptado participar en el diccionario '"+dictionary.GetName()+"'");
			} else if (parameters[0] == "reject") {
				//Remove
				DictionaryService::DeleteDictionaryCollaborator(user.GetID(), dictionary.GetID());

				//Send messages
				bot->editMessage(query->from->id, query->message->messageId, "Colaboración rechazada");
				bot->sendMessage(dictionary.GetCreatorID(), 
					"El usuario '"+user.GetName()+"' ha rechazado participar en el diccionario '"+dictionary.GetName()+"'");
			}
		} catch(ApplicationException &e) {
			//Edit message
			bot->editMessage(query->from->id, query->message->messageId, 
				"Error: "+std::string(e.what()));
		} catch(UnexpectedException &e) {
			bot->sendMessage(query->from->id, "Ha ocurrido un error. "+std::string(e.what())); 
		}
	});

}

/////////////////////////Bot methods//////////////////////////
void CCLHDictionaries::GetDictionariesForView(int64_t user_id, int64_t message_id, int64_t offset) {
	//Get dictionaries by offset
	BotLogic::GetDictionaries(user_id, offset, [&](int64_t count, std::vector<Dictionary>& dictionaries){ //Success
		//Dictionaries per page
		int8_t dictionaries_per_page = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_per_page"));

		//Create a markup keyboard
		TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

		for (Dictionary dictionary : dictionaries) {
			keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
				TgButton(dictionary.GetName(), "view_menu_"+std::to_string(dictionary.GetID())) 
			}));	
		}

		std::vector<TgButton> button_row;
		if (offset > 0) button_row.push_back(TgButton("⬅️", "view_nav_"+std::to_string(offset-dictionaries_per_page)));
		if (count > offset + dictionaries.size()) button_row.push_back(TgButton("➡️", "view_nav_"+std::to_string(offset+dictionaries_per_page)));
		if (button_row.size() > 0) keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(button_row));
		
		//Edit previous message
		bot->editMessage(user_id, message_id, "De acuerdo! Elige el diccionario que quieres ver: ", keyboard);
	},
	[&](std::string errorMessage){ //Failure
		bot->sendMessage(user_id, errorMessage);
	});
}

void CCLHDictionaries::GetDictionariesForEdit(int64_t user_id, int64_t message_id, int64_t offset) {
	//Get dictionaries by offset
	BotLogic::GetDictionaries(user_id, offset, [&](int64_t count, std::vector<Dictionary>& dictionaries){ //Success
		//Dictionaries per page
		int8_t dictionaries_per_page = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_per_page"));

		//Create a markup keyboard
		TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

		for (Dictionary dictionary : dictionaries) {
			keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
				TgButton(dictionary.GetName(), "edit_menu_"+std::to_string(dictionary.GetID())) 
			}));	
		}

		std::vector<TgButton> button_row;
		if (offset > 0) button_row.push_back(TgButton("⬅️", "edit_nav_"+std::to_string(offset-dictionaries_per_page)));
		if (count > offset + dictionaries.size()) button_row.push_back(TgButton("➡️", "edit_nav_"+std::to_string(offset+dictionaries_per_page)));
		if (button_row.size() > 0) keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(button_row));
		
		//Edit previous message
		bot->editMessage(user_id, message_id, "De acuerdo! Elige el diccionario que quieres editar: ", keyboard);
	},
	[&](std::string errorMessage){ //Failure
		bot->sendMessage(user_id, errorMessage);
	});
}

void CCLHDictionaries::GetDictionariesForDelete(int64_t user_id, int64_t message_id, int64_t offset) {
	//Get dictionaries by offset
	BotLogic::GetDictionaries(user_id, offset, [&](int64_t count, std::vector<Dictionary>& dictionaries){ //Success
		//Dictionaries per page
		int8_t dictionaries_per_page = std::stoi(ConfigurationService::GetInstance()->GetConfiguration("dictionaries_per_page"));

		//Create a markup keyboard
		TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

		for (Dictionary dictionary : dictionaries) {
			keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { 
				TgButton(dictionary.GetName(), "delete_confirm_"+std::to_string(dictionary.GetID())) 
			}));	
		}

		std::vector<TgButton> button_row;
		if (offset > 0) button_row.push_back(TgButton("⬅️", "delete_nav_"+std::to_string(offset-dictionaries_per_page)));
		if (count > offset + dictionaries.size()) button_row.push_back(TgButton("➡️", "delete_nav_"+std::to_string(offset+dictionaries_per_page)));
		if (button_row.size() > 0) keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(button_row));
		
		//Edit previous message
		bot->editMessage(user_id, message_id, "De acuerdo! Elige el diccionario que quieres borrar: ", keyboard);
	},
	[&](std::string errorMessage){ //Failure
		bot->sendMessage(user_id, errorMessage);
	});
}

void CCLHDictionaries::AddCardProcess(CardTypeEnum type, Dictionary &dictionary, TgBot::Message::Ptr message) {
	//Listen for the reply
	bot->AddReplyListener(message->chat->id, message->messageId, [this, type, message, dictionary](TgBot::Message::Ptr responseMsg){
		std::stringstream ss(responseMsg->text);
		std::string text;
		int i = 0;
		while(std::getline(ss, text,'\n')){
			try {
				if (text.length() > 100) throw CardExcededLength();

				//Replace quotes
				std::replace(text.begin(), text.end(), '"', '\'');

				Card card(type, text, dictionary.GetID());
				card.Create();

				i++;
			} catch (ApplicationException& e) {
				//Send message
				bot->sendMessage(responseMsg->chat->id, "Se ha producido un error en la carta '"+text+"'. " + std::string(e.what()));
			} catch(UnexpectedException &e) {
				bot->sendMessage(responseMsg->chat->id, "Ha ocurrido un error. "+std::string(e.what())); 
			}
		}

		//Unpublish if it was already published
		Dictionary dictionary_aux(dictionary);
		dictionary_aux.SetPublished(false);

		//Send message
		bot->sendMessage(responseMsg->chat->id, 
			"Has añadido "+std::to_string(i)+" cartas "+(type == CardTypeEnum::CARD_WHITE ? "blancas" : "negras")+".\n\n"
			"AVISO: debes volver a publicar tu diccionario, para ello haz click en el boton 'Publicar' del menú de edición.\n\n"
			"¿Que quieres hacer a continuación?", 
			GetEditCardMenu(dictionary.GetID()));

		//Remove listener
		bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
	});
}

void CCLHDictionaries::EditCardProcess(Dictionary &dictionary, TgBot::Message::Ptr message){
	//Listen for the reply
	bot->AddReplyListener(message->chat->id, message->messageId, [this, dictionary](TgBot::Message::Ptr responseMsg){
		try {

			Card card(std::stoi(responseMsg->text));
			card.Load();

			if (card.GetDictionaryID() != dictionary.GetID()) throw CardNotExistsException();

			//Delete previous message
			bot->deleteMessage(responseMsg->chat->id, responseMsg->replyToMessage->messageId);

			//Send new message
			TgBot::Message::Ptr message2 = bot->sendMessage(responseMsg->chat->id, "Ahora escribe el nuevo texto para la carta.", std::make_shared<TgBot::ForceReply>());

			//Listen for the reply
			bot->AddReplyListener(message2->chat->id, message2->messageId, [this, card, dictionary](TgBot::Message::Ptr responseMsg2){
				try {
					if (responseMsg2->text.length() > 100) throw CardExcededLength();

					Card card2(card);

					card2.SetText(std::string(responseMsg2->text));

					//Unpublish if it was already published
					Dictionary dictionary_aux(dictionary);
					dictionary_aux.SetPublished(false);

					//Delete previous message
					bot->deleteMessage(responseMsg2->chat->id, responseMsg2->replyToMessage->messageId);

					//Send message
					bot->sendMessage(responseMsg2->chat->id, 
						"Has editado la carta "+std::to_string(card2.GetID())+" a '"+card2.GetText()+"'.\n\n"
						"AVISO: debes volver a publicar tu diccionario, para ello haz click en el boton 'Publicar' del menú de edición.\n\n"
						"¿Que quieres hacer a continuación?", GetEditCardMenu(dictionary.GetID()));
				} catch (ApplicationException& e) {
					//Send message
					bot->sendMessage(responseMsg2->chat->id, "Se ha producido un error en la carta '"+std::string(responseMsg2->text)+"'. " + std::string(e.what()));
				} catch(UnexpectedException &e) {
					bot->sendMessage(responseMsg2->chat->id, "Ha ocurrido un error. "+std::string(e.what())); 
				}

				//Remove listener
				bot->RemoveReplyListener(responseMsg2->chat->id, responseMsg2->replyToMessage->messageId);
			});
		} catch (ApplicationException& e) {
			//Send message
			bot->sendMessage(responseMsg->chat->id, "Se ha producido un error. " + std::string(e.what()));
		} catch(UnexpectedException &e) {
			bot->sendMessage(responseMsg->chat->id, "Ha ocurrido un error. "+std::string(e.what())); 
		}

		//Remove listener
		bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
	});
}

void CCLHDictionaries::DeleteCardProcess(Dictionary &dictionary, TgBot::Message::Ptr message) {
	//Listen for the reply
	bot->AddReplyListener(message->chat->id, message->messageId, [this, dictionary](TgBot::Message::Ptr responseMsg){
		try {

			Card card(std::stoi(responseMsg->text));
			card.Load();

			if (card.GetDictionaryID() != dictionary.GetID()) throw CardNotExistsException();

			//Delete previous message
			bot->deleteMessage(responseMsg->chat->id, responseMsg->replyToMessage->messageId);

			//Delete the card
			card.Delete();

			//Unpublish if it was already published
			Dictionary dictionary_aux(dictionary);
			dictionary_aux.SetPublished(false);

			//Send message
			bot->sendMessage(responseMsg->chat->id, 
				"Has borrado la carta "+responseMsg->text+".\n\n"
				"AVISO: debes volver a publicar tu diccionario, para ello haz click en el boton 'Publicar' del menú de edición.\n\n"
				"¿Que quieres hacer a continuación?", GetEditCardMenu(dictionary.GetID()));
		} catch (ApplicationException& e) {
			//Send message
			bot->sendMessage(responseMsg->chat->id, "Se ha producido un error. " + std::string(e.what()));
		} catch(UnexpectedException &e) {
			bot->sendMessage(responseMsg->chat->id, "Ha ocurrido un error. "+std::string(e.what())); 
		}

		//Remove listener
		bot->RemoveReplyListener(responseMsg->chat->id, responseMsg->replyToMessage->messageId);
	});
}

TgBot::Message::Ptr CCLHDictionaries::SendCardList(CardTypeEnum type, Dictionary& dictionary, int64_t chat_id, std::string initial_text, bool force_reply) {
	//Get cards
	std::vector<Card> cards = CardService::GetAllCards(type, dictionary.GetID());
	std::string text = initial_text;
	for (int i = 0; i < cards.size(); i++) {
		text += std::to_string(cards[i].GetID())+". "+cards[i].GetText()+"\n";

		if (text.length() > 4000) {
			//Send new message
			bot->sendMessage(chat_id, text);

			//Reset the message
			text = "";
		}
	}

	//Send new message
	if (force_reply) return bot->sendMessage(chat_id, text, std::make_shared<TgBot::ForceReply>());
	else return bot->sendMessage(chat_id, text);
}

TgBot::InlineKeyboardMarkup::Ptr CCLHDictionaries::GetStartMenu() {
	//Create a markup keyboard
	TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Crear diccionario", "create_start") }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Ver diccionario", "view_start") }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Editar diccionario", "edit_start") }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar diccionario", "delete_start") }));

	return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr CCLHDictionaries::GetViewDictionaryMenu(int64_t dictionary_id) {
	//Create a markup keyboard
	TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Ver información general", "view_info_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Ver cartas", "view_cards_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Ver colaboradores", "view_collabs_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Volver", "start") }));

	return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr CCLHDictionaries::GetEditDictionaryMenu(int64_t dictionary_id) {
	//Create a markup keyboard
	TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Cambiar nombre", "edit_chname_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Cambiar privacidad", "edit_chpriv_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Gestionar cartas", "edit_cards_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Gestionar colaboradores", "edit_collabs_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Publicar diccionario", "edit_publish_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Volver", "start") }));

	return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr CCLHDictionaries::GetViewCardMenu(int64_t dictionary_id) {
	//Create a markup keyboard
	TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Ver blancas", "view_cards_white_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Ver negras", "view_cards_black_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Volver", "view_menu_"+std::to_string(dictionary_id)) }));

	return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr CCLHDictionaries::GetEditCardMenu(int64_t dictionary_id) {
	//Create a markup keyboard
	TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Añadir blanca", "edit_cards_addwhite_"+std::to_string(dictionary_id)), TgButton("Añadir negra", "edit_cards_addblack_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Editar blanca", "edit_cards_editwhite_"+std::to_string(dictionary_id)), TgButton("Editar negra", "edit_cards_editblack_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar blanca", "edit_cards_delwhite_"+std::to_string(dictionary_id)), TgButton("Borrar negra", "edit_cards_delblack_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Volver", "edit_menu_"+std::to_string(dictionary_id)) }));

	return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr CCLHDictionaries::GetEditCollaboratorsMenu(int64_t dictionary_id) {
	//Create a markup keyboard
	TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Añadir colaborador", "edit_collabs_add_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Editar colaborador", "edit_collabs_edit_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Borrar colaborador", "edit_collabs_del_"+std::to_string(dictionary_id)) }));
	keyboard->inlineKeyboard.push_back(Bot::GetKeyboardRow(std::vector<TgButton> { TgButton("Volver", "edit_menu_"+std::to_string(dictionary_id)) }));

	return keyboard;
}