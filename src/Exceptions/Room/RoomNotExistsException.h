#pragma once

#include "../ApplicationException.h"

class RoomNotExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Esta sala aun no está registrada en el juego.";
		}
};