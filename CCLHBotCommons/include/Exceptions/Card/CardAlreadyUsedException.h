#pragma once

#include "../ApplicationException.h"

class CardAlreadyUsedException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Ya has usado una carta esta ronda.";
		}
};