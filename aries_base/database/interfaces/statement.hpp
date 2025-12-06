/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 21:28
  filename:  aries_base/database/interfaces/statement.hpp

  purpose:   Interface for prepared statements
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_INTERFACES_STATEMENT_HPP
#define ARIES_BASE_DATABASE_INTERFACES_STATEMENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <memory>
#include <string>
#include <cstdint>

#include "aries_base/database/interfaces/result_sets.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief Abstract interface for prepared statements
 *
 * Prepared statements allow efficient execution of parameterized queries
 * and help prevent SQL injection attacks.
 */
class Statement {
 public:
  virtual ~Statement() = default;

  /**
   * @brief Bind an integer value to a parameter
   * @param index Parameter index (1-based)
   * @param value Integer value to bind
   */
  virtual void BindInt(int index, int value) = 0;

  /**
   * @brief Bind a 64-bit integer value to a parameter
   * @param index Parameter index (1-based)
   * @param value 64-bit integer value to bind
   */
  virtual void BindInt64(int index, int64_t value) = 0;

  /**
   * @brief Bind a double value to a parameter
   * @param index Parameter index (1-based)
   * @param value Double value to bind
   */
  virtual void BindDouble(int index, double value) = 0;

  /**
   * @brief Bind a string value to a parameter
   * @param index Parameter index (1-based)
   * @param value String value to bind
   */
  virtual void BindString(int index, const std::string& value) = 0;

  /**
   * @brief Bind a NULL value to a parameter
   * @param index Parameter index (1-based)
   */
  virtual void BindNull(int index) = 0;

  /**
   * @brief Bind a BLOB (binary data) to a parameter
   * @param index Parameter index (1-based)
   * @param data Pointer to binary data
   * @param size Size of binary data in bytes
   */
  virtual void BindBlob(int index, const void* data, size_t size) = 0;

  /**
   * @brief Execute the prepared statement (for INSERT, UPDATE, DELETE)
   * @return true if execution successful, false otherwise
   */
  virtual bool Execute() = 0;

  /**
   * @brief Execute the prepared statement and return results (for SELECT)
   * @return Result set
   */
  virtual std::unique_ptr<ResultSets> Query() = 0;

  /**
   * @brief Reset the statement for re-execution
   */
  virtual void Reset() = 0;

  /**
   * @brief Clear all bound parameters
   */
  virtual void ClearBindings() = 0;

  /**
   * @brief Get the last error message
   * @return Error message string
   */
  virtual std::string GetLastError() const = 0;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_INTERFACES_STATEMENT_HPP
// -----------------------------------------------------------------------------
