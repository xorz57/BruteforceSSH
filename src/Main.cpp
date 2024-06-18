#include <libssh/libssh.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool SSHConnect(const std::string &hostname, int port, const std::string &username, const std::string &password, int timeout) {
    ssh_session my_ssh_session = ssh_new();
    if (my_ssh_session == nullptr)
        return false;

    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, username.c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, hostname.c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_TIMEOUT, &timeout);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOSTKEYS, "ssh-ed25519");

    int rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK) {
        std::cerr << "Error connecting to " << hostname << ": " << ssh_get_error(my_ssh_session) << std::endl;
        ssh_free(my_ssh_session);
        return false;
    }

    rc = ssh_userauth_password(my_ssh_session, nullptr, password.c_str());
    if (rc == SSH_AUTH_SUCCESS) {
        std::cout << "Success: " << username << ":" << password << std::endl;
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return true;
    } else {
        std::cerr << "Failure: " << username << ":" << password << std::endl;
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
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

bool SSHConnectPasswords(const std::string &filename, const std::string &hostname, const std::string &username, int port, int timeout) {
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

int main() {
    SSHConnectAccounts("assets/accounts.txt", "127.0.0.1", 2222, 30);
    // SSHConnectPasswords("assets/passwords.txt", "127.0.0.1", "pi", 2222, 30);
    return 0;
}
