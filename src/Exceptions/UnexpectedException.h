#pragma once

#include <exception>

class UnexpectedException : public std::exception {
	public:
		/** Constructor (C strings).
		 *  @param message C-style string error message.
		 *                 The string contents are copied upon construction.
		 *                 Hence, responsibility for deleting the char* lies
		 *                 with the caller. 
		 */
		explicit UnexpectedException(const char* message):
		msg_("Error inesperado: \"" + std::string(message) + "\"")
		{
		}

		/** Constructor (C++ STL strings).
		 *  @param message The error message.
		 */
		explicit UnexpectedException(const std::string& message):
		msg_("Error inesperado: \"" + message + "\"")
		{}

		/** Destructor.
		 * Virtual to allow for subclassing.
		 */
		virtual ~UnexpectedException() throw () {}

		/** Returns a pointer to the (constant) error description.
		 *  @return A pointer to a const char*. The underlying memory
		 *          is in posession of the Exception object. Callers must
		 *          not attempt to free the memory.
		 */
		virtual const char* what() const throw () {
			return msg_.c_str();
		}

	protected:
		/** Error message.
		 */
		std::string msg_;
};