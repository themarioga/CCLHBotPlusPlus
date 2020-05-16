#pragma once

#include "../ApplicationException.h"

class PlayerNotExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No estas jugando ninguna partida.";
		}
};