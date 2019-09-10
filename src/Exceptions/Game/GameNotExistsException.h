#pragma once

#include "../ApplicationException.h"

class GameNotExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No hay ninguna partida iniciada en esta sala.";
		}
};