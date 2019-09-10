#pragma once

#include "../ApplicationException.h"

class PlayerAlreadyVoteDeleteException : public ApplicationException
{
	public:
		virtual const char* what() const throw() {
			return "Ya has votado para borrar esta partida.";
		}
};