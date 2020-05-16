#pragma once

#include "../ApplicationException.h"

class CardExcededLength : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "La carta es demasiado larga. No puede tener mas de 100 caracteres.";
		}
};