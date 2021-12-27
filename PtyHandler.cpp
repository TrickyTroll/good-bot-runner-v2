#include <pty.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <regex>
#include <stdexcept>

#include "PtyHandler.h"
#include "Command.h"

PtyHandler::PtyHandler() {

    m_masterFD = 0;
    m_slaveFD = 0;
    // Create the pseudo terminals
    openpty(&m_masterFD, &m_slaveFD, NULL, NULL, NULL);
    m_pid = fork();
    if (m_pid < 0) {
        close(m_masterFD);
        close(m_slaveFD);
        throw std::runtime_error("Cannot start child process.");
    }
}

PtyHandler::~PtyHandler() {
    close(m_masterFD);
    close(m_slaveFD);
}

int PtyHandler::getMasterFD() const {
    return m_masterFD;
}

int PtyHandler::getSlaveFD() const {
    return m_slaveFD;
}

pid_t PtyHandler::getPid() const {
    return m_pid;
}

const std::queue<std::unique_ptr<Command>>& PtyHandler::getCommands() const {
    return m_commands;
}

void PtyHandler::addToQueue(const Command& commandToAdd) {
    m_commands.push(std::make_unique<Command> (commandToAdd));
}

void PtyHandler::typeNextCommand() {

    std::string currentCommand{ m_commands.front()->getCommand() };
    std::string currentExpect{ m_commands.front()->getExpect() };
    m_commands.pop();

    char previousCharacter{currentCommand[0]};
	fd_set rfds;
    fd_set wfds;
    // timeout is a timeout for select:
	struct timeval timeout { 0, 0 };
	char buf[4097];
	ssize_t size;

    for (char character : currentCommand) {
        // Send the character to the child process
        // This is where the delay should be added.
        char buffer[1]{character};

		FD_ZERO(&wfds); // Clear the set
		FD_SET(m_masterFD, &wfds); // Remove file descriptor from set
		if (select(m_masterFD + 1, NULL, &wfds, NULL, &timeout)) {
			write(m_masterFD, buffer, 1);
		}
        previousCharacter = character;
    }

    // Adding a new line

	char buffer[1]{'\n'};

	FD_ZERO(&wfds);
	FD_SET(m_masterFD, &wfds);
	if (select(m_masterFD + 1, NULL, &wfds, NULL, &timeout)) {
		write(m_masterFD, buffer, 1);
	}

    // Read until what we expect
    // Using a regular expression to match
    char currentChar[1]{};
    std::string lineBuf{}; // A line is maximum 4096 chars for our purpose

    while (1) {
        // Reading until match
        FD_ZERO(&rfds);
        FD_SET(m_masterFD, &rfds);
        // Using select so often might be expensive...
        if (select(m_masterFD + 1, &rfds, NULL, NULL, &timeout)) {
            size = read(m_masterFD, currentChar, 1);
            if (currentChar[0] == '\n') {
                // matching 
                std::regex toExpectRegex(currentExpect, std::regex_constants::ECMAScript | std::regex_constants::icase);
                if (std::regex_search(lineBuf, toExpectRegex)) {
                    // We have found the line, done waiting.
                    break;
                }
                // clean string
                lineBuf.clear();
            }
            else {
                lineBuf.push_back(currentChar[0]);
            }
        }
    }

    // Read what is left after the child is done.
    // This could probably be replaced by flushing what is left
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(m_masterFD, &rfds);
        if (!select(m_masterFD + 1, &rfds, NULL, NULL, &timeout)) {
            // No more to read.
            break;
        }
        size = read(m_masterFD, buf, 4096);
        buf[size] = '\0';
    }
}

void PtyHandler::typeAll() {
    for (int i{ 0 }; i < m_commands.size(); i++) {
        typeNextCommand();
    }
}

