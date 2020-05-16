#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <tgbot/tgbot.h>

#include "Util.hpp"
#include "../Services/ConfigurationService.h"
#include "../Exceptions/UnexpectedException.h"

struct TgButton {
	std::string text;
	std::string callback;

	TgButton(std::string text, std::string callback) {
		this->text = text;
		this->callback = callback;
	}
};

class Bot {

	public:

		Bot(std::string, std::string);
		~Bot();
		
		void Start();
		void Listen();

		void StartMessageListener();
		void StartQueryListener();

		//Bot methods
		void AddCommandListener(const std::string&, const TgBot::EventBroadcaster::MessageListener&);
		void AddCallbackQueryListener(const std::string&, const std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)>&);
		void AddReplyListener(int64_t chat_id, int64_t message_id, const std::function<void(TgBot::Message::Ptr)>&);
		void RemoveReplyListener(int64_t chat_id, int64_t message_id);

		TgBot::Message::Ptr sendMessage(int64_t, const std::string&, TgBot::GenericReply::Ptr = std::make_shared<TgBot::GenericReply>());
		TgBot::Message::Ptr editMessage(int64_t, int32_t, const std::string&, TgBot::GenericReply::Ptr = std::make_shared<TgBot::GenericReply>());
		void deleteMessage(int64_t, int32_t);
		bool answerCallbackQuery(const std::string&, const std::string& = "");

		int64_t GetChatCreatorId(int64_t);
		std::vector<int64_t> GetChatAdminsitratorIds(int64_t);

		static std::string GetUsername(TgBot::User::Ptr);
		static std::vector<TgBot::InlineKeyboardButton::Ptr> GetKeyboardRow(const std::vector<TgButton>&);
		static std::string GetMessageUniqueID(int64_t, int64_t);

	private:

		TgBot::Bot *bot;

		std::map< std::string, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)> > queries;
		std::map< std::string, std::function<void(TgBot::Message::Ptr)> > replies;

		std::string separator;
};