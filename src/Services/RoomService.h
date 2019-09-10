#pragma once

#include "../Models/Room.h"

class RoomService {

	public:
		
		RoomService();
		~RoomService();

		/**
		 * @brief Get All Rooms
		 * 
		 * @return std::vector<Room> vector of rooms
		 */
		std::vector<Room> GetAllRooms();

	private:

};