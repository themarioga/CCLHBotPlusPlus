#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <tgbot/tgbot.h>

#include "BotLogic.h"

#include "Base/Bot.h"
#include "Base/Util.hpp"
#include "Services/ConfigurationService.h"

class CCLHDictionaries {

	public:

		CCLHDictionaries();
		~CCLHDictionaries();
		
		void Listen(bool webhooks);

	private:

		Bot *bot;

		//Bot methods
		void SetUpCommands();
		void SetUpQueries();

		//CCLHDictionaries methods
		void GetDictionariesForView(int64_t, int64_t, int64_t);
		void GetDictionariesForEdit(int64_t, int64_t, int64_t);
		void GetDictionariesForDelete(int64_t, int64_t, int64_t);
		void AddCardProcess(CardTypeEnum, Dictionary&, TgBot::Message::Ptr);
		void EditCardProcess(Dictionary&, TgBot::Message::Ptr);
		void DeleteCardProcess(Dictionary&, TgBot::Message::Ptr);
		TgBot::Message::Ptr SendCardList(CardTypeEnum, Dictionary&, int64_t, std::string, bool = false);

		//Static methods
		static TgBot::InlineKeyboardMarkup::Ptr GetStartMenu();
		static TgBot::InlineKeyboardMarkup::Ptr GetViewDictionaryMenu(int64_t);
		static TgBot::InlineKeyboardMarkup::Ptr GetEditDictionaryMenu(int64_t);
		static TgBot::InlineKeyboardMarkup::Ptr GetViewCardMenu(int64_t);
		static TgBot::InlineKeyboardMarkup::Ptr GetEditCardMenu(int64_t);
		static TgBot::InlineKeyboardMarkup::Ptr GetEditCollaboratorsMenu(int64_t);
};