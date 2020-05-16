#pragma once

#include "../ApplicationException.h"

class DictionaryMaxCollaboratorsReachedException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Se ha alcanzado el numero máximo de colaboradores.";
		}
};