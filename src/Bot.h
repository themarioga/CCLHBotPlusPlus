#pragma once

#include <string>
#include <vector>
#include <functional>
#include <tgbot/tgbot.h>

#include "Util.hpp"
#include "GameLogic.h"

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

		Bot();
		~Bot();
		
		void Start();
		void Listen();

		//Bot methods
		void StartQueryListener();
		void SetUpCommands();

	private:

		TgBot::Bot *bot;

		ConfigurationService configurationService;
		GameLogic gameLogic;

		std::map< std::string, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)> > queries;

		//Game methods
		void StartRound(Game&);
		void DeleteGame(User&, Game&);

		//Telegram methods
		void AddCallbackQueryListener(std::string, std::function<void(std::vector<std::string>, TgBot::CallbackQuery::Ptr)>);
		int64_t GetChatCreatorId(int64_t);
		std::vector<int64_t> GetChatAdminsitratorIds(int64_t);
		std::string GetUsername(TgBot::User::Ptr user);
		std::vector<TgBot::InlineKeyboardButton::Ptr> GetKeyboardRow(const std::vector<TgButton>&);
		std::string GetPlayerNamesFromPlayerArray(std::vector<Player>&, std::string="", std::string="");
		std::string GetCardsTextFromCardArray(std::vector<RoundWhiteCard>&, std::string="", std::string="");
};