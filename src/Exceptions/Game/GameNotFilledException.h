#pragma once

#include "../ApplicationException.h"

class GameNotFilledException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No puedes iniciar la partida por que aun se ha llenado.";
		}
};