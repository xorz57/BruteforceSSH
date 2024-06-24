# BruteforceSSH

[![Build](https://github.com/xorz57/BruteforceSSH/actions/workflows/Build.yml/badge.svg)](https://github.com/xorz57/BruteforceSSH/actions/workflows/Build.yml)

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

## Usage

```ps1
PS C:\Users\xorz57\Desktop\BruteforceSSH\build> .\BruteforceSSH.exe --help
Usage: BruteforceSSH [options]
Options:
  -f, --file <filename>      File
  -h, --host <hostname>      Host (default: 127.0.0.1)
  -p, --port <port>          Port (default: 22)
  -t, --timeout <timeout>    Timeout (seconds, default: 30)
  --help                     Print help
```

## Example

```ps1
PS C:\Users\xorz57\Desktop\BruteforceSSH\build> .\BruteforceSSH.exe -h localhost -f .\assets\accounts.txt -p 2222
Failure: pi:root
Failure: pi:toor
Failure: pi:1234
Success: pi:raspberry
```

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
