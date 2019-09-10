#pragma once

#include "../ApplicationException.h"

class DictionaryAlreadyExistsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Este nombre de diccionario ya esta registrado en el juego.";
		}
};