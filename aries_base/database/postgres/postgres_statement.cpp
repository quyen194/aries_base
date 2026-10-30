/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:32
  filename:  aries_base/database/postgres/postgres_statement.cpp

  purpose:   PostgreSQL prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <random>
#include <stdexcept>
#include <string>

#include "aries_base/database/postgres/postgres_statement.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace postgres {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

PostgresStatement::PostgresStatement(PGconn* conn, const std::string& sql)
    : conn_(conn), sql_(sql), param_count_(0), prepared_(false) {
  // Convert ? placeholders to $1, $2, etc.
  sql_ = ConvertPlaceholders(sql);

  // Generate unique statement name
  static std::random_device rd;
  static std::mt19937 gen(rd());
  stmt_name_ = "stmt_" + std::to_string(gen());

  // Prepare the statement
  PGresult* result =
      PQprepare(conn_, stmt_name_.c_str(), sql_.c_str(), 0, nullptr);

  if (!result || PQresultStatus(result) != PGRES_COMMAND_OK) {
    last_error_ = PQerrorMessage(conn_);
    if (result) {
      PQclear(result);
    }
    throw std::runtime_error("Failed to prepare statement: " + last_error_);
  }

  PQclear(result);
  prepared_ = true;
}
// -----------------------------------------------------------------------------

PostgresStatement::~PostgresStatement() {
  Close();
}
// -----------------------------------------------------------------------------

std::string PostgresStatement::ConvertPlaceholders(const std::string& sql) {
  std::string result;
  int paramNum = 1;
  bool inString = false;

  for (size_t i = 0; i < sql.length(); ++i) {
    char c = sql[i];

    if (c == '\'') {
      inString = !inString;
      result += c;
    } else if (c == '?' && !inString) {
      result += "$" + std::to_string(paramNum++);
    } else {
      result += c;
    }
  }

  param_count_ = paramNum - 1;
  params_.resize(param_count_);

  return result;
}
// -----------------------------------------------------------------------------

void PostgresStatement::BindInt(int index, int value) {
  if (index < 1 || index > param_count_) {
    return;
  }
  params_[index - 1].is_null = false;
  params_[index - 1].value = std::to_string(value);
  params_[index - 1].format = 0;
}
// -----------------------------------------------------------------------------

void PostgresStatement::BindInt64(int index, int64_t value) {
  if (index < 1 || index > param_count_) {
    return;
  }
  params_[index - 1].is_null = false;
  params_[index - 1].value = std::to_string(value);
  params_[index - 1].format = 0;
}
// -----------------------------------------------------------------------------

void PostgresStatement::BindDouble(int index, double value) {
  if (index < 1 || index > param_count_) {
    return;
  }
  params_[index - 1].is_null = false;
  params_[index - 1].value = std::to_string(value);
  params_[index - 1].format = 0;
}
// -----------------------------------------------------------------------------

void PostgresStatement::BindString(int index, const std::string& value) {
  if (index < 1 || index > param_count_) {
    return;
  }
  params_[index - 1].is_null = false;
  params_[index - 1].value = value;
  params_[index - 1].format = 0;
}
// -----------------------------------------------------------------------------

void PostgresStatement::BindNull(int index) {
  if (index < 1 || index > param_count_) {
    return;
  }
  params_[index - 1].is_null = true;
}
// -----------------------------------------------------------------------------

void PostgresStatement::BindBlob(int index, const void* data, size_t size) {
  if (index < 1 || index > param_count_) {
    return;
  }
  params_[index - 1].is_null = false;
  params_[index - 1].value = std::string(static_cast<const char*>(data), size);
  params_[index - 1].format = 1; // Binary format
}
// -----------------------------------------------------------------------------

bool PostgresStatement::Execute() {
  std::vector<const char*> paramPointers(param_count_);
  std::vector<int> paramLengths(param_count_);
  std::vector<int> paramFormats(param_count_);

  for (int i = 0; i < param_count_; ++i) {
    if (!params_[i].is_null) {
      paramPointers[i] = params_[i].value.c_str();
      paramLengths[i] = params_[i].value.length();
      paramFormats[i] = params_[i].format;
    } else {
      paramPointers[i] = nullptr;
      paramLengths[i] = 0;
      paramFormats[i] = 0;
    }
  }

  PGresult* result = PQexecPrepared(conn_,
                                    stmt_name_.c_str(),
                                    param_count_,
                                    paramPointers.data(),
                                    paramLengths.data(),
                                    paramFormats.data(),
                                    0);

  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK &&
                  PQresultStatus(result) != PGRES_TUPLES_OK)) {
    last_error_ = PQerrorMessage(conn_);
    if (result) {
      PQclear(result);
    }
    return false;
  }

  PQclear(result);
  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> PostgresStatement::Query() {
  std::vector<const char*> paramPointers(param_count_);
  std::vector<int> paramLengths(param_count_);
  std::vector<int> paramFormats(param_count_);

  for (int i = 0; i < param_count_; ++i) {
    if (!params_[i].is_null) {
      paramPointers[i] = params_[i].value.c_str();
      paramLengths[i] = params_[i].value.length();
      paramFormats[i] = params_[i].format;
    } else {
      paramPointers[i] = nullptr;
      paramLengths[i] = 0;
      paramFormats[i] = 0;
    }
  }

  PGresult* result = PQexecPrepared(conn_,
                                    stmt_name_.c_str(),
                                    param_count_,
                                    paramPointers.data(),
                                    paramLengths.data(),
                                    paramFormats.data(),
                                    0);

  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK) {
    last_error_ = PQerrorMessage(conn_);
    if (result) {
      PQclear(result);
    }
    return nullptr;
  }

  last_error_.clear();
  return std::make_unique<PostgresResult>(result);
}
// -----------------------------------------------------------------------------

void PostgresStatement::Reset() {
  for (auto& param : params_) {
    param.is_null = true;
    param.value.clear();
    param.format = 0;
  }
  last_error_.clear();
}
// -----------------------------------------------------------------------------

void PostgresStatement::ClearBindings() {
  Reset();
}
// -----------------------------------------------------------------------------

void PostgresStatement::Close() {
  if (prepared_) {
    std::string sql = "DEALLOCATE " + stmt_name_;
    PGresult* result = PQexec(conn_, sql.c_str());
    if (result) {
      PQclear(result);
    }
    prepared_ = false;
  }
}
// -----------------------------------------------------------------------------

std::string PostgresStatement::GetLastError() const {
  return last_error_;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace postgres
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
