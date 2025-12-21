#include "RegisterTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>
#include <iostream>

RegisterTask::RegisterTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void RegisterTask::do_service() {
    REGISTER_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(REGISTER_REQ));

    if (this->package_len < static_cast<int>(sizeof(HEAD) + sizeof(REGISTER_REQ))) {
        std::cerr << "RegisterTask invalid package_len=" << this->package_len << std::endl;
        return;
    }

    HEAD resp_head = {0};
    REGISTER_RESP resp_body = {0};
    resp_head.service_type = REGISTER;

    auto viewField = [](const char* buf, size_t n) {
        size_t len = 0;
        while (len < n && buf[len] != '\0') ++len;
        return std::string(buf, len);
    };

    BaseModel* model = ModelControl::getInstance()->getModel();
    std::string phone = viewField(req.user_phone, sizeof(req.user_phone));
    std::string code = viewField(req.code, sizeof(req.code));
    std::string name = viewField(req.user_name, sizeof(req.user_name));
    std::string pwd = viewField(req.user_pwd, sizeof(req.user_pwd));
    
    int code_status = model->VerifySmsCode(phone, code);
    std::cout << "RegisterTask phone=" << phone << " code=" << code << " code_status=" << code_status << std::endl;
    
    if (code_status == 1) {
        std::cout << "RegisterTask: Code verification failed (wrong code)" << std::endl;
        resp_body.status_code = 1;
    } else if (code_status == 3) {
        std::cout << "RegisterTask: Code verification failed (expired)" << std::endl;
        resp_body.status_code = 3;
    } else {
        std::cout << "RegisterTask: Code verified, registering user..." << std::endl;
        std::cout << "  phone=" << phone << " name=" << name << " pwd_len=" << pwd.length() << std::endl;
        resp_body = model->NewUserRegister(phone, pwd, name);
        std::cout << "RegisterTask: Registration result status_code=" << resp_body.status_code << std::endl;
    }

    resp_head.data_len = sizeof(REGISTER_RESP);
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(REGISTER_RESP));
    
    std::cout << "RegisterTask: Sending response to front server, client_fd=" << this->client_fd
              << " status_code=" << resp_body.status_code << std::endl;
    
    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    
    std::cout << "RegisterTask: Response sent successfully" << std::endl;
}
