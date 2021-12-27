#include <iostream>
#include <pty.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

void openpty_demo(const char* output_size) {
    // File descriptors
    int master{};
    int slave{};
    // Create the pseudo terminals
    openpty(&master, &slave, NULL, NULL, NULL);

    // Lines commented to that we can see the output
//    int _stdout = dup(STDOUT_FILENO);
//    dup2(slave, STDOUT_FILENO);

    pid_t pid = fork();
    if (pid == 0) {
        // We use
        //
        //     head -c $output_size /dev/zero
        //
        // as the command for our demo.
        const char* argv[] = { "ls", NULL };
        // Execute the command
        execvp(argv[0], const_cast<char* const*>(argv));
    }

    fd_set rfds;
    struct timeval tv { 0, 0 };
    char buf[4097];
    ssize_t size;
    size_t count = 0;

    // Read from master as we wait for the child process to exit.
    //
    // We don't wait for it to exit and then read at once, because otherwise the
    // command being executed could potentially saturate the slave's buffer and
    // stall.
    while (1) {
        // If the process id is dead
        if (waitpid(pid, NULL, WNOHANG) == pid) {
            break;
        }
        // else
        FD_ZERO(&rfds);
        FD_SET(master, &rfds);
        // Wait until file descriptor is ready
        if (select(master + 1, &rfds, NULL, NULL, &tv)) {
            // Once master is ready:
            // Read 4096 bytes from master and write to buf
            size = read(master, buf, 4096);
            // Terminate buf
            buf[size] = '\0';
            // increment count to how many bytes were read
            count += size;
        }
    }

    // No need for the next line since we are using the 
    // Child process terminated; we flush the output and restore stdout.
//    fsync(STDOUT_FILENO);
//    dup2(_stdout, STDOUT_FILENO);

    // Read until there's nothing to read, by which time we must have read
    // everything because the child is long gone.
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(master, &rfds);
        if (!select(master + 1, &rfds, NULL, NULL, &tv)) {
            // No more to read.
            break;
        }
        size = read(master, buf, 4096);
        buf[size] = '\0';
        count += size;
    }

    // Close both ends of the pty.
    close(master);
    close(slave);

    std::cout << "Total characters read: " << count << std::endl;
}

int main(int argc, const char* argv[]) {
    openpty_demo(argv[1]);
}
