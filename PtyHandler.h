#ifndef PTY_HANDLER_H
#define PTY_HANDLER_H
#include <queue>
#include <string>
#include <memory>
#include "Command.h"

class PtyHandler {

public:
	// Constructor
	PtyHandler();
	PtyHandler(const PtyHandler&) = delete;
	PtyHandler(PtyHandler&&) = default;
	// Destructor
	~PtyHandler();

	int getMasterFD() const;
	int getSlaveFD() const;
	pid_t getPid() const;
	const std::queue<std::unique_ptr<Command>>& getCommands() const;
	void addToQueue(const Command& commandToAdd);
	void typeNextCommand();
	void typeAll();

private:

	int m_masterFD;
	int m_slaveFD;
	pid_t m_pid;
	std::queue<std::unique_ptr<Command>> m_commands;
};
#endif
