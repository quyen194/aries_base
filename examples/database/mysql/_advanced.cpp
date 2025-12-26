/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/21 06:21
  filename:  aries_base/examples/database/mysql/advanced.cpp

  purpose:   Complex real-world scenarios with MySQL database
*********************************************************************/

// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>

#include <aries_base/database/db_factory.hpp>

#include "examples/database/mysql/_settings.hpp"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void setup() {
  try {
    auto db = DatabaseFactory::Create(DBType::MySQL);
    db->Connect(MYSQL_CONNECTION_STRING);
    db->Execute("DROP DATABASE IF EXISTS test");
    db->Execute("CREATE DATABASE test");
    db->Disconnect();
    std::cout << "✓ Example setup successful\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Example setup failed: " << e.what() << "\n";
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== MySQL Database Library - Advanced Example ===" << std::endl
            << std::endl;

  setup();

  try {
    auto db = DatabaseFactory::Create(DBType::MySQL);

    if (!db->Connect(MYSQL_CONNECTION_STRING)) {
      std::cerr << "Failed to connect: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Connected to database" << std::endl << std::endl;

    // ====================================================================
    // Setup: Create products and orders tables
    // ====================================================================
    std::cout << "--- Setting up database schema ---" << std::endl;

    if (!db->Execute(R"(
            CREATE TABLE products (
                id INTEGER PRIMARY KEY AUTO_INCREMENT,
                name TEXT NOT NULL,
                price REAL NOT NULL,
                stock INTEGER NOT NULL
            )
        )")) {
      std::cerr << "Failed to create products table: " << db->GetLastError() << std::endl;
      return 1;
    }

    if (!db->Execute(R"(
            CREATE TABLE orders (
                id INTEGER PRIMARY KEY AUTO_INCREMENT,
                product_id INTEGER NOT NULL,
                quantity INTEGER NOT NULL,
                order_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                total REAL NOT NULL,
                FOREIGN KEY(product_id) REFERENCES products(id)
            )
        )")) {
      std::cerr << "Failed to create orders table: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Created tables: products, orders" << std::endl << std::endl;

    // ====================================================================
    // Example 1: Insert products with prepared statements
    // ====================================================================
    std::cout << "--- Example 1: Inserting Products ---" << std::endl;

    auto insertProduct = db->Prepare(
        "INSERT INTO products (name, price, stock) VALUES (?, ?, ?)");

    if (!insertProduct) {
      std::cerr << "Failed to prepare insert product statement: " << db->GetLastError()
                << std::endl;
      return 1;
    }

    struct Product {
      const char* name;
      double price;
      int stock;
    };

    Product products[] = {{"Laptop", 999.99, 10},
                          {"Mouse", 29.99, 50},
                          {"Keyboard", 79.99, 30},
                          {"Monitor", 299.99, 15},
                          {"USB Cable", 9.99, 100}};

    for (const auto& p : products) {
      insertProduct->Reset();
      insertProduct->BindString(1, p.name);
      insertProduct->BindDouble(2, p.price);
      insertProduct->BindInt(3, p.stock);
      if (!insertProduct->Execute()) {
        std::cerr << "Failed to insert product " << p.name << ": "
                  << db->GetLastError() << std::endl;
        return 1;
      }
      std::cout << "  Inserted: " << p.name << " - $" << std::fixed
                << std::setprecision(2) << p.price << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 2: Complex queries with JOINs (simulated)
    // ====================================================================
    std::cout << "--- Example 2: Inserting Orders and Calculations ---"
              << std::endl;

    auto insertOrder = db->Prepare(
        "INSERT INTO orders (product_id, quantity, total) VALUES (?, ?, ?)");

    if (!insertOrder) {
      std::cerr << "Failed to prepare insert order statement: " << db->GetLastError()
                << std::endl;
      return 1;
    }

    // Simulate some orders
    struct Order {
      int product_id;
      int quantity;
    };

    Order orders[] = {
        {1, 2},  // 2 Laptops
        {2, 5},  // 5 Mice
        {3, 3},  // 3 Keyboards
        {1, 1},  // 1 Laptop
        {4, 2}   // 2 Monitors
    };

    for (const auto& order : orders) {
      // Get product info for price calculation
      std::string query = "SELECT price FROM products WHERE id = " +
                          std::to_string(order.product_id);
      auto result = db->Execute(query);

      if (!result) {
        std::cerr << "Failed to retrieve product price: " << db->GetLastError()
                  << std::endl;
        return 1;
      }

      double total = 0.0;
      if (result && result->Next()) {
        total = result->GetDouble(0) * order.quantity;
      }

      insertOrder->Reset();
      insertOrder->BindInt(1, order.product_id);
      insertOrder->BindInt(2, order.quantity);
      insertOrder->BindDouble(3, total);
      if (!insertOrder->Execute()) {
        std::cerr << "Failed to insert order for product ID "
                  << order.product_id << ": " << db->GetLastError()
                  << std::endl;
        return 1;
      }

      std::cout << "  Order: Product ID " << order.product_id << " x "
                << order.quantity << " = $" << std::fixed
                << std::setprecision(2) << total << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 3: Aggregate functions
    // ====================================================================
    std::cout << "--- Example 3: Product Statistics ---" << std::endl;

    auto statsResult = db->Execute(R"(
            SELECT
                name,
                price,
                stock,
                (SELECT COUNT(*) FROM orders WHERE product_id = products.id) as orders_count,
                COALESCE((SELECT SUM(total) FROM orders WHERE product_id = products.id), 0) as total_revenue
            FROM products
            ORDER BY total_revenue DESC
        )");

    if (!statsResult) {
      std::cerr << "Failed to retrieve product statistics: " << db->GetLastError()
                << std::endl;
      return 1;
    }

    std::cout << std::left
              << std::setw(15) << "Product"
              << std::setw(10) << "Price"
              << std::setw(10) << "Stock"
              << std::setw(10) << "Orders"
              << std::setw(15) << "Revenue"
              << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    while (statsResult->Next()) {
      std::cout << std::left << std::setw(15) << statsResult->GetString(0)
                << "$" << std::right << std::setw(7) << std::fixed << std::setprecision(2) << statsResult->GetDouble(1)
                << std::setw(2) << " "
                << std::right << std::setw(5) << statsResult->GetInt(2)
                << std::setw(5) << " "
                << std::right << std::setw(6) << statsResult->GetInt(3)
                << std::setw(4) << " "
                << "$" << std::right << std::setw(7) << std::fixed << std::setprecision(2) << statsResult->GetDouble(4)
                << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 4: Update inventory after orders
    // ====================================================================
    std::cout << "--- Example 4: Transaction - Processing Orders ---"
              << std::endl;

    db->Begin();

    try {
      // Reduce stock for each product ordered
      auto updateStock =
          db->Prepare("UPDATE products SET stock = stock - ? WHERE id = ?");

      if (!updateStock) {
        throw std::runtime_error("Failed to prepare update stock statement");
      }

      for (const auto& order : orders) {
        updateStock->Reset();
        updateStock->BindInt(1, order.quantity);
        updateStock->BindInt(2, order.product_id);
        if (!updateStock->Execute()) {
          throw std::runtime_error("Failed to update stock");
        }
      }

      db->Commit();
      std::cout << "All stock updates completed successfully" << std::endl;

    } catch (const std::exception& e) {
      db->Rollback();
      std::cerr << "Transaction failed: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 5: View updated inventory
    // ====================================================================
    std::cout << "--- Example 5: Updated Inventory ---" << std::endl;

    auto inventoryResult =
        db->Execute("SELECT id, name, stock FROM products ORDER BY id");

    if (!inventoryResult) {
      std::cerr << "Failed to retrieve inventory: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(20) << "Product"
              << std::setw(10) << "Stock"
              << std::endl;
    std::cout << std::string(35, '-') << std::endl;

    while (inventoryResult->Next()) {
      int id = inventoryResult->GetInt(0);
      std::string name = inventoryResult->GetString(1);
      int stock = inventoryResult->GetInt(2);

      std::cout << std::left << std::setw(5) << id << std::setw(20) << name
                << std::setw(10) << stock;

      if (stock < 20) {
        std::cout << " [LOW STOCK]";
      }
      std::cout << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 6: Summary statistics
    // ====================================================================
    std::cout << "--- Example 6: Business Summary ---" << std::endl;

    auto summaryResult = db->Execute(R"(
            SELECT
                COUNT(DISTINCT id) as total_products,
                SUM(stock) as total_stock,
                SUM(price * stock) as inventory_value,
                COUNT(DISTINCT (SELECT COUNT(*) FROM orders WHERE product_id = products.id)) as products_with_orders
            FROM products
        )");

    if (!summaryResult) {
      std::cerr << "Failed to retrieve summary statistics: " << db->GetLastError()
                << std::endl;
      return 1;
    }

    while (summaryResult->Next()) {
      std::cout << "Total Products: " << summaryResult->GetInt(0) << std::endl;
      std::cout << "Total Stock: " << summaryResult->GetInt(1) << " units" << std::endl;
      std::cout << "Inventory Value: $" << std::fixed << std::setprecision(2) << summaryResult->GetDouble(2) << std::endl;
    }

    std::cout << std::endl;

    auto ordersSummary = db->Execute(
        "SELECT COUNT(*) as order_count, SUM(total) as total_sales FROM orders");

    if (!ordersSummary) {
      std::cerr << "Failed to retrieve orders summary: " << db->GetLastError()
                << std::endl;
      return 1;
    }

    while (ordersSummary->Next()) {
      std::cout << "Total Orders: " << ordersSummary->GetInt(0) << std::endl;
      std::cout << "Total Sales: $" << std::fixed << std::setprecision(2) << ordersSummary->GetDouble(1) << std::endl;
    }

    std::cout << std::endl;

    db->Disconnect();
    std::cout << "\n=== Example Complete ===" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
// -----------------------------------------------------------------------------
