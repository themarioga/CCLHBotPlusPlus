#pragma once

#include <exception>

class NoResultsException : public std::exception
{
	public:
		virtual const char* what() const throw() {
			return "Error inesperado: la consulta no ha devuelto ningun resultado.";
		}
};