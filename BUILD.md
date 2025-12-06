# Build Instructions

This document provides detailed instructions for building the aries_base library on different operating systems.

## Table of Contents

- [System Requirements](#system-requirements)
- [Installing Dependencies](#installing-dependencies)
  - [Linux (Ubuntu/Debian)](#linux-ubuntudebian)
  - [Linux (Fedora/RHEL/CentOS)](#linux-fedorарhelcentos)
  - [macOS](#macos)
  - [Windows](#windows)
- [Building the Library](#building-the-library)
- [CMake Options](#cmake-options)
- [Build Targets](#build-targets)
- [Troubleshooting](#troubleshooting)

## System Requirements

- **CMake**: Version 3.20 or higher
- **C++ Compiler**:
  - GCC 7.0+ (Linux)
  - Clang 5.0+ (macOS/Linux)
  - MSVC 2017+ (Windows)
- **C++17** standard support

## Installing Dependencies

### Core Dependencies (Required)

- **spdlog**: Logging library (included as third-party)
- **pthread**: Threading library (usually system-provided)

### Database Dependencies (Optional)

The database abstraction layer supports multiple database engines. You can install only the ones you need:

- **SQLite**: Embedded database
- **MySQL/MariaDB**: Client library
- **PostgreSQL**: Client library
- **ODBC**: For Microsoft SQL Server support

---

## Linux (Ubuntu/Debian)

### Update Package List

```bash
sudo apt-get update
```

### Install Build Tools

```bash
sudo apt-get install -y \
    build-essential \
    cmake \
    git
```

### Install Database Dependencies

#### All Database Libraries (Recommended)

```bash
sudo apt-get install -y \
    libsqlite3-dev \
    libmysqlclient-dev \
    libpq-dev \
    unixodbc-dev
```

#### Individual Libraries

**SQLite:**
```bash
sudo apt-get install -y libsqlite3-dev
```

**MySQL/MariaDB:**
```bash
# For MySQL:
sudo apt-get install -y libmysqlclient-dev

# Or for MariaDB:
sudo apt-get install -y libmariadb-dev
```

**PostgreSQL:**
```bash
sudo apt-get install -y libpq-dev
```

**ODBC (for MSSQL):**
```bash
sudo apt-get install -y unixodbc-dev

# Optional: Install Microsoft ODBC Driver for SQL Server
curl https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
curl https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/prod.list | sudo tee /etc/apt/sources.list.d/mssql-release.list
sudo apt-get update
sudo ACCEPT_EULA=Y apt-get install -y msodbcsql18
```

### Verify Installation

```bash
# Check SQLite
pkg-config --modversion sqlite3

# Check MySQL
mysql_config --version

# Check PostgreSQL
pg_config --version

# Check ODBC
odbcinst -j
```

---

## Linux (Fedora/RHEL/CentOS)

### Install Build Tools

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git
```

### Install Database Dependencies

#### All Database Libraries

```bash
sudo dnf install -y \
    sqlite-devel \
    mysql-devel \
    postgresql-devel \
    unixODBC-devel
```

#### Individual Libraries

**SQLite:**
```bash
sudo dnf install -y sqlite-devel
```

**MySQL/MariaDB:**
```bash
# For MySQL:
sudo dnf install -y mysql-devel

# Or for MariaDB:
sudo dnf install -y mariadb-devel
```

**PostgreSQL:**
```bash
sudo dnf install -y postgresql-devel
```

**ODBC:**
```bash
sudo dnf install -y unixODBC-devel
```

---

## macOS

### Install Homebrew (if not installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Install Build Tools

```bash
brew install cmake
```

### Install Database Dependencies

#### All Database Libraries

```bash
brew install \
    sqlite \
    mysql-client \
    postgresql \
    unixodbc
```

#### Individual Libraries

**SQLite:**
```bash
brew install sqlite
```

**MySQL:**
```bash
brew install mysql-client

# Add to PATH
echo 'export PATH="/opt/homebrew/opt/mysql-client/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

**PostgreSQL:**
```bash
brew install postgresql

# Add to PATH (if needed)
echo 'export PATH="/opt/homebrew/opt/postgresql/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

**ODBC:**
```bash
brew install unixodbc

# For SQL Server support (optional)
brew tap microsoft/mssql-release https://github.com/Microsoft/homebrew-mssql-release
brew install msodbcsql18
```

### Set Library Paths (if needed)

```bash
# Add to ~/.zshrc or ~/.bash_profile
export LIBRARY_PATH="/opt/homebrew/lib:$LIBRARY_PATH"
export CPATH="/opt/homebrew/include:$CPATH"
```

---

## Windows

### Install Visual Studio

Download and install [Visual Studio 2019 or later](https://visualstudio.microsoft.com/downloads/) with:
- Desktop development with C++
- CMake tools for Windows

### Install CMake

Download and install from [cmake.org](https://cmake.org/download/)

Or use Chocolatey:
```powershell
choco install cmake
```

### Install Database Dependencies

#### SQLite

**Option 1: Download Pre-compiled Binaries**

1. Download SQLite amalgamation and DLL from [sqlite.org](https://www.sqlite.org/download.html)
   - `sqlite-amalgamation-*.zip`
   - `sqlite-dll-win64-*.zip`

2. Extract to a directory (e.g., `C:\sqlite`)

3. Create import library:
   ```cmd
   lib /DEF:sqlite3.def /OUT:sqlite3.lib /MACHINE:x64
   ```

**Option 2: Use vcpkg**

```powershell
vcpkg install sqlite3:x64-windows
```

#### MySQL

**Option 1: MySQL Installer**

1. Download [MySQL Installer](https://dev.mysql.com/downloads/installer/)
2. Install "MySQL Connector/C" component
3. Default install path: `C:\Program Files\MySQL\MySQL Connector C 6.1\`

**Option 2: Use vcpkg**

```powershell
vcpkg install libmysql:x64-windows
```

#### PostgreSQL

**Option 1: PostgreSQL Installer**

1. Download [PostgreSQL installer](https://www.postgresql.org/download/windows/)
2. During installation, select "Command Line Tools"
3. Libraries will be in: `C:\Program Files\PostgreSQL\<version>\lib`

**Option 2: Use vcpkg**

```powershell
vcpkg install libpq:x64-windows
```

#### ODBC (for MSSQL)

ODBC is included with Windows. For SQL Server support:

1. Download [Microsoft ODBC Driver for SQL Server](https://docs.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server)
2. Install the driver
3. ODBC libraries are in Windows SDK (usually pre-installed with Visual Studio)

### Using vcpkg (Recommended)

vcpkg is a package manager that simplifies dependency management on Windows.

```powershell
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Install all database dependencies
.\vcpkg install sqlite3:x64-windows libmysql:x64-windows libpq:x64-windows

# Integrate with Visual Studio
.\vcpkg integrate install
```

Then build with:
```powershell
cmake -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

---

## Building the Library

### Linux / macOS

```bash
# Clone repository
git clone <repository-url>
cd aries_base

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Install (optional)
sudo cmake --install .
```

### Windows (Visual Studio)

**Using Command Line:**

```powershell
# Open Developer Command Prompt for VS 2019/2022
cd aries_base
mkdir build
cd build

# Configure
cmake -G "Visual Studio 16 2019" -A x64 ..

# Build
cmake --build . --config Release
```

**Using Visual Studio IDE:**

1. Open Visual Studio
2. Select "Open a local folder"
3. Navigate to the `aries_base` directory
4. Visual Studio will automatically detect CMakeLists.txt
5. Build → Build All

---

## CMake Options

### Database Options

Control which database engines are enabled:

```bash
cmake -DARIES_DB_ENABLE_SQLITE=ON \
      -DARIES_DB_ENABLE_MYSQL=ON \
      -DARIES_DB_ENABLE_POSTGRES=ON \
      -DARIES_DB_ENABLE_MSSQL=ON \
      ..
```

To disable specific databases:

```bash
cmake -DARIES_DB_ENABLE_MYSQL=OFF \
      -DARIES_DB_ENABLE_POSTGRES=OFF \
      ..
```

### Build Type Options

```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Release with debug info
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### Additional Options

```bash
# Enable/disable examples
cmake -DARIES_BASE_BUILD_EXAMPLES=ON ..

# Enable/disable tests
cmake -DARIES_BASE_BUILD_TESTS=ON ..

# Set install prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

---

## Build Targets

### Build All

```bash
cmake --build .
```

### Build Specific Components

```bash
# Build only the library
cmake --build . --target aries_base

# Build all examples
cmake --build . --target aries_base_all_examples

# Build specific example
cmake --build . --target aries_base_example_database_basic

# Build all tests
cmake --build . --target aries_base_all_tests
```

### Run Examples

```bash
# After building
./examples/database/aries_base_example_database_basic
./examples/database/aries_base_example_database_prepare
./examples/database/aries_base_example_database_transaction
```

### Run Tests

```bash
# Using CTest
ctest

# Or run specific test
./tests/process/thread_pool/aries_base_test_process_thread_pool_basic
```

---

## Troubleshooting

### Database Libraries Not Found

**Linux:**
```bash
# Check if libraries are installed
dpkg -l | grep -E 'sqlite|mysql|postgres|odbc'

# Check pkg-config
pkg-config --list-all | grep -E 'sqlite|mysql|postgres'

# Set PKG_CONFIG_PATH if needed
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

**macOS:**
```bash
# Check Homebrew installations
brew list | grep -E 'sqlite|mysql|postgres|odbc'

# If libraries are not found, set CMAKE_PREFIX_PATH
cmake -DCMAKE_PREFIX_PATH="/opt/homebrew" ..
```

**Windows:**
```powershell
# Set environment variables
$env:CMAKE_PREFIX_PATH = "C:\sqlite;C:\Program Files\MySQL\MySQL Connector C 6.1;C:\Program Files\PostgreSQL\15"
cmake ..
```

### CMake Cannot Find Libraries

Manually specify library paths:

```bash
cmake \
  -DSQLITE3_INCLUDE_DIR=/path/to/sqlite/include \
  -DSQLITE3_LIBRARY=/path/to/sqlite/lib/libsqlite3.so \
  -DMYSQL_INCLUDE_DIR=/path/to/mysql/include \
  -DMYSQL_LIBRARY=/path/to/mysql/lib/libmysqlclient.so \
  -DPostgreSQL_INCLUDE_DIR=/path/to/postgres/include \
  -DPostgreSQL_LIBRARY=/path/to/postgres/lib/libpq.so \
  ..
```

### Linker Errors

**Linux:** Make sure to install the `-dev` or `-devel` packages, not just runtime packages.

**macOS:** If you get linker errors, try:
```bash
# Update library cache
sudo ldconfig  # On Linux

# On macOS, check library paths
otool -L /path/to/executable
```

**Windows:** Ensure you're using the correct architecture (x64 vs x86).

### MySQL Header Conflicts

If you encounter conflicts between MySQL and MariaDB headers:

```bash
# Use only MySQL
sudo apt-get remove libmariadb-dev

# Or use only MariaDB
sudo apt-get remove libmysqlclient-dev
```

### ODBC Driver Issues

If ODBC drivers are not detected:

```bash
# List installed drivers
odbcinst -q -d

# Check configuration
odbcinst -j
```

On Windows, verify in "ODBC Data Sources (64-bit)" control panel.

---

## Building for Production

### Optimized Release Build

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native" \
      -DARIES_BASE_BUILD_EXAMPLES=OFF \
      -DARIES_BASE_BUILD_TESTS=OFF \
      ..

cmake --build . --config Release -j$(nproc)
```

### Static Linking

To build with static libraries:

```bash
cmake -DBUILD_SHARED_LIBS=OFF ..
```

---

## Cross-Platform Notes

- The library uses C++17 features and requires a modern compiler
- All database code is conditionally compiled based on available dependencies
- The library will build successfully even if no database libraries are found (core functionality remains available)
- Use the unified API to ensure your code works across all supported databases

---

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [SQLite Documentation](https://www.sqlite.org/docs.html)
- [MySQL Documentation](https://dev.mysql.com/doc/)
- [PostgreSQL Documentation](https://www.postgresql.org/docs/)
- [Microsoft ODBC Documentation](https://docs.microsoft.com/en-us/sql/odbc/)

---

## Getting Help

If you encounter issues:

1. Check the [Troubleshooting](#troubleshooting) section above
2. Review CMake output for specific error messages
3. Ensure all dependencies are installed correctly
4. Check the [database README](aries_base/database/README.md) for database-specific information
5. Open an issue on the project repository with:
   - Your OS and version
   - CMake version (`cmake --version`)
   - Compiler version
   - Full CMake configuration output
   - Complete error messages
