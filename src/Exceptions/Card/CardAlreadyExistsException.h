#pragma once

#include "../ApplicationException.h"

class CardAlreadyExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "La carta que intentas guardar ya existe.";
		}
};