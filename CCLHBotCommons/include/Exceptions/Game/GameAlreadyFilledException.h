#pragma once

#include "../ApplicationException.h"

class GameAlreadyFilledException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No puedes unirte a esta partida por que ya se ha llenado.";
		}
};