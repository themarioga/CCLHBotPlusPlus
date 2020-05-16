#pragma once

#include "../ApplicationException.h"

class CardNotExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No se ha encontrado la carta solicitada.";
		}
};