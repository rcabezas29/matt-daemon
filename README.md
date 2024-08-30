# matt-daemon

`Matt_daemon` is a custom daemon process implemented in C++. It is designed to run as a background service with root privileges, handling network connections, logging activities, and ensuring only one instance runs at a time.

## Features

- **Daemon Process**: Runs in the background as a daemon, ensuring only one instance is active at any time.
- **Root Privileges Required**: The program must be executed with root rights.
- **Network Listener**: Listens on port `4242` for incoming connections.
- **Client Management**: Supports up to 3 simultaneous client connections. Attempts to connect more than 3 clients will result in the new connection being refused.
- **Signal Handling**: Intercepts system signals, logs them, and shuts down gracefully.
- **Logging**: All actions and events are logged with timestamps in the `/var/log/matt_daemon/matt_daemon.log` file.
- **Lock Mechanism**: Uses a lock file (`/var/lock/matt_daemon.lock`) to ensure only one instance of the daemon is running.
- **Client Interaction**: The daemon accepts commands from clients. Sending a "quit" command will shut down the daemon. All other commands are logged.
- **Tintin_reporter**: A logging utility class that handles log management and formatting.

## Installation

### Prerequisites

- C++ Compiler (e.g., `g++`)
- Root privileges

### Compilation

To compile the daemon, use the following command:

```bash
make 
```

### Running the Daemon

To start the daemon, execute:

```bash
sudo ./build/Matt_daemon
```

Ensure you run the daemon with sudo or as the root user since it requires root privileges.

### Stopping the Daemon

To stop the daemon, you can:

- Send the "quit" command from a connected client.
- Send a termination signal (e.g., SIGTERM or SIGINT) to the process.

### Logging

Logs are stored in:

```bash
/var/log/matt_daemon/matt_daemon.log
```

The log entries include timestamps in the format **[DD/MM/YYYY - HH:MM:SS]**.

### Lock File

To prevent multiple instances, the daemon creates a lock file at:

```bash
/var/lock/matt_daemon.lock
```

This file is deleted when the daemon shuts down.

### Usage Example

To connect to the daemon using netcat, open a terminal and run:

```bash
nc localhost 4242
```

You can then send commands to the daemon, such as:

- `quit` - Shuts down the daemon.
- Any other command will be logged.
