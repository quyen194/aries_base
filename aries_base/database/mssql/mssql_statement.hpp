/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:57
  filename:  aries_base/database/mssql/mssql_statement.hpp

  purpose:   MSSQL prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_MSSQL_MSSQL_STATEMENT_HPP
#define ARIES_BASE_DATABASE_MSSQL_MSSQL_STATEMENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>

#include <sql.h>
#include <sqlext.h>

#include "aries_base/database/interfaces/statement.hpp"
#include "aries_base/database/mssql/mssql_result.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mssql {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief MSSQL prepared statement implementation
 */
class MSSQLStatement : public Statement {
 public:
  MSSQLStatement(SQLHDBC hdbc, const std::string& sql);
  ~MSSQLStatement() override;

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
  std::string GetLastError() const override;

 private:
  std::string GetODBCError();

 private:
  struct BindData {
    enum class Type { INT, INT64, DOUBLE, STRING, BLOB, NULL_VAL };
    Type type;
    SQLLEN indicator;
    int intVal;
    int64_t int64Val;
    double doubleVal;
    std::string stringVal;
    std::vector<uint8_t> blobVal;
  };

 private:
  SQLHDBC hdbc_;
  SQLHSTMT hstmt_;
  std::string sql_;
  std::string last_error_;
  std::vector<BindData> bindings_;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace mssql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_MSSQL_MSSQL_STATEMENT_HPP
// -----------------------------------------------------------------------------
