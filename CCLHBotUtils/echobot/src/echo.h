#pragma once

#include "Base/Bot.h"

class Echo {
    public:
        Echo(std::string);
        ~Echo();

        void Listen(bool webhooks);

    private:
        std::string botType;
        Bot *bot;
};