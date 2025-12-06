/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 21:17
  filename:  aries_base/database/interfaces/result_sets.hpp

  purpose:   Interface for query result sets
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_INTERFACES_RESULT_SETS_HPP
#define ARIES_BASE_DATABASE_INTERFACES_RESULT_SETS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>
#include <cstdint>
#include <vector>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief Abstract interface for query result sets
 *
 * Result sets allow iteration over rows returned by SELECT queries.
 */
class ResultSets {
 public:
  virtual ~ResultSets() = default;

  /**
   * @brief Move to the next row in the result set
   * @return true if a row is available, false if no more rows
   */
  virtual bool Next() = 0;

  /**
   * @brief Get an integer value from the current row
   * @param col Column index (0-based)
   * @return Integer value
   */
  virtual int GetInt(int col) = 0;

  /**
   * @brief Get a 64-bit integer value from the current row
   * @param col Column index (0-based)
   * @return 64-bit integer value
   */
  virtual int64_t GetInt64(int col) = 0;

  /**
   * @brief Get a double value from the current row
   * @param col Column index (0-based)
   * @return Double value
   */
  virtual double GetDouble(int col) = 0;

  /**
   * @brief Get a string value from the current row
   * @param col Column index (0-based)
   * @return String value
   */
  virtual std::string GetString(int col) = 0;

  /**
   * @brief Get a BLOB (binary data) from the current row
   * @param col Column index (0-based)
   * @return Vector containing binary data
   */
  virtual std::vector<uint8_t> GetBlob(int col) = 0;

  /**
   * @brief Check if a column value is NULL
   * @param col Column index (0-based)
   * @return true if NULL, false otherwise
   */
  virtual bool IsNull(int col) = 0;

  /**
   * @brief Get the number of columns in the result set
   * @return Number of columns
   */
  virtual int GetColumnCount() const = 0;

  /**
   * @brief Get the name of a column
   * @param col Column index (0-based)
   * @return Column name
   */
  virtual std::string GetColumnName(int col) const = 0;

  /**
   * @brief Get an integer value by column name
   * @param columnName Column name
   * @return Integer value
   */
  virtual int GetInt(const std::string& columnName) = 0;

  /**
   * @brief Get a 64-bit integer value by column name
   * @param columnName Column name
   * @return 64-bit integer value
   */
  virtual int64_t GetInt64(const std::string& columnName) = 0;

  /**
   * @brief Get a double value by column name
   * @param columnName Column name
   * @return Double value
   */
  virtual double GetDouble(const std::string& columnName) = 0;

  /**
   * @brief Get a string value by column name
   * @param columnName Column name
   * @return String value
   */
  virtual std::string GetString(const std::string& columnName) = 0;

  /**
   * @brief Check if a column value is NULL by column name
   * @param columnName Column name
   * @return true if NULL, false otherwise
   */
  virtual bool IsNull(const std::string& columnName) = 0;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_INTERFACES_RESULT_SETS_HPP
// -----------------------------------------------------------------------------
