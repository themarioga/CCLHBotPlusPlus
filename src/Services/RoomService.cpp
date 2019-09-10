#include "RoomService.h"
#include "../Db.h"

RoomService::RoomService() {

}

RoomService::~RoomService() {

}

std::vector<Room> RoomService::GetAllRooms() {
	try {
		std::vector<Room> rooms;

		//Query
		std::shared_ptr<SQLite::Statement> query = Db::Instance()->CreateQuery("SELECT id, name, owner_id, active FROM rooms");
		while (query->executeStep()) {
			Room room = query->getColumns<Room, 4>();
			rooms.push_back(room);
		}
		
		return rooms;
	} catch (SQLite::Exception& e) {
		throw UnexpectedException(e.what());
	} catch (std::runtime_error& e) {
		throw UnexpectedException(e.what());
	}
}