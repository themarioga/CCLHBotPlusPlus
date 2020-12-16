#include "Base/Bot.h"

#include <tgbot/tools/StringTools.h>

Bot::Bot(std::string token, std::string separator) {
	bot = new TgBot::Bot(token);
	this->separator = separator;
}

Bot::~Bot() {
	delete bot;
}

void Bot::ListenLongPoll() {
	while (true) {
		try {
			std::clog << "Iniciando bot con username: " << bot->getApi().getMe()->username.c_str() << std::endl;

			TgBot::TgLongPoll longPoll(*bot);

			std::clog << "Comenzando longPoll" << std::endl;
			while (true) {
				try {
					longPoll.start();
				} catch (std::runtime_error& e) {
					std::cerr << "["+Util::GetCurrentDatetime()+"] Error (listen): " << e.what() << std::endl;
				}
			}
		} catch (std::runtime_error& e) {
			std::cerr << "["+Util::GetCurrentDatetime()+"] Error (longPoll): " << e.what() << std::endl;
		}
	}
}

void Bot::ListenWebHook(std::string webhookURL, int port) {
	while (true) {
		try {
			std::clog << "Iniciando bot con username: " << bot->getApi().getMe()->username.c_str() << std::endl;

			TgBot::TgWebhookTcpServer webhook(port, *bot);
			bot->getApi().setWebhook(webhookURL);

			std::clog << "Escuchando peticiones webhook desde " << webhookURL << " en el puerto " << port << std::endl;
			while (true) {
				try {
					webhook.start();
				} catch (std::runtime_error& e) {
					std::cerr << "["+Util::GetCurrentDatetime()+"] Error (listen): " << e.what() << std::endl;
					return;
				}
			}
		} catch (std::runtime_error& e) {
			std::cerr << "["+Util::GetCurrentDatetime()+"] Error (webhook): " << e.what() << std::endl;
		}
	}
}

void Bot::StartMessageListener() {
	bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
		//If it's a response
		if (message->replyToMessage != nullptr) {
			//Check if the message is in the reply map
			std::map< std::string, std::function<void(TgBot::Message::Ptr)> >::iterator it;
			it = replies.find(GetMessageUniqueID(message->replyToMessage->chat->id, message->replyToMessage->messageId));
			if (it != replies.end()) {
				//Call the asociated function
				it->second(message);
			} else {
				//Show error message
				bot->getApi().sendMessage(message->from->id, "Error: Comando no válido.");
				std::cerr << "["+Util::GetCurrentDatetime()+"] Error: Comando no válido." << std::endl;
				return;
			}
		}
	});
}

void Bot::StartQueryListener() {
	bot->getEvents().onCallbackQuery([this](TgBot::CallbackQuery::Ptr query) {
		TgBot::Message::Ptr message = query->message;
		
		//Parse the query
		std::vector<std::string> result = StringTools::split(query->data, separator[0]);
		if (!result.size()) {
			bot->getApi().sendMessage(query->from->id, "Error: Comando no válido.");
			std::cerr << "["+Util::GetCurrentDatetime()+"] Error: Comando no válido." << std::endl;
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
			std::cerr << "["+Util::GetCurrentDatetime()+"] Error: Comando no válido." << std::endl;
			return;
		}
	});
}

//////////////////////Telegram methods////////////////////////

void Bot::AddCommandListener(const std::string& command, const TgBot::EventBroadcaster::MessageListener& callback) {
	bot->getEvents().onCommand(command, callback);
}

void Bot::AddCallbackQueryListener(const std::string& query, const std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)>& function) {
	queries.emplace(std::pair< std::string, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)> >(query, function));
}

void Bot::AddReplyListener(int64_t chat_id, int64_t message_id, const std::function<void(TgBot::Message::Ptr)>& function) {
	replies.emplace(std::pair< std::string, std::function<void(TgBot::Message::Ptr)> >(GetMessageUniqueID(chat_id, message_id), function));
}

void Bot::RemoveReplyListener(int64_t chat_id, int64_t message_id) {
	replies.erase(replies.find(GetMessageUniqueID(chat_id, message_id)));
}

TgBot::Message::Ptr Bot::sendMessage(int64_t chatId, const std::string& message, TgBot::GenericReply::Ptr replyMarkup) {
	return bot->getApi().sendMessage(chatId, message, false, 0, replyMarkup);
}

TgBot::Message::Ptr Bot::editMessage(int64_t chatId, int32_t messageId, const std::string& message, TgBot::GenericReply::Ptr replyMarkup) {
	return bot->getApi().editMessageText(message, chatId, messageId, "", "", false, replyMarkup);
}

void Bot::deleteMessage(int64_t chatId, int32_t messageId) {
	bot->getApi().deleteMessage(chatId, messageId);
}

bool Bot::answerCallbackQuery(const std::string& queryId, const std::string& message) {
	return bot->getApi().answerCallbackQuery(queryId, message);
}

int64_t Bot::GetChatCreatorId(int64_t chatId) {
	std::vector<TgBot::ChatMember::Ptr> admins = bot->getApi().getChatAdministrators(chatId);
	for (int i = 0; i < admins.size(); i++) {
		if (admins[i]->status == "creator") return admins[i]->user->id;
	}

	throw UnexpectedException("No se ha encontrado al creador del grupo");
}

std::string Bot::GetMessageUniqueID(int64_t chat_id, int64_t message_id) {
	return std::to_string(chat_id)+" "+std::to_string(message_id);
}

std::vector<int64_t> Bot::GetChatAdminsitratorIds(int64_t chatId) {
	std::vector<TgBot::ChatMember::Ptr> admins = bot->getApi().getChatAdministrators(chatId);
	std::vector<int64_t> admins_ids(admins.size());
	for (int i = 0; i < admins.size(); i++) {
		admins_ids.push_back(admins[i]->user->id);
	}

	return admins_ids;
}

std::string Bot::GetUsername(TgBot::User::Ptr user) {
	return user->firstName + " " + user->lastName + " (@"+user->username+")";
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
