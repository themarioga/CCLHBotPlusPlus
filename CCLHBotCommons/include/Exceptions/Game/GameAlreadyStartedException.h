#pragma once

#include "../ApplicationException.h"

class GameAlreadyStartedException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No puedes unirte a esta partida por que ya esta iniciada.";
		}
};