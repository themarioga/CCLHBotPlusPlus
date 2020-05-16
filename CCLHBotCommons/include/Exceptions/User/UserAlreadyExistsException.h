#pragma once

#include "../ApplicationException.h"

class UserAlreadyExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Ya estas registrado en el juego.";
		}
};