#include "Command.h"
#include <string>

Command::Command(const std::string& command, const std::string& expect)
: m_command(command), m_expect(expect) {}

std::string Command::getCommand() const {
	return m_command;
}

std::string Command::getExpect() const {
	return m_expect;
}