#pragma once

#include "../ApplicationException.h"

class UserNotActiveException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "El usuario solicitado ha bloqueado al bot.";
		}
};