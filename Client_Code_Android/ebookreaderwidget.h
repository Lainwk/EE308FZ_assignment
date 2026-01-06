#ifndef EBOOKREADERWIDGET_H
#define EBOOKREADERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

namespace Ui {
class EBookReaderWidget;
}

class EBookReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EBookReaderWidget(QWidget *parent = nullptr);
    ~EBookReaderWidget();
    
    // 加载PDF（通过页面图片）
    void loadPDFPages(const QString& title, const QString& author, 
                      const QString& pagesDir, const QString& pagePrefix, 
                      int pageCount);

signals:
    void backRequested();

private slots:
    void onBackToShelf();
    void onPreviousPage();
    void onNextPage();
    void onFirstPage();
    void onLastPage();

private:
    void loadPage(int pageNum);
    void updatePageDisplay();
    void updateNavigationButtons();
    
    Ui::EBookReaderWidget *ui;
    
    QString m_bookTitle;
    QString m_bookAuthor;
    QString m_pagesDir;
    QString m_pagePrefix;
    int m_currentPage;
    int m_totalPages;
    
    QLabel* m_pageDisplay;
    QScrollArea* m_scrollArea;
    QLabel* m_pageLabel;
    QLabel* m_titleLabel;
    QPushButton* m_btnPrev;
    QPushButton* m_btnNext;
    QPushButton* m_btnFirst;
    QPushButton* m_btnLast;
    QPushButton* m_btnBack;
};

#endif // EBOOKREADERWIDGET_H