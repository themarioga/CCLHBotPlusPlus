#pragma once

#include "../ApplicationException.h"

class GameNotConfiguredException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No puedes unirte a la partida por que aun no está configurada.";
		}
};