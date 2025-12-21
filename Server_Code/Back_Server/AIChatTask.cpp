#include "AIChatTask.h"
#include "FrontBridgeSender.h"
#include "Protocol.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

// AI API Configuration
#define AI_API_HOST L"mixai.cc"
#define AI_API_PATH L"/v1/chat/completions"
#define AI_API_KEY "sk-AFyQeA2K5dBnWY58"
#define AI_MODEL "claude-sonnet-4-5-20250929"
#define AI_SYSTEM_PROMPT "假设你现在是一名心理医生，使用轻松自然温柔的语气回复用户的话语以及聊天，如果用户有问题需要提出简单的建议，字数不能超过50个字，最低字数没有限制，并且使用中文回复，不使用表情，可以用颜文字，不换行，以下是用户向你的提问："

AIChatTask::AIChatTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

std::string AIChatTask::buildPrompt(const std::string& user_message) {
    return std::string(AI_SYSTEM_PROMPT) + user_message;
}

// Simple JSON string escape
std::string escapeJson(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

// Simple JSON parser to extract AI response
std::string extractAIResponse(const std::string& json) {
    // Look for "content":"..." pattern
    size_t pos = json.find("\"content\"");
    if (pos == std::string::npos) return "";
    
    pos = json.find("\"", pos + 9);  // Find opening quote after "content"
    if (pos == std::string::npos) return "";
    pos++;  // Move past opening quote
    
    size_t end = pos;
    while (end < json.length()) {
        if (json[end] == '"' && (end == 0 || json[end-1] != '\\')) {
            break;
        }
        end++;
    }
    
    if (end >= json.length()) return "";
    
    std::string content = json.substr(pos, end - pos);
    
    // Unescape basic JSON escapes
    std::string result;
    for (size_t i = 0; i < content.length(); i++) {
        if (content[i] == '\\' && i + 1 < content.length()) {
            switch (content[i+1]) {
                case 'n': result += '\n'; i++; break;
                case 't': result += '\t'; i++; break;
                case 'r': result += '\r'; i++; break;
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                default: result += content[i]; break;
            }
        } else {
            result += content[i];
        }
    }
    
    return result;
}

std::string AIChatTask::callAIAPI(const std::string& user_message) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    std::string response;
    
    try {
        // Build JSON request
        std::string prompt = buildPrompt(user_message);
        std::string json_body = "{\"model\":\"" + std::string(AI_MODEL) +
                               "\",\"max_tokens\":150,\"temperature\":0.7," +
                               "\"messages\":[{\"role\":\"user\",\"content\":\"" +
                               escapeJson(prompt) + "\"}]}";
        
        std::cout << "[AIChatTask] Request JSON: " << json_body << std::endl;
        
        // Initialize WinHTTP
        hSession = WinHttpOpen(L"MindCarrer AI Client/1.0",
                              WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                              WINHTTP_NO_PROXY_NAME,
                              WINHTTP_NO_PROXY_BYPASS, 0);
        
        if (!hSession) {
            std::cerr << "[AIChatTask] WinHttpOpen failed" << std::endl;
            return "抱歉，AI服务暂时不可用";
        }
        
        // Connect to server
        hConnect = WinHttpConnect(hSession, AI_API_HOST, INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            std::cerr << "[AIChatTask] WinHttpConnect failed" << std::endl;
            WinHttpCloseHandle(hSession);
            return "抱歉，无法连接到AI服务";
        }
        
        // Create request
        hRequest = WinHttpOpenRequest(hConnect, L"POST", AI_API_PATH,
                                     NULL, WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     WINHTTP_FLAG_SECURE);
        
        if (!hRequest) {
            std::cerr << "[AIChatTask] WinHttpOpenRequest failed" << std::endl;
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "抱歉，创建请求失败";
        }
        
        // Set headers
        std::wstring headers = L"Content-Type: application/json\r\n";
        headers += L"Authorization: Bearer ";
        headers += std::wstring(AI_API_KEY, AI_API_KEY + strlen(AI_API_KEY));
        headers += L"\r\n";
        
        // Send request
        BOOL bResults = WinHttpSendRequest(hRequest,
                                          headers.c_str(), -1,
                                          (LPVOID)json_body.c_str(),
                                          json_body.length(),
                                          json_body.length(), 0);
        
        if (!bResults) {
            std::cerr << "[AIChatTask] WinHttpSendRequest failed" << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "抱歉，发送请求失败";
        }
        
        // Receive response
        bResults = WinHttpReceiveResponse(hRequest, NULL);
        if (!bResults) {
            std::cerr << "[AIChatTask] WinHttpReceiveResponse failed" << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "抱歉，接收响应失败";
        }
        
        // Read data
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        LPSTR pszOutBuffer;
        
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                std::cerr << "[AIChatTask] WinHttpQueryDataAvailable failed" << std::endl;
                break;
            }
            
            if (dwSize == 0) break;
            
            pszOutBuffer = new char[dwSize + 1];
            ZeroMemory(pszOutBuffer, dwSize + 1);
            
            if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
                std::cerr << "[AIChatTask] WinHttpReadData failed" << std::endl;
                delete[] pszOutBuffer;
                break;
            }
            
            response.append(pszOutBuffer, dwDownloaded);
            delete[] pszOutBuffer;
            
        } while (dwSize > 0);
        
        // Cleanup
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        
        std::cout << "[AIChatTask] Response: " << response << std::endl;
        
        // Extract AI response from JSON
        std::string ai_response = extractAIResponse(response);
        if (ai_response.empty()) {
            std::cerr << "[AIChatTask] Failed to extract AI response" << std::endl;
            return "抱歉，解析响应失败";
        }
        
        return ai_response;
        
    } catch (const std::exception& e) {
        std::cerr << "[AIChatTask] Exception: " << e.what() << std::endl;
        if (hRequest) WinHttpCloseHandle(hRequest);
        if (hConnect) WinHttpCloseHandle(hConnect);
        if (hSession) WinHttpCloseHandle(hSession);
        return "抱歉，处理请求时发生错误";
    }
}

void AIChatTask::do_service() {
    // Parse request
    AI_CHAT_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(AI_CHAT_REQ));
    
    std::cout << "[AIChatTask] User " << req.user_id << " message: " << req.message << std::endl;
    
    // Call AI API
    std::string ai_response = callAIAPI(req.message);
    
    std::cout << "[AIChatTask] AI response: " << ai_response << std::endl;
    
    // Build response package
    char resp_package[PACKAGESIZE] = {0};
    HEAD resp_head = {0};
    resp_head.service_type = AI_CHAT;
    resp_head.data_len = sizeof(AI_CHAT_RESP);
    
    AI_CHAT_RESP resp_body = {0};
    resp_body.status_code = 0;  // Success
    strncpy_s(resp_body.ai_response, sizeof(resp_body.ai_response), ai_response.c_str(), _TRUNCATE);
    resp_body.error_msg[0] = '\0';
    
    // Copy to package
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(AI_CHAT_RESP));
    
    // Send response
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    
    std::cout << "[AIChatTask] Response sent to client" << std::endl;
}