#ifndef CHANNELSPAGE_H
#define CHANNELSPAGE_H

#include <QWidget>
#include <QVector>
#include <QList>
#include "localstore.h"

namespace Ui {
class ChannelsPage;
}

class EBookReaderWidget;

class ChannelsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelsPage(QWidget *parent = nullptr);
    ~ChannelsPage();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onTabChanged(int index);
    void onBookClicked(int bookId);
    void onBackToShelf();
    void onAISendClicked();

private:
    void initTexts();
    void initCommunity();
    void initToolbox();
    void initAIChat();
    void sendAIMessage(const QString& message);
    void addUserMessage(const QString& message);
    void addAIMessage(const QString& message);
    void addSystemMessage(const QString& message);
    void loadBookShelf();
    void refreshFeed();
    bool fetchFeedFromServer();
    QVector<CommentRecord> fetchCommentsFromServer(const QString& blogId, int limit = 20);
    QWidget* buildPostWidget(const PostRecord& post, int index);
    QString formatTime(qint64 ts) const;
    SessionData currentSession() const;
    void publishCurrentText();

    Ui::ChannelsPage *ui;
    QVector<PostRecord> m_posts;
    QVector<bool> m_expanded;
    
    // 电子书相关
    EBookReaderWidget* m_ebookReader;
    QWidget* m_bookShelfWidget;
    bool m_toolboxInitialized;
};

#endif // CHANNELSPAGE_H
