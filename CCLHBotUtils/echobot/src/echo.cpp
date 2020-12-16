#include "echo.h"

Echo::Echo(std::string type) {
	this->botType = type;

	//Init Bot
	bot = new Bot(ConfigurationService::GetInstance()->GetConfiguration(type+"_bot_token"), ConfigurationService::GetInstance()->GetConfiguration(type+"_bot_query_separator"));

	//Start listening for queries
	bot->StartQueryListener();
}

Echo::~Echo() {
	delete bot;
}

void Echo::Listen(bool webhooks) {
	if (webhooks) {
		bot->ListenWebHook(
			ConfigurationService::GetInstance()->GetConfiguration(botType+"_bot_webhook_url"),
			std::stoi(ConfigurationService::GetInstance()->GetConfiguration(botType+"_bot_webhook_port"))
		);
	} else {
    	bot->ListenLongPoll();
	}
}