// Simple program to create and initialize SQLite database
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::cout << "SQLite Database Initialization Tool" << std::endl;
    std::cout << "====================================" << std::endl;
    
    const char* db_file = "mindcarrer.db";
    const char* sql_file = "init_sqlite_db.sql";
    
    // Read SQL file
    std::ifstream file(sql_file);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open SQL file: " << sql_file << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();
    file.close();
    
    std::cout << "SQL file loaded: " << sql_file << std::endl;
    std::cout << "SQL size: " << sql.length() << " bytes" << std::endl;
    
    // Open database
    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_file, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "ERROR: Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }
    
    std::cout << "Database opened: " << db_file << std::endl;
    
    // Execute SQL
    char* err_msg = nullptr;
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "ERROR: SQL execution failed: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    
    std::cout << "SQL executed successfully!" << std::endl;
    
    // Verify tables
    const char* verify_sql = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;";
    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, verify_sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        std::cout << "\nCreated tables:" << std::endl;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* name = sqlite3_column_text(stmt, 0);
            std::cout << "  - " << name << std::endl;
        }
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    
    std::cout << "\n====================================" << std::endl;
    std::cout << "SUCCESS: Database initialized!" << std::endl;
    std::cout << "Database file: " << db_file << std::endl;
    std::cout << "====================================" << std::endl;
    
    return 0;
}