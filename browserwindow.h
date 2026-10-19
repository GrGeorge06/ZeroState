#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QVector>
#include "webview.h"

struct Bookmark {
    QString title;
    QString url;
};

class BrowserWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit BrowserWindow(bool isIncognito = false, QWidget *parent = nullptr);
    ~BrowserWindow() override;

private slots:
    void addNewTab(const QUrl &url = QUrl());
    void closeTab(int index);
    void onUrlEntered();
    void onTabChanged(int index);
    void toggleAdBlocker();
    void openIncognitoWindow();
    void openHistoryDialog();
    void openDownloadsDialog();
    void openSettingsDialog();
    void promptAddBookmark();
    void rebuildBookmarksBar();

private:
    void applyCustomTheme();
    QUrl parseInputUrl(const QString &input);
    QString createNewTabHtml();

    // Persistent Storage Helpers
    void loadPersistentData();
    void savePersistentData();
    QString getStoragePath();

    QTabWidget *tabWidget;
    QLineEdit *urlBar;
    QPushButton *btnAdBlock;
    QPushButton *btnBack;
    QPushButton *btnForward;
    QPushButton *btnReload;
    QToolBar *bookmarksBar;

    AdBlockerInterceptor *adBlocker;
    bool isIncognitoWindow;
    QStringList historyList;
    QVector<Bookmark> bookmarksList;
};
