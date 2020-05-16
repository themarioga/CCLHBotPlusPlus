#pragma once

#include "../ApplicationException.h"

class DictionaryNotExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "El diccionario seleccionado no existe.";
		}
};