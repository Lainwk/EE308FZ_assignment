#include "SmsCodeTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"
#include "SmsCodeHttpClient.h"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

SmsCodeTask::SmsCodeTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

static std::string generate_code() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1000, 9999);
    return std::to_string(dist(gen));
}

void SmsCodeTask::do_service() {
    try {
        SMS_CODE_REQ req = {0};
        memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(SMS_CODE_REQ));

        // Basic validation to avoid corrupted payload
        if (this->package_len < static_cast<int>(sizeof(HEAD) + sizeof(SMS_CODE_REQ))) {
            std::cerr << "SmsCodeTask invalid package_len=" << this->package_len << std::endl;
            return;
        }

        auto viewField = [](const char* buf, size_t n) {
            size_t len = 0;
            while (len < n && buf[len] != '\0') ++len;
            return std::string(buf, len);
        };

        std::string phone = viewField(req.user_phone, sizeof(req.user_phone));
        std::cout << "SmsCodeTask start phone=" << phone << " package_len=" << this->package_len << std::endl;

        HEAD resp_head = {0};
        SMS_CODE_RESP resp_body = {0};
        resp_head.service_type = SMS_CODE;

        std::string code = generate_code();
        bool send_ok = false;
        try {
            SmsCodeHttpClient client;
            std::string content = std::string("您的验证码是：") + code + "。请不要把验证码泄露给其他人。";
            std::cout << "SmsCodeTask sending SMS to " << phone << " content_len=" << content.size() << std::endl;
            std::string resp = client.sendSMS(phone, content);
            std::cout << "SmsCodeTask sendSMS done, resp_size=" << resp.size() << std::endl;
            if (resp.find("Success") != std::string::npos || resp.find("\"code\":2") != std::string::npos) {
                send_ok = true;
            } else {
                std::cerr << "SmsCodeTask sendSMS response: " << resp << std::endl;
                send_ok = false;
            }
        } catch (const std::exception& e) {
            std::cerr << "SmsCodeTask sendSMS exception: " << e.what() << std::endl;
            send_ok = false;
        } catch (...) {
            std::cerr << "SmsCodeTask sendSMS unknown exception" << std::endl;
            send_ok = false;
        }

        if (send_ok) {
            int store_res = ModelControl::getInstance()->getModel()->StoreSmsCode(phone, code, "login");
            if (store_res != 0) {
                std::cerr << "SmsCodeTask StoreSmsCode failed, res=" << store_res << std::endl;
            }
            resp_body.status_code = (store_res == 0) ? 0 : 1;
        } else {
            resp_body.status_code = 1;
        }

        resp_head.data_len = sizeof(SMS_CODE_RESP);
        char resp_package[PACKAGESIZE] = {0};
        memcpy(resp_package, &resp_head, sizeof(HEAD));
        memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(SMS_CODE_RESP));
        FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    } catch (const std::bad_alloc& e) {
        std::cerr << "SmsCodeTask bad_alloc: " << e.what() << std::endl;
        // Return failure to client to avoid hanging
        HEAD resp_head = {SMS_CODE, sizeof(SMS_CODE_RESP)};
        SMS_CODE_RESP resp_body = {1};
        char resp_package[PACKAGESIZE] = {0};
        memcpy(resp_package, &resp_head, sizeof(HEAD));
        memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(SMS_CODE_RESP));
        FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
    } catch (const std::exception& e) {
        std::cerr << "SmsCodeTask exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "SmsCodeTask unknown fatal exception" << std::endl;
    }
}


