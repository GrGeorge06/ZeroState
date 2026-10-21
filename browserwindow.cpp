#include "browserwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QWebEngineProfile>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardPaths>

BrowserWindow::BrowserWindow(bool isIncognito, QWidget *parent)
: QMainWindow(parent), isIncognitoWindow(isIncognito) {
    resize(1200, 800);
    setWindowTitle(isIncognito ? "BraveLite (Incognito)" : "BraveLite");

    adBlocker = new AdBlockerInterceptor(this);
    if (!isIncognito) {
        QWebEngineProfile::defaultProfile()->setUrlRequestInterceptor(adBlocker);
        loadPersistentData();
    }

    applyCustomTheme();

    // Top Navigation Area
    QWidget *topNav = new QWidget(this);
    QHBoxLayout *navLayout = new QHBoxLayout(topNav);
    navLayout->setContentsMargins(8, 6, 8, 4);
    navLayout->setSpacing(6);

    btnBack = new QPushButton("←", this);
    btnForward = new QPushButton("→", this);
    btnReload = new QPushButton("↻", this);

    urlBar = new QLineEdit(this);
    urlBar->setPlaceholderText("Search with DuckDuckGo or enter address");
    connect(urlBar, &QLineEdit::returnPressed, this, &BrowserWindow::onUrlEntered);

    btnAdBlock = new QPushButton("Shield: ON", this);
    btnAdBlock->setStyleSheet("background-color: #e04400; color: #fff; font-weight: bold; border-radius: 4px; padding: 4px 10px;");
    connect(btnAdBlock, &QPushButton::clicked, this, &BrowserWindow::toggleAdBlocker);

    QPushButton *btnBookmark = new QPushButton("★ Bookmark", this);
    connect(btnBookmark, &QPushButton::clicked, this, &BrowserWindow::promptAddBookmark);

    QPushButton *btnIncognito = new QPushButton("🕶 Incognito", this);
    connect(btnIncognito, &QPushButton::clicked, this, &BrowserWindow::openIncognitoWindow);

    QPushButton *btnHistory = new QPushButton("History", this);
    connect(btnHistory, &QPushButton::clicked, this, &BrowserWindow::openHistoryDialog);

    QPushButton *btnSettings = new QPushButton("⚙ Settings", this);
    connect(btnSettings, &QPushButton::clicked, this, &BrowserWindow::openSettingsDialog);

    navLayout->addWidget(btnBack);
    navLayout->addWidget(btnForward);
    navLayout->addWidget(btnReload);
    navLayout->addWidget(urlBar);
    navLayout->addWidget(btnBookmark);
    navLayout->addWidget(btnAdBlock);
    navLayout->addWidget(btnIncognito);
    navLayout->addWidget(btnHistory);
    navLayout->addWidget(btnSettings);

    // Bookmarks Toolbar
    bookmarksBar = new QToolBar("Bookmarks", this);
    bookmarksBar->setMovable(false);
    rebuildBookmarksBar();

    // Tabs Setup
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &BrowserWindow::closeTab);
    connect(tabWidget, &QTabWidget::currentChanged, this, &BrowserWindow::onTabChanged);

    connect(btnBack, &QPushButton::clicked, [this]() {
        if (auto *view = qobject_cast<WebView*>(tabWidget->currentWidget())) view->back();
    });
        connect(btnForward, &QPushButton::clicked, [this]() {
            if (auto *view = qobject_cast<WebView*>(tabWidget->currentWidget())) view->forward();
        });
            connect(btnReload, &QPushButton::clicked, [this]() {
                if (auto *view = qobject_cast<WebView*>(tabWidget->currentWidget())) view->reload();
            });

                QWidget *centralContainer = new QWidget(this);
                QVBoxLayout *mainLayout = new QVBoxLayout(centralContainer);
                mainLayout->setContentsMargins(0, 0, 0, 0);
                mainLayout->setSpacing(0);
                mainLayout->addWidget(topNav);
                mainLayout->addWidget(bookmarksBar);
                mainLayout->addWidget(tabWidget);

                setCentralWidget(centralContainer);
                addNewTab();
}

BrowserWindow::~BrowserWindow() {
    if (!isIncognitoWindow) {
        savePersistentData();
    }
}

void BrowserWindow::applyCustomTheme() {
    // Red minimalist dot for tab closing, dark Chrome feel
    setStyleSheet(R"(
        QMainWindow { background-color: #1e1f22; }
        QWidget { background-color: #1e1f22; color: #dfdfdf; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Ubuntu, sans-serif; }
        QLineEdit {
            background-color: #2b2d30;
            border: 1px solid #3e4146;
            border-radius: 14px;
            padding: 5px 12px;
            color: #ffffff;
            font-size: 13px;
        }
        QLineEdit:focus { border: 1px solid #7289da; }
        QPushButton {
            background-color: #2b2d30;
            border: 1px solid #383a40;
            border-radius: 5px;
            padding: 4px 10px;
            color: #e0e0e0;
        }
        QPushButton:hover { background-color: #35373c; }
        QToolBar {
            background-color: #242629;
            border-bottom: 1px solid #2f3136;
            padding: 2px 8px;
            spacing: 6px;
        }
        QTabBar::tab {
            background: #242629;
            color: #9aa0a6;
            padding: 6px 14px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background: #2f3136;
            color: #ffffff;
        }
        QTabBar::close-button {
            background-color: #d83c3e;
            border-radius: 6px;
            min-width: 12px;
            max-width: 12px;
            min-height: 12px;
            max-height: 12px;
            margin: 2px;
        }
        QTabBar::close-button:hover {
            background-color: #ff5252;
        }
    )");
}

QString BrowserWindow::createNewTabHtml() {
    QString timeStr = QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy • hh:mm AP");
    return QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>
                body {
                    margin: 0; background: #191a1c; color: #f0f0f0;
                    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
                    display: flex; flex-direction: column; align-items: center; justify-content: center;
                    height: 80vh; text-align: center;
                }
                h1 { font-size: 3rem; margin: 0 0 20px 0; font-weight: 500; }
                .search-box {
                    display: flex; align-items: center; background: #2b2d30;
                    border: 1px solid #3f4248; border-radius: 24px;
                    width: 540px; padding: 6px 16px; margin-bottom: 25px;
                }
                .search-box input {
                    flex: 1; background: transparent; border: none; outline: none;
                    color: white; font-size: 16px; padding: 6px 8px;
                }
                .datetime { font-size: 1.1rem; color: #8e9297; }
            </style>
        </head>
        <body>
            <h1>Welcome!</h1>
            <form action="https://duckduckgo.com/" method="GET" class="search-box">
            <input type="text" name="q" placeholder="Search DuckDuckGo or enter URL" autofocus autocomplete="off" />
            </form>
            <div class="datetime">%1</div>
            </body>
            </html>
    )").arg(timeStr);
}

void BrowserWindow::addNewTab(const QUrl &url) {
    WebView *view = new WebView(this);
    if (isIncognitoWindow) {
        QWebEngineProfile *incogProfile = new QWebEngineProfile(this);
        incogProfile->setUrlRequestInterceptor(adBlocker);
        QWebEnginePage *p = new QWebEnginePage(incogProfile, view);
        view->setPage(p);
    }

    int idx = tabWidget->addTab(view, "New Tab");
    tabWidget->setCurrentIndex(idx);

    connect(view, &QWebEngineView::titleChanged, [this, view](const QString &t) {
        int i = tabWidget->indexOf(view);
        if (i != -1) tabWidget->setTabText(i, t.isEmpty() ? "New Tab" : t.left(16));
    });

        connect(view, &QWebEngineView::urlChanged, [this, view](const QUrl &u) {
            if (tabWidget->currentWidget() == view) {
                urlBar->setText(u.toString());
            }
            if (!isIncognitoWindow && !u.isEmpty() && u.scheme() != "about" && !u.toString().startsWith("data:")) {
                historyList.append(u.toString());
                savePersistentData();
            }
        });

        if (url.isEmpty()) {
            view->setHtml(createNewTabHtml());
        } else {
            view->load(url);
        }
}

void BrowserWindow::closeTab(int index) {
    if (tabWidget->count() > 1) {
        QWidget *w = tabWidget->widget(index);
        tabWidget->removeTab(index);
        w->deleteLater();
    } else {
        close();
    }
}

QUrl BrowserWindow::parseInputUrl(const QString &input) {
    if (input.startsWith("http://") || input.startsWith("https://")) {
        return QUrl(input);
    }
    if (input.contains(".") && !input.contains(" ")) {
        return QUrl("https://" + input);
    }
    return QUrl("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(input));
}

void BrowserWindow::onUrlEntered() {
    QString raw = urlBar->text().trimmed();
    if (raw.isEmpty()) return;
    QUrl target = parseInputUrl(raw);
    if (auto *view = qobject_cast<WebView*>(tabWidget->currentWidget())) {
        view->load(target);
    }
}

void BrowserWindow::onTabChanged(int index) {
    if (auto *view = qobject_cast<WebView*>(tabWidget->widget(index))) {
        urlBar->setText(view->url().toString());
    }
}

void BrowserWindow::toggleAdBlocker() {
    bool newState = !adBlocker->isEnabled();
    adBlocker->setEnabled(newState);

    if (newState) {
        btnAdBlock->setText("Shield: ON");
        btnAdBlock->setStyleSheet("background-color: #e04400; color: #fff; font-weight: bold; border-radius: 4px; padding: 4px 10px;");
    } else {
        btnAdBlock->setText("Shield: OFF");
        btnAdBlock->setStyleSheet("background-color: #4f545c; color: #bbb; font-weight: normal; border-radius: 4px; padding: 4px 10px;");
    }
}

void BrowserWindow::openIncognitoWindow() {
    BrowserWindow *incog = new BrowserWindow(true);
    incog->show();
}

// ---------------- Bookmarks Management ----------------
void BrowserWindow::promptAddBookmark() {
    auto *view = qobject_cast<WebView*>(tabWidget->currentWidget());
    if (!view) return;

    QString currentUrl = view->url().toString();
    QString defaultTitle = view->title().isEmpty() ? "Bookmark" : view->title();

    bool okTitle = false;
    QString title = QInputDialog::getText(this, "Add Bookmark", "Name:", QLineEdit::Normal, defaultTitle, &okTitle);
    if (!okTitle || title.isEmpty()) return;

    bool okUrl = false;
    QString urlStr = QInputDialog::getText(this, "Add Bookmark", "URL:", QLineEdit::Normal, currentUrl, &okUrl);
    if (!okUrl || urlStr.isEmpty()) return;

    bookmarksList.append({title, urlStr});
    savePersistentData();
    rebuildBookmarksBar();
}

void BrowserWindow::rebuildBookmarksBar() {
    bookmarksBar->clear();
    for (int i = 0; i < bookmarksList.size(); ++i) {
        Bookmark b = bookmarksList[i];
        QPushButton *btn = new QPushButton(b.title, bookmarksBar);
        btn->setStyleSheet("QPushButton { border: none; padding: 3px 8px; border-radius: 4px; } QPushButton:hover { background-color: #35373c; }");

        // Left click loads URL
        connect(btn, &QPushButton::clicked, [this, b]() {
            if (auto *v = qobject_cast<WebView*>(tabWidget->currentWidget())) {
                v->load(QUrl(b.url));
            }
        });

        // Right click custom menu (Edit / Delete)
        btn->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(btn, &QPushButton::customContextMenuRequested, [this, i, btn](const QPoint &pos) {
            QMenu menu(btn);
            QAction *editAct = menu.addAction("Edit Bookmark");
            QAction *delAct = menu.addAction("Delete Bookmark");
            QAction *chosen = menu.exec(btn->mapToGlobal(pos));

            if (chosen == editAct) {
                bool ok = false;
                QString nTitle = QInputDialog::getText(this, "Edit Bookmark", "Name:", QLineEdit::Normal, bookmarksList[i].title, &ok);
                if (ok && !nTitle.isEmpty()) bookmarksList[i].title = nTitle;

                QString nUrl = QInputDialog::getText(this, "Edit Bookmark", "URL:", QLineEdit::Normal, bookmarksList[i].url, &ok);
                if (ok && !nUrl.isEmpty()) bookmarksList[i].url = nUrl;

                savePersistentData();
                rebuildBookmarksBar();
            } else if (chosen == delAct) {
                bookmarksList.removeAt(i);
                savePersistentData();
                rebuildBookmarksBar();
            }
        });

        bookmarksBar->addWidget(btn);
    }
}

// ---------------- History & Persistence ----------------
QString BrowserWindow::getStoragePath() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/BraveLite";
    QDir().mkpath(configDir);
    return configDir + "/browser_data.json";
}

void BrowserWindow::loadPersistentData() {
    QFile file(getStoragePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject root = doc.object();

    // Load History
    historyList.clear();
    QJsonArray histArr = root["history"].toArray();
    for (auto val : histArr) historyList.append(val.toString());

    // Load Bookmarks
    bookmarksList.clear();
    QJsonArray bmArr = root["bookmarks"].toArray();
    for (auto val : bmArr) {
        QJsonObject obj = val.toObject();
        bookmarksList.append({obj["title"].toString(), obj["url"].toString()});
    }
}

void BrowserWindow::savePersistentData() {
    if (isIncognitoWindow) return; // Never save in incognito

    QFile file(getStoragePath());
    if (!file.open(QIODevice::WriteOnly)) return;

    QJsonObject root;
    QJsonArray histArr;
    for (const QString &h : historyList) histArr.append(h);
    root["history"] = histArr;

    QJsonArray bmArr;
    for (const auto &b : bookmarksList) {
        QJsonObject obj;
        obj["title"] = b.title;
        obj["url"] = b.url;
        bmArr.append(obj);
    }
    root["bookmarks"] = bmArr;

    file.write(QJsonDocument(root).toJson());
}

void BrowserWindow::openHistoryDialog() {
    QDialog dlg(this);
    dlg.setWindowTitle("Browsing History");
    dlg.resize(500, 420);
    QVBoxLayout layout(&dlg);

    QListWidget list(&dlg);
    for (int i = historyList.size() - 1; i >= 0; --i) {
        list.addItem(historyList[i]);
    }
    layout.addWidget(&list);

    connect(&list, &QListWidget::itemDoubleClicked, [this, &dlg](QListWidgetItem *it) {
        if (auto *v = qobject_cast<WebView*>(tabWidget->currentWidget())) {
            v->load(QUrl(it->text()));
            dlg.accept();
        }
    });

    QPushButton btnClear("Clear History", &dlg);
    connect(&btnClear, &QPushButton::clicked, [&]() {
        historyList.clear();
        list.clear();
        savePersistentData();
    });
    layout.addWidget(&btnClear);
    dlg.exec();
}

void BrowserWindow::openDownloadsDialog() {
    QDialog dlg(this);
    dlg.setWindowTitle("Downloads");
    dlg.resize(400, 250);
    QVBoxLayout layout(&dlg);
    layout.addWidget(new QLabel("Downloads path: " + QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), &dlg));
    dlg.exec();
}

void BrowserWindow::openSettingsDialog() {
    QDialog dlg(this);
    dlg.setWindowTitle("Settings");
    dlg.resize(650, 420);

    QHBoxLayout *hLayout = new QHBoxLayout(&dlg);
    QListWidget *sidebar = new QListWidget(&dlg);
    sidebar->setMaximumWidth(220);

    sidebar->addItem("Autofill and passwords");
    sidebar->addItem("Languages");
    sidebar->addItem("Downloads");
    sidebar->addItem("Accessibility");
    sidebar->addItem("System");
    sidebar->addItem("Reset settings");

    QStackedWidget *pages = new QStackedWidget(&dlg);

    auto addPage = [&](const QString &txt) {
        QWidget *p = new QWidget();
        QVBoxLayout *l = new QVBoxLayout(p);
        l->addWidget(new QLabel(txt, p));
        l->addStretch();
        pages->addWidget(p);
    };

    addPage("Autofill and passwords\n\n• Manage logins\n• Payment methods");
    addPage("Languages\n\n• Preferred: English (US)\n• Spelling dictionary: Active");
    addPage("Downloads\n\n• Location: ~/Downloads\n• Ask download location: Disabled");
    addPage("Accessibility\n\n• High contrast: Disabled\n• Default zoom: 100%");
    addPage("System\n\n• Default search: DuckDuckGo (Hardcoded)\n• Hardware acceleration: Enabled");

    // Reset settings page
    QWidget *pReset = new QWidget();
    QVBoxLayout *lReset = new QVBoxLayout(pReset);
    QPushButton *btnReset = new QPushButton("Restore settings to their original defaults");
    connect(btnReset, &QPushButton::clicked, [&]() {
        QFile::remove(getStoragePath());
        historyList.clear();
        bookmarksList.clear();
        rebuildBookmarksBar();
        QMessageBox::information(&dlg, "Reset", "All browsing data and bookmarks have been reset.");
    });
    lReset->addWidget(btnReset);
    lReset->addStretch();
    pages->addWidget(pReset);

    connect(sidebar, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);
    sidebar->setCurrentRow(0);

    hLayout->addWidget(sidebar);
    hLayout->addWidget(pages);
    dlg.exec();
}
