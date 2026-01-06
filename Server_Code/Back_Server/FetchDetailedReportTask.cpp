#include "FetchDetailedReportTask.h"
#include "FrontBridgeSender.h"
#include "ModelControl.h"

#include <cstring>
#include <string>

FetchDetailedReportTask::FetchDetailedReportTask(char* recv_msg_package, int len, int client_fd)
    : BaseTask(recv_msg_package, len, client_fd) {}

void FetchDetailedReportTask::do_service() {
    FETCH_DETAILED_REPORT_REQ req = {0};
    memcpy(&req, this->recv_msg_package + sizeof(HEAD), sizeof(FETCH_DETAILED_REPORT_REQ));

    // Parse user_id
    long long user_id = 0;
    try {
        user_id = std::stoll(std::string(req.user_id));
    } catch (...) {
        user_id = 0;
    }

    // Fetch detailed report
    BaseModel::ReportData report = ModelControl::getInstance()->getModel()->FetchDetailedReport(
        user_id, 
        req.scope_days > 0 ? req.scope_days : 30
    );

    // Build response
    FETCH_DETAILED_REPORT_RESP resp_body = {};
    resp_body.status_code = report.status_code;
    resp_body.indicator_count = static_cast<int>(report.indicators.size());
    resp_body.summary_len = static_cast<int>(report.summary.size());

    // Calculate total data length
    int total_data_len = 0;
    for (const auto& ind : report.indicators) {
        total_data_len += sizeof(INDICATOR_ITEM);
        total_data_len += static_cast<int>(ind.trend_data.size()) * sizeof(TREND_POINT);
    }
    total_data_len += resp_body.summary_len;
    resp_body.total_data_len = total_data_len;

    // Check if response fits in package
    int base_size = sizeof(HEAD) + sizeof(FETCH_DETAILED_REPORT_RESP);
    int total_size = base_size + total_data_len;

    if (total_size > PACKAGESIZE) {
        // Response too large, return error or truncate
        resp_body.status_code = 2;  // Package size exceeded
        resp_body.indicator_count = 0;
        resp_body.summary_len = 0;
        resp_body.total_data_len = 0;
        total_size = base_size;
    }

    // Build response package
    HEAD resp_head = {FETCH_DETAILED_REPORT, total_size - static_cast<int>(sizeof(HEAD))};
    char resp_package[PACKAGESIZE] = {0};
    memcpy(resp_package, &resp_head, sizeof(HEAD));
    memcpy(resp_package + sizeof(HEAD), &resp_body, sizeof(FETCH_DETAILED_REPORT_RESP));

    if (resp_body.status_code == 0 && resp_body.indicator_count > 0) {
        int offset = sizeof(HEAD) + sizeof(FETCH_DETAILED_REPORT_RESP);

        // Write INDICATOR_ITEM array
        for (const auto& ind : report.indicators) {
            INDICATOR_ITEM item = {};
            strncpy_s(item.dimension_code, sizeof(item.dimension_code), 
                      ind.dimension_code.c_str(), _TRUNCATE);
            item.last_score = ind.last_score;
            item.avg_7d = ind.avg_7d;
            item.avg_30d = ind.avg_30d;
            strncpy_s(item.trend, sizeof(item.trend), ind.trend.c_str(), _TRUNCATE);
            strncpy_s(item.risk_level, sizeof(item.risk_level), 
                      ind.risk_level.c_str(), _TRUNCATE);
            item.trend_point_count = static_cast<int>(ind.trend_data.size());

            memcpy(resp_package + offset, &item, sizeof(INDICATOR_ITEM));
            offset += sizeof(INDICATOR_ITEM);
        }

        // Write all trend points
        for (const auto& ind : report.indicators) {
            for (const auto& point : ind.trend_data) {
                TREND_POINT tp = {};
                strncpy_s(tp.date, sizeof(tp.date), point.date.c_str(), _TRUNCATE);
                tp.score = point.score;
                memcpy(resp_package + offset, &tp, sizeof(TREND_POINT));
                offset += sizeof(TREND_POINT);
            }
        }

        // Write summary text
        if (resp_body.summary_len > 0) {
            memcpy(resp_package + offset, report.summary.data(), resp_body.summary_len);
            offset += resp_body.summary_len;
        }
    }

    FrontBridgeSender::getInstance()->writeToShm(resp_package, PACKAGESIZE, this->client_fd);
}