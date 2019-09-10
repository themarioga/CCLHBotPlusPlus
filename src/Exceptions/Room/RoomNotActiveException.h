#pragma once

#include "../ApplicationException.h"

class RoomNotActiveException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "El bot ha sido expulsado de la sala solicitada.";
		}
};