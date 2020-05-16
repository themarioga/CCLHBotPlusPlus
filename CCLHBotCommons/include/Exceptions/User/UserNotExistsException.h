#pragma once

#include "../ApplicationException.h"

class UserNotExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Aun no te has registrado en el juego.";
		}
};