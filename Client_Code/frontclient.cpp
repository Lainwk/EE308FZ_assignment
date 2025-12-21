#include "frontclient.h"
#include <QHostAddress>
#include <QDebug>
#include <QDateTime>

FrontClient::FrontClient() {
    reconnectTimer = new QTimer(this);
    reconnectTimer->setInterval(5000);
    connect(reconnectTimer, &QTimer::timeout, this, &FrontClient::onReconnectTimer);
}

FrontClient& FrontClient::instance() {
    static FrontClient inst;
    return inst;
}

bool FrontClient::connectToServer(const QString& host, int port) {
    currentHost = host;
    currentPort = port;
    mainSocket.abort();
    mainSocket.connectToHost(QHostAddress(host), port);
    bool connected = mainSocket.waitForConnected(3000);
    qDebug() << "[FrontClient] Main socket connected:" << connected;

    if (connected) {
        emit connectionStateChanged(true);
    }

    return connected;
}

bool FrontClient::isConnected() const {
    return mainSocket.state() == QAbstractSocket::ConnectedState;
}

bool FrontClient::ensureConnected() {
    if (isConnected()) {
        return true;
    }
    mainSocket.abort();
    mainSocket.connectToHost(QHostAddress(currentHost), currentPort);
    bool connected = mainSocket.waitForConnected(3000);
    qDebug() << "[FrontClient] Reconnected:" << connected;

    if (connected) {
        emit connectionStateChanged(true);
    }

    return connected;
}

bool FrontClient::sendPacket(const QByteArray& packet) {
    if (!ensureConnected()) {
        qDebug() << "[FrontClient] sendPacket failed: not connected";
        return false;
    }
    qint64 written = mainSocket.write(packet);
    if (written != packet.size()) {
        qDebug() << "[FrontClient] sendPacket failed: written=" << written;
        return false;
    }
    bool flushed = mainSocket.waitForBytesWritten(3000);
    qDebug() << "[FrontClient] sendPacket: written=" << written << "flushed=" << flushed;
    return flushed;
}

bool FrontClient::sendSimple(int serviceType, const QByteArray& body) {
    HEAD head = {serviceType, body.size()};
    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&head), sizeof(head));
    packet.append(body);
    qDebug() << "[FrontClient] sendSimple: type=" << serviceType << "len=" << body.size();
    return sendPacket(packet);
}

QByteArray FrontClient::sendAndReceive(int serviceType, const QByteArray& body, HEAD* outHead) {
    qDebug() << "[FrontClient] sendAndReceive START: type=" << serviceType;

    if (!sendSimple(serviceType, body)) {
        qDebug() << "[FrontClient] sendSimple FAILED";
        return {};
    }

    int max_retries = 5;
    for (int retry = 0; retry < max_retries; retry++) {
        qDebug() << "[FrontClient] Waiting (attempt" << (retry + 1) << "/" << max_retries << ")";

        int available = mainSocket.bytesAvailable();
        if (available < static_cast<int>(sizeof(HEAD))) {
            if (!mainSocket.waitForReadyRead(5000)) {
                qDebug() << "[FrontClient] Timeout";
                if (mainSocket.bytesAvailable() < static_cast<int>(sizeof(HEAD))) {
                    continue;
                }
            }
        }

        HEAD respHead = {};
        qint64 n = mainSocket.read(reinterpret_cast<char*>(&respHead), sizeof(respHead));

        if (n != sizeof(respHead)) {
            qDebug() << "[FrontClient] HEAD read failed";
            continue;
        }

        qDebug() << "[FrontClient] HEAD: type=" << respHead.service_type << "len=" << respHead.data_len;

        if (respHead.data_len < 0 || respHead.data_len > 8192) {
            qDebug() << "[FrontClient] Invalid data_len";
            continue;
        }

        if (respHead.service_type != serviceType) {
            qDebug() << "[FrontClient] Type mismatch, discarding";
            QByteArray discard;
            discard.resize(respHead.data_len);
            int discardBytes = 0;

            while (discardBytes < respHead.data_len) {
                if (mainSocket.bytesAvailable() == 0) {
                    mainSocket.waitForReadyRead(5000);
                }
                int chunk = mainSocket.read(discard.data() + discardBytes, respHead.data_len - discardBytes);
                if (chunk <= 0) break;
                discardBytes += chunk;
            }

            if (discardBytes != respHead.data_len) {
                return {};
            }
            continue;
        }

        QByteArray respBody;
        respBody.resize(respHead.data_len);
        int readBytes = 0;
        while (readBytes < respHead.data_len) {
            if (mainSocket.bytesAvailable() == 0) {
                if (!mainSocket.waitForReadyRead(5000)) {
                    break;
                }
            }

            int chunk = mainSocket.read(respBody.data() + readBytes, respHead.data_len - readBytes);
            if (chunk <= 0) break;
            readBytes += chunk;
        }

        if (readBytes != respHead.data_len) {
            qDebug() << "[FrontClient] BODY incomplete";
            continue;
        }

        qDebug() << "[FrontClient] SUCCESS: received" << readBytes << "bytes";
        if (outHead) *outHead = respHead;
        return respBody;
    }

    qDebug() << "[FrontClient] FAILED after" << max_retries << "attempts";
    return {};
}

void FrontClient::enableMessagePush(bool enable) {
    pushEnabled = enable;
    qDebug() << "[FrontClient] Message push" << (enable ? "enabled" : "disabled");

    if (enable) {
        if (!pushSocket) {
            pushSocket = new QTcpSocket(this);
            connect(pushSocket, &QTcpSocket::readyRead, this, &FrontClient::onPushSocketReadyRead);
            connect(pushSocket, &QTcpSocket::disconnected, this, &FrontClient::onPushSocketDisconnected);
            connect(pushSocket, &QTcpSocket::connected, this, &FrontClient::onPushSocketConnected);
            connect(pushSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                    this, &FrontClient::onPushSocketError);
        }

        shouldReconnect = true;
        connectPushSocket();
    } else {
        shouldReconnect = false;
        stopReconnectTimer();

        if (pushSocket) {
            pushSocket->abort();
            emit pushConnectionStateChanged(false);
        }
    }
}

void FrontClient::setUserId(const QString& userId) {
    currentUserId = userId;
    qDebug() << "[FrontClient] User ID:" << userId;
}

void FrontClient::connectPushSocket() {
    if (!pushSocket || !pushEnabled) return;

    qDebug() << "[FrontClient] Connecting push socket";
    pushSocket->abort();
    pushSocket->connectToHost(QHostAddress(currentHost), currentPort);
}

void FrontClient::onPushSocketConnected() {
    qDebug() << "[FrontClient] Push socket connected";
    emit pushConnectionStateChanged(true);
    stopReconnectTimer();
}

void FrontClient::onPushSocketReadyRead() {
    if (!pushSocket) return;

    pushBuffer.append(pushSocket->readAll());
    qDebug() << "[FrontClient] Push data received, buffer:" << pushBuffer.size();

    processPushData();
}

void FrontClient::onPushSocketDisconnected() {
    qDebug() << "[FrontClient] Push socket disconnected";
    emit pushConnectionStateChanged(false);

    if (pushEnabled && shouldReconnect) {
        startReconnectTimer();
    }
}

void FrontClient::onPushSocketError(QAbstractSocket::SocketError error) {
    qDebug() << "[FrontClient] Push socket error:" << error;
}

void FrontClient::onReconnectTimer() {
    if (!pushSocket || !pushEnabled || !shouldReconnect) return;

    if (pushSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "[FrontClient] Reconnecting push socket";
        connectPushSocket();
    }
}

void FrontClient::processPushData() {
    while (pushBuffer.size() >= static_cast<int>(sizeof(HEAD))) {
        HEAD head;
        memcpy(&head, pushBuffer.constData(), sizeof(HEAD));

        qDebug() << "[FrontClient] Push packet: type=" << head.service_type << "len=" << head.data_len;

        if (head.data_len < 0 || head.data_len > 8192) {
            qDebug() << "[FrontClient] Invalid push data_len, clearing buffer";
            pushBuffer.clear();
            return;
        }

        int totalSize = sizeof(HEAD) + head.data_len;
        if (pushBuffer.size() < totalSize) {
            qDebug() << "[FrontClient] Incomplete push packet";
            return;
        }

        QByteArray body = pushBuffer.mid(sizeof(HEAD), head.data_len);
        pushBuffer.remove(0, totalSize);

        if (head.service_type == MESSAGE_PULL) {
            qDebug() << "[FrontClient] Message push received";

            if (body.size() >= static_cast<int>(sizeof(MESSAGE_LIST_RESP))) {
                MESSAGE_LIST_RESP* resp = reinterpret_cast<MESSAGE_LIST_RESP*>(body.data());

                qDebug() << "[FrontClient] Messages: status=" << resp->status_code
                         << "count=" << resp->message_count;

                if (resp->status_code == 0 && resp->message_count > 0) {
                    int offset = sizeof(MESSAGE_LIST_RESP);

                    for (int i = 0; i < resp->message_count; i++) {
                        if (offset + static_cast<int>(sizeof(MESSAGE_ITEM)) > body.size()) break;

                        MESSAGE_ITEM* item = reinterpret_cast<MESSAGE_ITEM*>(body.data() + offset);
                        offset += sizeof(MESSAGE_ITEM);

                        if (offset + item->content_len > body.size()) break;

                        QString senderId = QString::fromUtf8(item->sender_id, 32).trimmed();
                        QString content = QString::fromUtf8(body.data() + offset, item->content_len);
                        offset += item->content_len;

                        qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

                        qDebug() << "[FrontClient] Message from:" << senderId;
                        emit messageReceived(senderId, content, timestamp);
                    }
                }
            }
        }
    }
}

void FrontClient::startReconnectTimer() {
    if (reconnectTimer && !reconnectTimer->isActive()) {
        qDebug() << "[FrontClient] Starting reconnect timer";
        reconnectTimer->start();
    }
}

void FrontClient::stopReconnectTimer() {
    if (reconnectTimer && reconnectTimer->isActive()) {
        qDebug() << "[FrontClient] Stopping reconnect timer";
        reconnectTimer->stop();
    }
}
