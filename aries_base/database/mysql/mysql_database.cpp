/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:24
  filename:  aries_base/database/mysql/mysql_database.cpp

  purpose:   MySQL database implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <stdexcept>
#include <cstring>
#include <sstream>

#include "aries_base/database/mysql/mysql_database.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mysql {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

MySQLDatabase::MySQLDatabase()
  : mysql_(nullptr), connected_(false) {
  mysql_ = mysql_init(nullptr);
  if (!mysql_) {
    throw std::runtime_error("Failed to initialize MySQL");
  }
}
// -----------------------------------------------------------------------------

MySQLDatabase::~MySQLDatabase() {
  Disconnect();
  if (mysql_) {
    mysql_close(mysql_);
  }
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::ParseConnectionString(const std::string& connStr,
                                          std::string& host,
                                          int& port,
                                          std::string& user,
                                          std::string& password,
                                          std::string& database) {
  // Parse format: "host=127.0.0.1;port=3306;user=root;password=pass;database=mydb"
  std::istringstream iss(connStr);
  std::string token;

  host = "localhost";
  port = 3306;

  while (std::getline(iss, token, ';')) {
    size_t pos = token.find('=');
    if (pos != std::string::npos) {
      std::string key = token.substr(0, pos);
      std::string value = token.substr(pos + 1);

      if (key == "host") {
        host = value;
      } else if (key == "port") {
        port = std::stoi(value);
      } else if (key == "user") {
        user = value;
      } else if (key == "password" || key == "pwd") {
        password = value;
      } else if (key == "database" || key == "db") {
        database = value;
      }
    }
  }

  return !user.empty();
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::Connect(const std::string& connectionString) {
  if (connected_) {
    Disconnect();
  }

  std::string host, user, password, database;
  int port;

  if (!ParseConnectionString(
          connectionString, host, port, user, password, database)) {
    last_error_ = "Invalid connection string format";
    return false;
  }

  // Set connection options
  mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8mb4");

  if (!mysql_real_connect(mysql_,
                          host.c_str(),
                          user.c_str(),
                          password.c_str(),
                          database.c_str(),
                          port,
                          nullptr,
                          0)) {
    last_error_ = mysql_error(mysql_);
    connected_ = false;
    return false;
  }

  connected_ = true;
  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

void MySQLDatabase::Disconnect() {
  if (mysql_ && connected_) {
    mysql_close(mysql_);
    mysql_ = mysql_init(nullptr);
  }
  connected_ = false;
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::IsConnected() const {
  return connected_ && mysql_ping(mysql_) == 0;
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::Begin() {
  return mysql_query(mysql_, "START TRANSACTION") == 0;
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::Commit() {
  return mysql_commit(mysql_) == 0;
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::Rollback() {
  return mysql_rollback(mysql_) == 0;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MySQLDatabase::Execute(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  if (mysql_query(mysql_, sql.c_str()) != 0) {
    last_error_ = mysql_error(mysql_);
    return nullptr;
  }

  MYSQL_RES* result = mysql_store_result(mysql_);
  last_error_.clear();

  if (result) {
    return std::make_unique<MySQLResult>(result);
  } else {
    // No result set (INSERT, UPDATE, DELETE, etc.)
    if (mysql_field_count(mysql_) == 0) {
      // Query was successful and should not return data
      return std::make_unique<MySQLResult>(nullptr);
    } else {
      // Error occurred
      last_error_ = mysql_error(mysql_);
      return nullptr;
    }
  }
}
// -----------------------------------------------------------------------------

std::unique_ptr<Statement> MySQLDatabase::Prepare(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  try {
    return std::make_unique<MySQLStatement>(mysql_, sql);
  } catch (const std::exception& e) {
    last_error_ = e.what();
    return nullptr;
  }
}
// -----------------------------------------------------------------------------

std::string MySQLDatabase::GetLastError() const {
  return last_error_;
}
// -----------------------------------------------------------------------------

int64_t MySQLDatabase::GetLastInsertId() const {
  if (!connected_) {
    return 0;
  }
  return mysql_insert_id(mysql_);
}
// -----------------------------------------------------------------------------

int64_t MySQLDatabase::GetAffectedRows() const {
  if (!connected_) {
    return 0;
  }
  return mysql_affected_rows(mysql_);
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MySQLDatabase::ShowTables() {
  return Execute("SHOW TABLES");
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MySQLDatabase::ShowDatabases() {
  return Execute("SHOW DATABASES");
}
// -----------------------------------------------------------------------------

std::string MySQLDatabase::GetServerVersion() {
  if (!connected_) {
    return "";
  }
  return mysql_get_server_info(mysql_);
}
// -----------------------------------------------------------------------------

bool MySQLDatabase::SetCharset(const std::string& charset) {
  if (!connected_) {
    return false;
  }
  return mysql_set_character_set(mysql_, charset.c_str()) == 0;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace mysql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
