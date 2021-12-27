#ifndef COMMAND_H
#define COMMAND_H

#include <string>

/**
* @brief A class to contain commands that will be sent to a PTY.
*/
class Command {

public:
	// Constructor
	Command(const std::string& command, const std::string& expect);
	// Required for the queue to work
	Command() = delete;
	// No copies allowed
	Command(const Command&) = delete;
	Command(Command&&) = default;

	// Getters
	std::string getCommand() const;
	std::string getExpect() const;

private:
	std::string m_command;
	std::string m_expect;
};

#endif // !COMMAND_H
