#pragma once

#include "BaseTask.h"
#include <string>

class AIChatTask : public BaseTask {
public:
    AIChatTask(char* recv_msg_package, int len, int client_fd);
    void do_service() override;

private:
    // Call AI API and get response
    std::string callAIAPI(const std::string& user_message);
    
    // Build the full prompt with system instruction
    std::string buildPrompt(const std::string& user_message);
};