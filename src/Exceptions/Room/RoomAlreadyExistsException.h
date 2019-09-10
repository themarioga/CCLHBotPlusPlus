#pragma once

#include "../ApplicationException.h"

class RoomAlreadyExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Esta sala ya está registrada en el juego.";
		}
};