#pragma once

#include "Base/Bot.h"
#include "GameLogic.h"

class CCLH {
    public:
        CCLH();
        ~CCLH();

        void Listen();

    private:
        Bot *bot;

        //Core methods
		void SetUpCommands();
        void SetUpQueries();

		//Game methods
		void StartRound(Game&);
		void DeleteGame(User&, Game&);
		void GetDictionaries(int8_t, Game&);
};