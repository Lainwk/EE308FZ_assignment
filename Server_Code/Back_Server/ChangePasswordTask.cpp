#include "ChangePasswordTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>
#include <iostream>

ChangePasswordTask::ChangePasswordTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void ChangePasswordTask::do_service() {
    CHANGE_PASSWORD_REQ recv_body = {0};
    memcpy(&recv_body, this->recv_msg_package + sizeof(HEAD), sizeof(CHANGE_PASSWORD_REQ));

    HEAD resp_head = {0};
    CHANGE_PASSWORD_RESP resp_body = {0};
    resp_head.service_type = CHANGE_PASSWORD;

    BaseModel* model = ModelControl::getInstance()->getModel();

    std::string user_id = std::string(recv_body.user_id);
    std::string old_password = std::string(recv_body.old_password);
    std::string new_password = std::string(recv_body.new_password);
    
    // 去除字符串末尾的空字符
    user_id.erase(std::find(user_id.begin(), user_id.end(), '\0'), user_id.end());
    old_password.erase(std::find(old_password.begin(), old_password.end(), '\0'), old_password.end());
    new_password.erase(std::find(new_password.begin(), new_password.end(), '\0'), new_password.end());
    
    std::cout << "[ChangePasswordTask] User: " << user_id << " requesting password change" << std::endl;

    // 调用Model层修改密码
    BaseModel::ChangePasswordResult result = model->ChangePassword(user_id, old_password, new_password);
    
    resp_body.status_code = result.status_code;
    strncpy_s(resp_body.message, sizeof(resp_body.message), result.message.c_str(), _TRUNCATE);
    
    std::cout << "[ChangePasswordTask] Result: " << result.status_code 
              << " Message: " << result.message << std::endl;

    resp_head.data_len = sizeof(CHANGE_PASSWORD_RESP);
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(CHANGE_PASSWORD_RESP));

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}