# BruteforceSSH

[![Build](https://github.com/xorz57/BruteforceSSH/actions/workflows/Build.yml/badge.svg)](https://github.com/xorz57/BruteforceSSH/actions/workflows/Build.yml)

## Usage

```console
Usage: BruteforceSSH [options]
Options:
  -f, --file <filename>      File
  -u, --user <username>      User (optional)
  -h, --host <hostname>      Host (default: 127.0.0.1)
  -p, --port <port>          Port (default: 22)
  -t, --timeout <timeout>    Timeout (seconds, default: 30)
  --help                     Print help
```

## Examples

```console
./BruteforceSSH --file accounts.txt --host localhost --port 2222
Failure: pi:root
Failure: pi:toor
Failure: pi:1234
Success: pi:raspberry
```

```console
./BruteforceSSH --file passwords.txt --host localhost --port 2222 --user pi
Failure: pi:root
Failure: pi:toor
Failure: pi:1234
Success: pi:raspberry
```

## Testing

Ensure you have Docker and Docker Compose installed on your machine.

1. Clone the repository:

    ```sh
    git clone https://github.com/xorz57/BruteforceSSH.git
    cd BruteforceSSH
    ```

2. Start the OpenSSH server using Docker Compose:

    ```sh
    docker-compose up -d
    ```

3. The OpenSSH server will be available on port `2222` of your localhost.

### Credentials

- **Username**: `pi`
- **Password**: `raspberry`

## Dependencies

BruteforceSSH relies on the following libraries:

- [libssh](https://github.com/canonical/libssh)

## How to Build

#### Linux & macOS

```bash
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh

git clone https://github.com/xorz57/BruteforceSSH.git
cd BruteforceSSH
cmake -B build -DCMAKE_BUILD_TYPE=Release -S . -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
ctest --build-config Release
```

#### Windows

```powershell
git clone https://github.com/microsoft/vcpkg.git C:/vcpkg
C:/vcpkg/bootstrap-vcpkg.bat
C:/vcpkg/vcpkg.exe integrate install

git clone https://github.com/xorz57/BruteforceSSH.git
cd BruteforceSSH
cmake -B build -DCMAKE_BUILD_TYPE=Release -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
ctest --build-config Release
```

## Stargazers over time

[![Stargazers over time](https://starchart.cc/xorz57/BruteforceSSH.svg?variant=adaptive)](https://starchart.cc/xorz57/BruteforceSSH)
