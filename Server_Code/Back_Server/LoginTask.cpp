#include "LoginTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>

LoginTask::LoginTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void LoginTask::do_service() {
    LOGIN_REQ recv_body = {0};
    memcpy(&recv_body, this->recv_msg_package + sizeof(HEAD), sizeof(LOGIN_REQ));

    HEAD resp_head = {0};
    LOGIN_RESP resp_body = {0};
    resp_head.service_type = LOGIN;

    BaseModel* model = ModelControl::getInstance()->getModel();

    // 判断登录方式：检查验证码字段是否为空
    std::string phone = std::string(recv_body.user_phone);
    std::string code = std::string(recv_body.code);
    std::string pwd = std::string(recv_body.user_pwd);
    
    // 去除字符串末尾的空字符
    code.erase(std::find(code.begin(), code.end(), '\0'), code.end());
    pwd.erase(std::find(pwd.begin(), pwd.end(), '\0'), pwd.end());
    
    if (!code.empty()) {
        // 验证码登录模式
        std::cout << "[LoginTask] SMS code login mode for phone: " << phone << std::endl;
        int code_status = model->VerifySmsCode(phone, code);
        if (code_status == 1) {
            resp_body.status_code = 1;  // 验证码错误
        } else if (code_status == 3) {
            resp_body.status_code = 4;  // 验证码过期
        } else {
            // 验证码正确，直接登录（不检查密码）
            resp_body = model->LoginCheck(phone, "");  // 传空密码，只验证手机号
        }
    } else if (!pwd.empty()) {
        // 密码登录模式
        std::cout << "[LoginTask] Password login mode for phone: " << phone << std::endl;
        resp_body = model->LoginCheck(phone, pwd);
    } else {
        // 既没有验证码也没有密码
        std::cout << "[LoginTask] ERROR: Neither password nor code provided" << std::endl;
        resp_body.status_code = 2;  // 登录失败
    }

    resp_head.data_len = sizeof(LOGIN_RESP);
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(LOGIN_RESP));

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}

