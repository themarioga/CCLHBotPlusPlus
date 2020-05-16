#pragma once

#include "Card.h"
#include "User.h"
#include "../Exceptions/Player/PlayerAlreadyExistsException.h"
#include "../Exceptions/Player/PlayerAlreadyVoteDeleteException.h"
#include "../Exceptions/Player/PlayerNotExistsException.h"
#include "../Exceptions/Player/PlayerNotEnoughPermissionsException.h"
#include "../Exceptions/UnexpectedException.h"

class Player : public User {

	public:
		//Constructor
		Player();
		Player(int64_t);
		Player(int64_t, std::string, int64_t, int64_t, int32_t);

		//Autofill constructor
		Player(int64_t, std::string, uint8_t, int64_t, int64_t, int32_t, int32_t);

		//Destructor
		~Player();

		//Basic methods
		void Load();
		void Create();
		void Delete();

		//Setters
		void SetMessageID(int64_t);
		void SetJoinOrder(int32_t);
		void SetPoints(int32_t);

		//Getters
		int64_t GetGameID() const { return game_id; }
		int64_t GetMessageID() const { return message_id; }
		int32_t GetJoinOrder() const { return join_order; }
		int32_t GetPoints() const { return points; }

	private:
		int64_t game_id;
		int64_t message_id;
		int32_t join_order;
		int32_t points;

};