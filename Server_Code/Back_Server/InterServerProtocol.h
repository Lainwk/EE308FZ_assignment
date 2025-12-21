#pragma once

#include <cstdint>

// TCP bridge between front and back server now uses this simple envelope.
struct InterServerHeader {
    int32_t client_id;   // Logical client identifier assigned by front server
    int32_t payload_len; // Size of following payload in bytes
};

// Back-end listener configuration (front server connects here).
constexpr const char* BACK_SERVER_HOST = "127.0.0.1";
constexpr uint16_t BACK_SERVER_PORT = 12000;
