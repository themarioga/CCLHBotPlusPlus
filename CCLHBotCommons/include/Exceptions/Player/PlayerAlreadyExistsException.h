#pragma once

#include "../ApplicationException.h"

class PlayerAlreadyExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Ya estas jugando una partida.";
		}
};