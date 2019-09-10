#pragma once

#include "../ApplicationException.h"

class PlayerNotEnoughPermissionsException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "No tienes permiso para realizar esta acción.";
		}
};