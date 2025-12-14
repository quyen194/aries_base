/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:04
  filename:  aries_base/database/mysql/mysql_statement.hpp

  purpose:   MySQL prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_MYSQL_MYSQL_STATEMENT_HPP
#define ARIES_BASE_DATABASE_MYSQL_MYSQL_STATEMENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>
#include <memory>
#include <map>

#include <mysql/mysql.h>

#include "aries_base/database/interfaces/statement.hpp"
#include "aries_base/database/mysql/mysql_result.hpp"
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
/**
 * @brief MySQL prepared statement implementation
 */
class MySQLStatement : public Statement {
 public:
  MySQLStatement(MYSQL* mysql, const std::string& sql);
  ~MySQLStatement() override;

  // Statement interface
  void BindInt(int index, int value) override;
  void BindInt64(int index, int64_t value) override;
  void BindDouble(int index, double value) override;
  void BindString(int index, const std::string& value) override;
  void BindNull(int index) override;
  void BindBlob(int index, const void* data, size_t size) override;
  bool Execute() override;
  std::unique_ptr<ResultSets> Query() override;
  void Reset() override;
  void ClearBindings() override;
  void Close() override;
  std::string GetLastError() const override;

 private:
  struct BindData {
    enum class Type { INT, INT64, DOUBLE, STRING, BLOB, NULL_VAL };
    Type type;
    int intVal;
    int64_t int64Val;
    double doubleVal;
    std::string stringVal;
    std::vector<uint8_t> blobVal;
  };

 private:
  MYSQL* mysql_;
  MYSQL_STMT* stmt_;
  std::string sql_;
  std::string last_error_;
  std::map<int, BindData> bindings_;
  std::vector<MYSQL_BIND> mysql_binds_;
};
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

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_MYSQL_MYSQL_STATEMENT_HPP
// -----------------------------------------------------------------------------
