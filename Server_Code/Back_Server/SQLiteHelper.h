#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

// SQLite RAII wrapper for automatic resource management
class SQLiteDB {
public:
    SQLiteDB() : db_(nullptr) {}
    
    ~SQLiteDB() {
        close();
    }

    // Open database connection
    bool open(const std::string& db_path) {
        close(); // Close any existing connection
        
        int rc = sqlite3_open(db_path.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite] Failed to open database: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_close(db_);
            db_ = nullptr;
            return false;
        }

        // Improve concurrency to avoid "database is locked" during bursts
        sqlite3_busy_timeout(db_, 5000);
        char* err_msg = nullptr;
        rc = sqlite3_exec(db_, "PRAGMA journal_mode=WAL; PRAGMA synchronous=NORMAL;", nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite] Failed to set WAL/synchronous pragmas: " << err_msg << std::endl;
            sqlite3_free(err_msg);
        }
        
        // Enable foreign keys
        rc = sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite] Failed to enable foreign keys: " << err_msg << std::endl;
            sqlite3_free(err_msg);
        }
        
        return true;
    }

    // Close database connection
    void close() {
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
    }

    // Get raw sqlite3 pointer
    sqlite3* get() const { return db_; }

    // Check if database is open
    bool isOpen() const { return db_ != nullptr; }

    // Execute a simple SQL statement (no result set)
    bool execute(const std::string& sql) {
        char* err_msg = nullptr;
        int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite] Execute failed: " << err_msg << std::endl;
            sqlite3_free(err_msg);
            return false;
        }
        return true;
    }

    // Begin transaction
    bool beginTransaction() {
        return execute("BEGIN TRANSACTION;");
    }

    // Commit transaction
    bool commit() {
        return execute("COMMIT;");
    }

    // Rollback transaction
    bool rollback() {
        return execute("ROLLBACK;");
    }

    // Get last insert row id
    long long lastInsertId() const {
        return sqlite3_last_insert_rowid(db_);
    }

    // Get last error message
    std::string lastError() const {
        return db_ ? sqlite3_errmsg(db_) : "Database not open";
    }

private:
    sqlite3* db_;
    
    // Prevent copying
    SQLiteDB(const SQLiteDB&) = delete;
    SQLiteDB& operator=(const SQLiteDB&) = delete;
};

// SQLite prepared statement wrapper
class SQLiteStmt {
public:
    SQLiteStmt() : stmt_(nullptr) {}
    
    ~SQLiteStmt() {
        finalize();
    }

    // Prepare statement
    bool prepare(sqlite3* db, const std::string& sql) {
        finalize();
        
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite] Prepare failed: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        return true;
    }

    // Finalize statement
    void finalize() {
        if (stmt_) {
            sqlite3_finalize(stmt_);
            stmt_ = nullptr;
        }
    }

    // Bind parameters (1-based index)
    bool bindInt(int index, int value) {
        return sqlite3_bind_int(stmt_, index, value) == SQLITE_OK;
    }

    bool bindInt64(int index, long long value) {
        return sqlite3_bind_int64(stmt_, index, value) == SQLITE_OK;
    }

    bool bindDouble(int index, double value) {
        return sqlite3_bind_double(stmt_, index, value) == SQLITE_OK;
    }

    bool bindText(int index, const std::string& value) {
        return sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
    }

    bool bindNull(int index) {
        return sqlite3_bind_null(stmt_, index) == SQLITE_OK;
    }

    // Execute statement (for INSERT/UPDATE/DELETE)
    bool execute() {
        int rc = sqlite3_step(stmt_);
        sqlite3_reset(stmt_);
        return rc == SQLITE_DONE;
    }

    // Step through result set (for SELECT)
    // Returns: SQLITE_ROW if row available, SQLITE_DONE if no more rows, SQLITE_ERROR on error
    int step() {
        return sqlite3_step(stmt_);
    }

    // Reset statement for reuse
    void reset() {
        sqlite3_reset(stmt_);
    }

    // Get column values (0-based index)
    int getInt(int col) const {
        return sqlite3_column_int(stmt_, col);
    }

    long long getInt64(int col) const {
        return sqlite3_column_int64(stmt_, col);
    }

    double getDouble(int col) const {
        return sqlite3_column_double(stmt_, col);
    }

    std::string getText(int col) const {
        const unsigned char* text = sqlite3_column_text(stmt_, col);
        return text ? std::string(reinterpret_cast<const char*>(text)) : "";
    }

    bool isNull(int col) const {
        return sqlite3_column_type(stmt_, col) == SQLITE_NULL;
    }

    // Get column count
    int columnCount() const {
        return sqlite3_column_count(stmt_);
    }

    // Get raw statement pointer
    sqlite3_stmt* get() const { return stmt_; }

private:
    sqlite3_stmt* stmt_;
    
    // Prevent copying
    SQLiteStmt(const SQLiteStmt&) = delete;
    SQLiteStmt& operator=(const SQLiteStmt&) = delete;
};
