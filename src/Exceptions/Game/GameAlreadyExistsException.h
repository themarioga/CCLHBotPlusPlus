#pragma once

#include "../ApplicationException.h"

class GameAlreadyExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Esta sala ya tiene una partida en marcha.";
		}
};