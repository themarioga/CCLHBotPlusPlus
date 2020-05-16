#pragma once

#include "../ApplicationException.h"
#include "../../Enums/CardTypeEnum.h"

class DictionaryNotCompletedException : public ApplicationException
{
	public:
		DictionaryNotCompletedException(CardTypeEnum type, int32_t min_cards) {
			this->type = type;
			this->min_cards = min_cards;
		}

		virtual const char* what() const throw() {
			std::string text = (type == CardTypeEnum::CARD_BLACK ? "El diccionario seleccionado no tiene suficientes cartas negras." : "El diccionario seleccionado no tiene suficientes cartas blancas.");
			text += "\nNecesitas al menos "+std::to_string(min_cards)+" cartas de este tipo.";
			return text.c_str();
		}

	private:
		CardTypeEnum type;
		int32_t min_cards;
};