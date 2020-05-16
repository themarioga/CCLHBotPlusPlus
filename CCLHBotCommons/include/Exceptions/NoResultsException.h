#pragma once

#include "ApplicationException.h"

class NoResultsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Error inesperado: la consulta no ha devuelto ningun resultado.";
		}
};