#ifndef EVENT_EXCEPTIONS_H__
#define EVENT_EXCEPTIONS_H__

/**Common exceptions used by the EventAPI
 *
 * Code here uses the anthill namespace. IN the future, all AntHill code
 * should be placed inside this namespace.
 *
 * This code could be inside eventAPI.h but we kepp it separated just to
 * keep it less crowded.
 *
 */


#include <stdexcept>


namespace anthill {

/**Signals that the requested input handler was not found by the runtime.
 *
 * @see getInputHandler(), getOutputHandler()
 */
class HandlerNotFoundError : public std::runtime_error {
public:
	HandlerNotFoundError(const std::string& arg)
		: std::runtime_error(arg) {};
};


}; // namespace anthill


#endif // EVENT_EXCEPTIONS_H__
