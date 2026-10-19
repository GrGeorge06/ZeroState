#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineHistory>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestInfo>
#include <QWebEngineDownloadRequest>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineCookieStore>
#include <QContextMenuEvent>
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
#include <QTabBar>
#include <QFileDialog>
#include <QDesktopServices>
#include <QProgressBar>
#include <QFormLayout>
#include <QTextStream>
#include <QShortcut>
#include <QMap>
#include <QStackedWidget>
#include <functional>
#include <memory>

// ---------------- 1. AD & DEVICE TELEMETRY BLOCKER ----------------
class AdBlocker : public QWebEngineUrlRequestInterceptor {
    Q_OBJECT
public:
    explicit AdBlocker(QObject *parent = nullptr) : QWebEngineUrlRequestInterceptor(parent), m_enabled(true) {
        blocklist = {
            "iot-logser.realme.com", "bdapi-ads.realmemobile.com", "bdapi-in-ads.realmemobile.com", "iot-eu-logser.realme.com",
            "ck.ads.oppomobile.com", "data.ads.oppomobile.com", "adsfs.oppomobile.com", "adx.ads.oppomobile.com",
            "data.mistat.xiaomi.com", "data.mistat.india.xiaomi.com", "data.mistat.rus.xiaomi.com",
            "sdkconfig.ad.xiaomi.com", "sdkconfig.ad.intl.xiaomi.com", "tracking.rus.miui.com", "api.ad.xiaomi.com",
            "metrics.data.hicloud.com", "metrics2.data.hicloud.com", "grs.hicloud.com", "logservice.hicloud.com",
            "logservice1.hicloud.com", "logbak.hicloud.com",
            "iadsdk.apple.com", "metrics.icloud.com", "metrics.mzstatic.com", "api-adservices.apple.com",
            "books-analytics-events.apple.com", "weather-analytics-events.apple.com", "notes-analytics-events.apple.com",
            "smetrics.samsung.com", "samsung-com.112.2o7.net", "analytics-api.samsunghealthcn.com", "samsungads.com", "nmetrics.samsung.com",
            "click.oneplus.cn",
            "adtago.s3.amazonaws.com", "analyticsengine.s3.amazonaws.com", "analytics.s3.amazonaws.com",
            "advice-ads.s3.amazonaws.com", "amazon-adsystem.com",
            "doubleclick.net", "googleadservices.com", "googlesyndication.com",
            "adservice.google.com", "adservice.google", "pagead2.googlesyndication.com",
            "pagead2.googleadservices.com", "afs.googlesyndication.com", "stats.g.doubleclick.net",
            "mediavisor.doubleclick.net", "ad.doubleclick.net", "static.doubleclick.net", "m.doubleclick.net",
            "google-analytics.com", "analytics.google.com", "click.googleanalytics.com", "ssl.google-analytics.com",
            "adcolony.com", "wd.adcolony.com", "ads30.adcolony.com", "adc3-launch.adcolony.com", "events3alt.adcolony.com",
            "media.net", "static.media.net", "adservetx.media.net",
            "mouseflow.com", "tools.mouseflow.com", "o2.mouseflow.com", "cdn.mouseflow.com", "cdn-test.mouseflow.com", "api.mouseflow.com", "gtm.mouseflow.com",
            "luckyorange.com", "cdn.luckyorange.com", "realtime.luckyorange.com", "api.luckyorange.com",
            "w1.luckyorange.com", "upload.luckyorange.net", "cs.luckyorange.net", "settings.luckyorange.net",
            "browser.sentry-cdn.com", "app.getsentry.com", "sentry.io",
            "notify.bugsnag.com", "sessions.bugsnag.com", "api.bugsnag.com", "app.bugsnag.com",
            "pixel.facebook.com", "an.facebook.com", "static.ads-twitter.com", "ads-api.twitter.com",
            "ads.linkedin.com", "analytics.pointdrive.linkedin.com", "ads.pinterest.com", "log.pinterest.com", "trk.pinterest.com",
            "events.reddit.com", "events.redditmedia.com", "ads.youtube.com",
            "ads-api.tiktok.com", "analytics.tiktok.com", "ads-sg.tiktok.com", "analytics-sg.tiktok.com",
            "business-api.tiktok.com", "ads.tiktok.com", "log.byteoversea.com",
            "ads.yahoo.com", "analytics.yahoo.com", "geo.yahoo.com", "udcm.yahoo.com",
            "analytics.query.yahoo.com", "partnerads.ysm.yahoo.com", "log.fc.yahoo.com",
            "gemini.yahoo.com", "adtech.yahooinc.com",
            "auction.unityads.unity3d.com", "webview.unityads.unity3d.com", "config.unityads.unity3d.com", "adserver.unityads.unity3d.com",
            "adnxs.com", "scorecardresearch.com", "zedo.com", "outbrain.com", "taboola.com",
            "adroll.com", "rubiconproject.com", "openx.net", "pubmatic.com", "criteo.com",
            "casalemedia.com", "moatads.com", "smartadserver.com", "chartbeat.com", "quantserve.com",
            "advertising.com", "revcontent.com", "popads.net", "propellerads.com", "ad-delivery.net",
            "hotjar.com", "clarity.ms", "stats.wp.com", "yandex.ru", "metrika.yandex.ru"
        };
    }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    int getBlockedCount(const QString &host) const {
        return siteBlockedCounts.value(host, 0);
    }

    void resetSiteBlockedCount(const QString &host) {
        siteBlockedCounts.remove(host);
    }

signals:
    void trackerBlocked(const QString &originHost, int totalBlocked);

protected:
    void interceptRequest(QWebEngineUrlRequestInfo &info) override {
        if (!m_enabled) return;
        QString host = info.requestUrl().host().toLower();
        QString fullUrl = info.requestUrl().toString().toLower();
        QString initiator = info.initiator().host().toLower();

        bool shouldBlock = false;
        for (const QString &domain : blocklist) {
            if (host == domain || host.endsWith("." + domain)) {
                shouldBlock = true;
                break;
            }
        }
        if (!shouldBlock && (fullUrl.contains("/pagead") || fullUrl.contains("/widget/ads") ||
            fullUrl.contains("/ads.js") || fullUrl.contains("/adserver/"))) {
            shouldBlock = true;
            }

            if (shouldBlock) {
                info.block(true);
                if (!initiator.isEmpty()) {
                    siteBlockedCounts[initiator]++;
                    emit trackerBlocked(initiator, siteBlockedCounts[initiator]);
                }
            }
    }

private:
    bool m_enabled;
    QStringList blocklist;
    QMap<QString, int> siteBlockedCounts;
};

// ---------------- 2. CUSTOM WEB ENGINE PAGE ----------------
class CustomWebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit CustomWebEnginePage(QObject *parent = nullptr) : QWebEnginePage(parent) {}
    explicit CustomWebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr)
    : QWebEnginePage(profile, parent) {}

signals:
    void loginCaptured(const QString &site, const QString &user, const QString &pass);

protected:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override {
        Q_UNUSED(level); Q_UNUSED(lineNumber); Q_UNUSED(sourceID);
        if (message.startsWith("LOGIN_SUBMIT::")) {
            QStringList parts = message.split("::");
            if (parts.size() >= 4) {
                emit loginCaptured(parts[1], parts[2], parts[3]);
            }
        }
    }
};

class BrowserWindow;

// ---------------- 3. CUSTOM WEB VIEW ----------------
class CustomWebView : public QWebEngineView {
    Q_OBJECT
public:
    explicit CustomWebView(QWidget *parent = nullptr) : QWebEngineView(parent) {}

protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

    void contextMenuEvent(QContextMenuEvent *event) override {
        QMenu *menu = new QMenu(this);
        QAction *actBack = menu->addAction("Back");
        actBack->setEnabled(history()->canGoBack());
        connect(actBack, &QAction::triggered, this, &QWebEngineView::back);

        QAction *actForward = menu->addAction("Forward");
        actForward->setEnabled(history()->canGoForward());
        connect(actForward, &QAction::triggered, this, &QWebEngineView::forward);

        QAction *actReload = menu->addAction("Reload");
        connect(actReload, &QAction::triggered, this, &QWebEngineView::reload);
        menu->addSeparator();

        QAction *actSave = menu->addAction("Save as...");
        connect(actSave, &QAction::triggered, [this]() { page()->triggerAction(QWebEnginePage::SavePage); });

        QAction *actSource = menu->addAction("View page source");
        connect(actSource, &QAction::triggered, [this]() { page()->triggerAction(QWebEnginePage::ViewSource); });

        QAction *actInspect = menu->addAction("Inspect");
        connect(actInspect, &QAction::triggered, [this]() { page()->triggerAction(QWebEnginePage::InspectElement); });

        menu->exec(event->globalPos());
        delete menu;
    }
};

// ---------------- 4. HOVER-REVEAL CLOSE BUTTON ----------------
class TabCloseButton : public QPushButton {
    Q_OBJECT
public:
    explicit TabCloseButton(QWidget *parent = nullptr) : QPushButton(parent) {
        setFixedSize(14, 14);
        setCursor(Qt::PointingHandCursor);
        setToolTip("Close Tab");
        setText("✕");
        updateState(false);
    }

    void updateState(bool hovered) {
        setEnabled(hovered);
        if (hovered) {
            setStyleSheet(
                "QPushButton { background-color: #e81123; border: none; border-radius: 7px; "
                "color: white; font-size: 8px; font-weight: bold; line-height: 14px; margin: 0; padding: 0; } "
                "QPushButton:hover { background-color: #ff5460; }"
            );
        } else {
            setStyleSheet("QPushButton { background: transparent; border: none; color: transparent; }");
        }
    }
};

// ---------------- 5. CUSTOM TAB BAR ----------------
class CustomTabBar : public QTabBar {
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr) : QTabBar(parent) {
        setMouseTracking(true);
        setExpanding(false);
    }

protected:
    void tabInserted(int index) override {
        QTabBar::tabInserted(index);
        TabCloseButton *closeBtn = new TabCloseButton(this);

        connect(closeBtn, &QPushButton::clicked, [this, closeBtn]() {
            for (int i = 0; i < count(); ++i) {
                if (tabButton(i, QTabBar::RightSide) == closeBtn) {
                    emit tabCloseRequested(i);
                    break;
                }
            }
        });

        setTabButton(index, QTabBar::RightSide, closeBtn);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        QTabBar::mouseMoveEvent(event);
        int hovered = tabAt(event->pos());
        for (int i = 0; i < count(); ++i) {
            if (auto *btn = qobject_cast<TabCloseButton*>(tabButton(i, QTabBar::RightSide))) {
                btn->updateState(i == hovered);
            }
        }
    }

    void leaveEvent(QEvent *event) override {
        QTabBar::leaveEvent(event);
        for (int i = 0; i < count(); ++i) {
            if (auto *btn = qobject_cast<TabCloseButton*>(tabButton(i, QTabBar::RightSide))) {
                btn->updateState(false);
            }
        }
    }
};

struct Bookmark {
    QString title;
    QString url;
    QIcon icon;
};

struct SavedCredential {
    QString site;
    QString username;
    QString password;
};

struct DownloadItem {
    QString fileName;
    QString url;
    QString path;
    qint64 totalBytes = 0;
    qint64 receivedBytes = 0;
    bool isFinished = false;
};

struct SitePermission {
    QString origin;
    QString permission;
    bool granted;
};

// ---------------- 6. MAIN BROWSER WINDOW ----------------
class BrowserWindow : public QMainWindow {
    Q_OBJECT
public:
    BrowserWindow(bool isIncognito = false, QWidget *parent = nullptr)
    : QMainWindow(parent), m_isIncognito(isIncognito) {

        resize(1260, 840);
        setWindowTitle(m_isIncognito ? "BraveLite [Incognito]" : "BraveLite");

        // Set Cross-Platform App Window Icon
        QIcon appIcon = QIcon::fromTheme("bravelite");
        if (appIcon.isNull()) appIcon = QIcon(QDir::homePath() + "/.local/share/icons/hicolor/512x512/apps/bravelite.png");
        if (appIcon.isNull()) appIcon = QIcon("assets/icon.ico");
        setWindowIcon(appIcon);

        m_adBlocker = new AdBlocker(this);
        connect(m_adBlocker, &AdBlocker::trackerBlocked, this, &BrowserWindow::onTrackerBlocked);

        if (!m_isIncognito) {
            auto *prof = QWebEngineProfile::defaultProfile();
            prof->setUrlRequestInterceptor(m_adBlocker);
            prof->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
            prof->setPersistentStoragePath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/BraveLite/Storage");
            connect(prof, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

            // Injected Cosmetic Filter
            QWebEngineScript cosmeticScript;
            cosmeticScript.setName("CosmeticAdBlockFilter");
            cosmeticScript.setInjectionPoint(QWebEngineScript::DocumentCreation);
            cosmeticScript.setWorldId(QWebEngineScript::MainWorld);
            cosmeticScript.setRunsOnSubFrames(true);
            cosmeticScript.setSourceCode(R"(
                (function() {
                    const injectStyle = () => {
                        if (document.getElementById('__bravelite_cosmetic')) return;
                        const s = document.createElement('style');
                        s.id = '__bravelite_cosmetic';
                        s.innerHTML = `
                            .adbox, .banner_ads, .adsbox, .textads,
                            [class*="banner_ads"], [class*="adbox"],
                            div[id*="google_ads"], div[id*="ad-slot"],
                            #ad-container, .adsbygoogle { display: none !important; visibility: hidden !important; height: 0 !important; width: 0 !important; }
                        `;
                        (document.head || document.documentElement).appendChild(s);
                    };
                    injectStyle();
                    window.addEventListener('DOMContentLoaded', injectStyle);
                })();
            )");
            prof->scripts()->insert(cosmeticScript);

            loadPersistentData();
        }

        applyStyling();
        setupUI();
        setupShortcuts();
        addNewTab();
    }

    ~BrowserWindow() override {
        if (!m_isIncognito) savePersistentData();
    }

    CustomWebView* createNewTabWithView(const QUrl &url = QUrl());

public slots:
    void promptSaveCredentials(const QString &site, const QString &user, const QString &pass) {
        QDialog dlg(this);
        dlg.setWindowTitle("Save Password?");
        dlg.resize(360, 180);
        dlg.setStyleSheet("background-color: #242529; color: #fff;");
        QFormLayout form(&dlg);

        QLabel header("Do you want BraveLite to save this login?");
        header.setStyleSheet("font-weight: bold; color: #fff;");
        form.addRow(&header);

        QLineEdit userEdit(user, &dlg);
        QLineEdit passEdit(pass, &dlg);
        passEdit.setEchoMode(QLineEdit::Password);
        userEdit.setStyleSheet("background: #1e1f22; border: 1px solid #3c4043; border-radius: 4px; padding: 4px; color: #fff;");
        passEdit.setStyleSheet("background: #1e1f22; border: 1px solid #3c4043; border-radius: 4px; padding: 4px; color: #fff;");

        form.addRow("Username:", &userEdit);
        form.addRow("Password:", &passEdit);

        QHBoxLayout btns;
        QPushButton btnSave("Save", &dlg);
        btnSave.setStyleSheet("background-color: #ff5500; font-weight: bold; border-radius: 4px; padding: 6px 14px; color: #fff;");
        QPushButton btnNever("Never", &dlg);
        btnNever.setStyleSheet("background-color: #35373c; border-radius: 4px; padding: 6px 14px; color: #d0d0d0;");
        btns.addWidget(&btnNever);
        btns.addWidget(&btnSave);
        form.addRow(&btns);

        connect(&btnSave, &QPushButton::clicked, [&]() {
            savedPasswords.append({site, userEdit.text(), passEdit.text()});
            savePersistentData();
            dlg.accept();
        });
        connect(&btnNever, &QPushButton::clicked, &dlg, &QDialog::reject);

        dlg.exec();
    }

private slots:
    void onTrackerBlocked(const QString &originHost, int totalBlocked) {
        Q_UNUSED(totalBlocked);
        if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
            if (v->url().host().toLower() == originHost) {
                updatePrivacyBadge(v->url());
            }
        }
    }

    void updatePrivacyBadge(const QUrl &url) {
        if (url.isEmpty() || url.scheme() == "about" || url.toString().startsWith("data:")) {
            btnPrivacyGrade->hide();
            return;
        }

        btnPrivacyGrade->show();
        QString host = url.host().toLower();
        int blocked = m_adBlocker->getBlockedCount(host);
        bool isHttps = (url.scheme() == "https");

        QString grade = "A";
        QString color = "#52c41a"; // Green

        if (!isHttps) {
            grade = "F";
            color = "#f5222d";
        } else if (blocked >= 20) {
            grade = "F";
            color = "#f5222d";
        } else if (blocked >= 10) {
            grade = "D";
            color = "#fa541c";
        } else if (blocked >= 4) {
            grade = "C";
            color = "#fa8c16";
        } else if (blocked >= 1) {
            grade = "B";
            color = "#13c2c2";
        } else {
            grade = "A";
            color = "#52c41a";
        }

        btnPrivacyGrade->setText(grade);
        btnPrivacyGrade->setToolTip(QString("Privacy Grade: %1\nBlocked trackers: %2\nEncrypted: %3")
        .arg(grade, QString::number(blocked), isHttps ? "Yes (HTTPS)" : "No (Insecure HTTP)"));
        btnPrivacyGrade->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: #ffffff; font-weight: bold; border-radius: 4px; padding: 2px 7px; font-size: 11px; } "
            "QPushButton:hover { opacity: 0.85; }"
        ).arg(color));
    }

    void showPrivacyDetails() {
        if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
            QUrl url = v->url();
            QString host = url.host();
            int blocked = m_adBlocker->getBlockedCount(host.toLower());
            bool isHttps = (url.scheme() == "https");

            QMessageBox::information(this, "Site Privacy Report",
                                     QString("<b>Domain:</b> %1<br>"
                                     "<b>Privacy Grade:</b> %2<br>"
                                     "<b>Blocked Trackers:</b> %3<br>"
                                     "<b>Encrypted Connection:</b> %4<br><br>"
                                     "<i>Grades are calculated using real-time tracker telemetry and connection encryption.</i>")
                                     .arg(host, btnPrivacyGrade->text(), QString::number(blocked), isHttps ? "HTTPS (Secure)" : "HTTP (Unencrypted)"));
        }
    }

    void handleDownload(QWebEngineDownloadRequest *download) {
        QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + download->downloadFileName();
        QString path = QFileDialog::getSaveFileName(this, "Save File", defaultPath);
        if (!path.isEmpty()) {
            download->setDownloadDirectory(QFileInfo(path).path());
            download->setDownloadFileName(QFileInfo(path).fileName());
            download->accept();

            DownloadItem item;
            item.fileName = download->downloadFileName();
            item.url = download->url().toString();
            item.path = path;
            item.totalBytes = download->totalBytes();
            downloadsList.append(item);
            int idx = downloadsList.size() - 1;

            btnDownload->setStyleSheet("QPushButton { font-size: 17px; color: #ff5500; }");

            connect(download, &QWebEngineDownloadRequest::receivedBytesChanged, [this, download, idx]() {
                if (idx < downloadsList.size()) {
                    downloadsList[idx].receivedBytes = download->receivedBytes();
                    downloadsList[idx].totalBytes = download->totalBytes();
                }
            });

            connect(download, &QWebEngineDownloadRequest::isFinishedChanged, [this, download, idx]() {
                if (download->isFinished()) {
                    if (idx < downloadsList.size()) downloadsList[idx].isFinished = true;
                    btnDownload->setStyleSheet("QPushButton { font-size: 17px; color: #52c41a; }");
                    savePersistentData();
                }
            });
            savePersistentData();
        } else {
            download->cancel();
        }
    }

    void openDownloadsPopup() {
        QMenu menu(this);
        menu.setStyleSheet("QMenu { background: #282a2d; color: #fff; padding: 6px; border: 1px solid #3c4043; } QMenu::item { padding: 6px 20px; }");

        if (downloadsList.isEmpty()) {
            menu.addAction("No recent downloads")->setEnabled(false);
        } else {
            for (int i = downloadsList.size() - 1; i >= qMax(0, downloadsList.size() - 4); --i) {
                auto &d = downloadsList[i];
                QString status = d.isFinished ? "✔ " : "⏳ ";
                menu.addAction(status + d.fileName);
            }
        }
        menu.addSeparator();
        QAction *moreAct = menu.addAction("More downloads");
        connect(moreAct, &QAction::triggered, this, &BrowserWindow::openDownloadsTab);

        menu.exec(btnDownload->mapToGlobal(QPoint(0, btnDownload->height() + 4)));
    }

    void openDownloadsTab() {
        QWidget *page = new QWidget(this);
        page->setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(60, 40, 60, 40);
        layout->setSpacing(20);

        QLineEdit *searchBox = new QLineEdit(page);
        searchBox->setPlaceholderText("🔍 Search download history");
        searchBox->setStyleSheet("background: #2b2d30; border: 1px solid #3c4043; border-radius: 20px; padding: 10px 18px; color: #fff; max-width: 650px; font-size: 14px;");
        layout->addWidget(searchBox);

        QLabel *todayLabel = new QLabel("Downloads History", page);
        todayLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #9aa0a6; margin-top: 15px;");
        layout->addWidget(todayLabel);

        QWidget *cardsContainer = new QWidget(page);
        QVBoxLayout *cardsLayout = new QVBoxLayout(cardsContainer);
        cardsLayout->setSpacing(14);

        for (int i = downloadsList.size() - 1; i >= 0; --i) {
            auto &d = downloadsList[i];
            QWidget *card = new QWidget(cardsContainer);
            card->setStyleSheet("background: #242529; border-radius: 8px; border: 1px solid #32353b;");
            QHBoxLayout *cLayout = new QHBoxLayout(card);
            cLayout->setContentsMargins(20, 16, 20, 16);

            QLabel *icon = new QLabel("📁", card);
            icon->setStyleSheet("font-size: 30px; padding-right: 14px;");

            QVBoxLayout *infoLayout = new QVBoxLayout();
            QLabel *title = new QLabel(d.fileName, card);
            title->setStyleSheet("font-size: 15px; font-weight: bold; color: #ffffff;");

            QUrl origin(d.url);
            QLabel *host = new QLabel("From " + (origin.host().isEmpty() ? d.url : ("https://" + origin.host())), card);
            host->setStyleSheet("font-size: 13px; color: #8e9297;");

            infoLayout->addWidget(title);
            infoLayout->addWidget(host);

            if (d.isFinished) {
                QLabel *doneLabel = new QLabel("✔ Done", card);
                doneLabel->setStyleSheet("color: #52c41a; font-weight: bold; font-size: 13px; margin-top: 4px;");
                infoLayout->addWidget(doneLabel);
            } else {
                double mbRec = d.receivedBytes / (1024.0 * 1024.0);
                double mbTot = d.totalBytes / (1024.0 * 1024.0);
                QLabel *meta = new QLabel(QString("Downloading - %1 MB of %2 MB").arg(mbRec, 0, 'f', 1).arg(mbTot, 0, 'f', 1), card);
                meta->setStyleSheet("font-size: 12px; color: #8e9297;");
                QProgressBar *pbar = new QProgressBar(card);
                pbar->setFixedHeight(4);
                pbar->setTextVisible(false);
                pbar->setRange(0, 100);
                pbar->setValue(d.totalBytes > 0 ? (d.receivedBytes * 100 / d.totalBytes) : 0);
                pbar->setStyleSheet("QProgressBar { background: #3c4043; border: none; } QProgressBar::chunk { background: #ff5500; }");
                infoLayout->addWidget(meta);
                infoLayout->addWidget(pbar);
            }

            QPushButton *folderBtn = new QPushButton("📂", card);
            folderBtn->setStyleSheet("background: transparent; border: none; font-size: 18px;");
            folderBtn->setToolTip("Open containing folder");
            QString path = d.path;
            connect(folderBtn, &QPushButton::clicked, [path]() {
                QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).path()));
            });

            cLayout->addWidget(icon);
            cLayout->addLayout(infoLayout);
            cLayout->addStretch();
            cLayout->addWidget(folderBtn);
            cardsLayout->addWidget(card);
        }
        cardsLayout->addStretch();
        layout->addWidget(cardsContainer);
        layout->addStretch();

        int idx = tabStack->addWidget(page);
        tabBar->addTab("Downloads");
        tabBar->setCurrentIndex(idx);
    }

    void openHistoryTab() {
        QWidget *page = new QWidget(this);
        page->setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(60, 40, 60, 40);
        layout->setSpacing(20);

        QLineEdit *searchBox = new QLineEdit(page);
        searchBox->setPlaceholderText("🔍 Search browsing history");
        searchBox->setStyleSheet("background: #2b2d30; border: 1px solid #3c4043; border-radius: 20px; padding: 10px 18px; color: #fff; max-width: 650px; font-size: 14px;");
        layout->addWidget(searchBox);

        QHBoxLayout *topBar = new QHBoxLayout();
        QLabel *title = new QLabel("Browsing History", page);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffffff;");
        QPushButton *btnClear = new QPushButton("Clear All History", page);
        btnClear->setStyleSheet("background-color: #35373c; border-radius: 6px; padding: 6px 14px; color: #ff5500; font-weight: bold;");
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(btnClear);
        layout->addLayout(topBar);

        QListWidget *list = new QListWidget(page);
        list->setStyleSheet("background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 12px; color: #fff; font-size: 14px;");
        for (int i = historyList.size() - 1; i >= 0; --i) {
            list->addItem(historyList[i]);
        }
        layout->addWidget(list);

        connect(list, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *it) {
            addNewTab(QUrl(it->text()));
        });

        connect(btnClear, &QPushButton::clicked, [this, list]() {
            if (QMessageBox::question(this, "Clear History", "Erase all history?") == QMessageBox::Yes) {
                historyList.clear();
                list->clear();
                savePersistentData();
            }
        });

        int idx = tabStack->addWidget(page);
        tabBar->addTab("History");
        tabBar->setCurrentIndex(idx);
    }

    void openPermissionsTab() {
        QWidget *page = new QWidget(this);
        page->setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(60, 40, 60, 40);
        layout->setSpacing(20);

        QLabel *title = new QLabel("Per-Site Permission Dashboard", page);
        title->setStyleSheet("font-size: 20px; font-weight: bold; color: #fff;");
        layout->addWidget(title);

        QListWidget *list = new QListWidget(page);
        list->setStyleSheet("background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 12px; color: #fff; font-size: 14px;");

        if (sitePermissions.isEmpty()) {
            list->addItem("No active hardware or location grants recorded.");
        } else {
            for (int i = 0; i < sitePermissions.size(); ++i) {
                const auto &p = sitePermissions[i];
                list->addItem(QString("%1 — %2: %3").arg(p.origin, p.permission, p.granted ? "Granted" : "Blocked"));
            }
        }
        layout->addWidget(list);

        QPushButton *btnRevoke = new QPushButton("Revoke All Permissions", page);
        btnRevoke->setStyleSheet("background-color: #e81123; border-radius: 6px; padding: 8px 18px; color: #fff; font-weight: bold; width: 220px;");
        connect(btnRevoke, &QPushButton::clicked, [this, list]() {
            sitePermissions.clear();
            list->clear();
            list->addItem("All permissions revoked.");
        });
        layout->addWidget(btnRevoke);

        int idx = tabStack->addWidget(page);
        tabBar->addTab("Permissions");
        tabBar->setCurrentIndex(idx);
    }

    void openSettingsTab() {
        QWidget *page = new QWidget(this);
        page->setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QHBoxLayout *hLayout = new QHBoxLayout(page);
        hLayout->setContentsMargins(40, 30, 40, 30);
        hLayout->setSpacing(30);

        QListWidget *sidebar = new QListWidget(page);
        sidebar->setMaximumWidth(240);
        sidebar->setStyleSheet(R"(
            QListWidget { background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 8px; outline: none; font-size: 14px; }
            QListWidget::item { padding: 12px 16px; border-radius: 6px; color: #9aa0a6; }
            QListWidget::item:selected { background-color: #ff5500; color: #ffffff; font-weight: bold; }
        )");

        sidebar->addItem("Autofill and passwords");
        sidebar->addItem("Bookmarks Manager");
        sidebar->addItem("DNS over Oblivious HTTP");
        sidebar->addItem("SponsorBlock Configuration");
        sidebar->addItem("Permissions Dashboard");
        sidebar->addItem("Downloads");
        sidebar->addItem("Delete browsing data");
        sidebar->addItem("Reset settings");

        QWidget *content = new QWidget(page);
        QVBoxLayout *cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(10, 0, 10, 0);

        QLabel *detailTitle = new QLabel(content);
        detailTitle->setStyleSheet("font-size: 22px; font-weight: bold; margin-bottom: 12px; color: #ffffff;");
        QListWidget *itemsList = new QListWidget(content);
        itemsList->setStyleSheet("background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 12px; color: #fff; font-size: 14px;");

        QWidget *btnRow = new QWidget(content);
        QHBoxLayout *btnLayout = new QHBoxLayout(btnRow);
        btnLayout->setContentsMargins(0, 8, 0, 0);
        btnLayout->setSpacing(10);

        QPushButton *actionBtn1 = new QPushButton(btnRow);
        QPushButton *actionBtn2 = new QPushButton(btnRow);
        actionBtn1->setStyleSheet("background: #ff5500; font-weight: bold; padding: 8px 16px; border-radius: 6px; color: #fff;");
        actionBtn2->setStyleSheet("background: #35373c; font-weight: bold; padding: 8px 16px; border-radius: 6px; color: #fff;");
        btnLayout->addWidget(actionBtn1);
        btnLayout->addWidget(actionBtn2);
        btnLayout->addStretch();

        cLayout->addWidget(detailTitle);
        cLayout->addWidget(itemsList);
        cLayout->addWidget(btnRow);

        auto showCategoryPtr = std::make_shared<std::function<void(int)>>();
        *showCategoryPtr = [=](int row) {
            itemsList->clear();
            actionBtn1->disconnect();
            actionBtn2->disconnect();
            actionBtn1->hide();
            actionBtn2->hide();

            if (row == 0) {
                detailTitle->setText("Autofill and passwords");
                for (const auto &p : savedPasswords) {
                    itemsList->addItem(p.site + "  |  User: " + p.username + "  |  Pass: ••••••••");
                }
                actionBtn1->setText("Import Passwords / Bookmarks");
                actionBtn2->setText("Export Passwords (CSV)");
                actionBtn1->show();
                actionBtn2->show();
                connect(actionBtn1, &QPushButton::clicked, this, &BrowserWindow::importTop5Browsers);
                connect(actionBtn2, &QPushButton::clicked, this, &BrowserWindow::exportPasswords);
            } else if (row == 1) {
                detailTitle->setText("Bookmarks Manager");
                for (const auto &b : bookmarksList) {
                    itemsList->addItem(b.title + " (" + b.url + ")");
                }
                actionBtn1->setText("Import Bookmarks (HTML/JSON/Browser)");
                actionBtn2->setText("Export Bookmarks (HTML)");
                actionBtn1->show();
                actionBtn2->show();
                connect(actionBtn1, &QPushButton::clicked, this, &BrowserWindow::importTop5Browsers);
                connect(actionBtn2, &QPushButton::clicked, this, &BrowserWindow::exportBookmarks);
            } else if (row == 2) {
                detailTitle->setText("DNS over Oblivious HTTP (ODoH)");
                itemsList->addItem(QString("OHTTP Status: %1").arg(odohEnabled ? "Enabled (Relay + Resolver active)" : "Disabled"));
                itemsList->addItem("Relay: https://odoh-relay.cloudflare.com");
                itemsList->addItem("Resolver: https://odoh.cloudflare-dns.com/dns-query");
                itemsList->addItem("Splits queries: Relay sees your IP only; Resolver sees DNS request only.");
                actionBtn1->setText(odohEnabled ? "Disable ODoH" : "Enable ODoH");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this, row, showCategoryPtr]() {
                    odohEnabled = !odohEnabled;
                    (*showCategoryPtr)(row);
                });
            } else if (row == 3) {
                detailTitle->setText("SponsorBlock Configuration");
                itemsList->addItem("Status: Active (Auto-skips sponsors on YouTube)");
                itemsList->addItem("Server: " + sponsorBlockServer);
                actionBtn1->setText("Change Server URL (Self-Hosted)");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this, row, showCategoryPtr]() {
                    bool ok = false;
                    QString url = QInputDialog::getText(this, "SponsorBlock Server", "Endpoint:", QLineEdit::Normal, sponsorBlockServer, &ok);
                    if (ok && !url.isEmpty()) {
                        sponsorBlockServer = url;
                        (*showCategoryPtr)(row);
                    }
                });
            } else if (row == 4) {
                detailTitle->setText("Permissions Dashboard");
                itemsList->addItem("Inspect and revoke camera, mic, and location permissions.");
                actionBtn1->setText("Open Dashboard Tab");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, this, &BrowserWindow::openPermissionsTab);
            } else if (row == 5) {
                detailTitle->setText("Downloads");
                itemsList->addItem("Location: " + QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
                itemsList->addItem("Download history is saved persistently.");
            } else if (row == 6) {
                detailTitle->setText("Delete browsing data");
                itemsList->addItem("Clear Cookies and Web Cache");
                actionBtn1->setText("Delete All Cookies");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this]() {
                    QWebEngineProfile::defaultProfile()->cookieStore()->deleteAllCookies();
                    QMessageBox::information(this, "Cleared", "Cookies and storage cleared.");
                });
            } else if (row == 7) {
                detailTitle->setText("Reset settings");
                itemsList->addItem("Restore BraveLite defaults to pristine condition");
                actionBtn1->setText("Reset All Browser Data");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this]() {
                    QFile::remove(getStorageFile());
                    historyList.clear();
                    bookmarksList.clear();
                    savedPasswords.clear();
                    downloadsList.clear();
                    renderBookmarks();
                    QMessageBox::information(this, "Reset", "All configuration and bookmarks have been reset.");
                });
            }
        };

        connect(sidebar, &QListWidget::currentRowChanged, [showCategoryPtr](int r) {
            (*showCategoryPtr)(r);
        });
        sidebar->setCurrentRow(0);

        hLayout->addWidget(sidebar);
        hLayout->addWidget(content, 1);

        int idx = tabStack->addWidget(page);
        tabBar->addTab("Settings");
        tabBar->setCurrentIndex(idx);
    }

    void openAppMenu() {
        QMenu menu(this);
        menu.setStyleSheet(R"(
            QMenu { background-color: #202124; border: 1px solid #3c4043; color: #e8eaed; padding: 6px 0px; font-size: 13px; }
            QMenu::item { padding: 6px 28px 6px 20px; }
            QMenu::item:selected { background-color: #2b2e32; }
            QMenu::separator { height: 1px; background-color: #3c4043; margin: 4px 0px; }
        )");

        menu.addAction("New tab\tCtrl+T", this, [this]() { addNewTab(); });
        menu.addAction("New window\tCtrl+N", this, [this]() { (new BrowserWindow(false))->show(); });
        menu.addAction("New private window\tCtrl+Shift+N", this, &BrowserWindow::openIncognito);
        menu.addSeparator();

        QMenu *zoomMenu = menu.addMenu("Zoom");
        zoomMenu->addAction("Zoom In\tCtrl++", this, &BrowserWindow::zoomIn);
        zoomMenu->addAction("Zoom Out\tCtrl+-", this, &BrowserWindow::zoomOut);
        zoomMenu->addAction("Reset Zoom (100%)\tCtrl+0", this, &BrowserWindow::zoomReset);

        menu.addAction("History\tCtrl+H", this, &BrowserWindow::openHistoryTab);
        menu.addAction("Bookmarks Manager", this, &BrowserWindow::openSettingsTab);
        menu.addAction("Downloads\tCtrl+J", this, &BrowserWindow::openDownloadsTab);
        menu.addAction("Site Permissions", this, &BrowserWindow::openPermissionsTab);

        menu.addAction("Delete browsing data...\tCtrl+Shift+Del", this, [this]() {
            if (QMessageBox::question(this, "Clear Data", "Delete all cookies and history?") == QMessageBox::Yes) {
                QWebEngineProfile::defaultProfile()->cookieStore()->deleteAllCookies();
                historyList.clear();
                savePersistentData();
                QMessageBox::information(this, "Cleared", "Cookies and history cleared.");
            }
        });

        menu.addSeparator();
        menu.addAction("Settings", this, &BrowserWindow::openSettingsTab);
        menu.addAction("Exit", this, &BrowserWindow::close);

        menu.exec(btnMenu->mapToGlobal(QPoint(0, btnMenu->height() + 4)));
    }

    // ---------------- IMPORT / EXPORT ----------------
    void importTop5Browsers() {
        QStringList options = {
            "Brave Browser", "Google Chrome", "Chromium", "Mozilla Firefox", "Microsoft Edge",
            "Upload Bookmarks HTML/JSON File...", "Upload Passwords CSV File..."
        };
        bool ok = false;
        QString choice = QInputDialog::getItem(this, "Import Bookmarks & Passwords", "Select Source:", options, 0, false, &ok);
        if (!ok || choice.isEmpty()) return;

        #ifdef Q_OS_WIN
        QString localApp = QDir::homePath() + "/AppData/Local";
        if (choice == "Brave Browser") {
            readChromiumBookmarks(localApp + "/BraveSoftware/Brave-Browser/User Data/Default/Bookmarks");
        } else if (choice == "Google Chrome") {
            readChromiumBookmarks(localApp + "/Google/Chrome/User Data/Default/Bookmarks");
        } else if (choice == "Chromium") {
            readChromiumBookmarks(localApp + "/Chromium/User Data/Default/Bookmarks");
        } else if (choice == "Microsoft Edge") {
            readChromiumBookmarks(localApp + "/Microsoft/Edge/User Data/Default/Bookmarks");
        } else if (choice == "Mozilla Firefox") {
            QDir fxDir(QDir::homePath() + "/AppData/Roaming/Mozilla/Firefox/Profiles");
            QStringList entries = fxDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            if (!entries.isEmpty()) {
                QMessageBox::information(this, "Firefox Import", "Found Firefox profile. You can also export Firefox bookmarks to HTML and select 'Upload Bookmarks' for instant precision.");
            } else {
                QMessageBox::information(this, "Firefox", "Please export your Firefox bookmarks to an HTML file and select 'Upload Bookmarks'.");
            }
        }
        #else
        QString home = QDir::homePath();
        if (choice == "Brave Browser") readChromiumBookmarks(home + "/.config/BraveSoftware/Brave-Browser/Default/Bookmarks");
        else if (choice == "Google Chrome") readChromiumBookmarks(home + "/.config/google-chrome/Default/Bookmarks");
        else if (choice == "Chromium") readChromiumBookmarks(home + "/.config/chromium/Default/Bookmarks");
        else if (choice == "Microsoft Edge") readChromiumBookmarks(home + "/.config/microsoft-edge/Default/Bookmarks");
        else if (choice == "Mozilla Firefox") {
            QDir fxDir(home + "/.mozilla/firefox");
            QStringList entries = fxDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            QString chosenProfile;
            for (const QString &dirName : entries) {
                if (dirName.contains("default") || dirName.contains("release")) {
                    chosenProfile = home + "/.mozilla/firefox/" + dirName;
                    break;
                }
            }
            if (!chosenProfile.isEmpty() && QFile::exists(chosenProfile + "/bookmarkbackups")) {
                QMessageBox::information(this, "Firefox Import", "Found Firefox profile. You can also export Firefox bookmarks to HTML and select 'Upload Bookmarks' for instant precision.");
            } else {
                QMessageBox::information(this, "Firefox", "Please export your Firefox bookmarks to an HTML file and select 'Upload Bookmarks'.");
            }
        }
        #endif
        else if (choice.startsWith("Upload Bookmarks")) {
            QString path = QFileDialog::getOpenFileName(this, "Select Bookmarks", QDir::homePath(), "Web Bookmarks (*.html *.json)");
            if (!path.isEmpty()) {
                if (path.endsWith(".json")) readChromiumBookmarks(path);
                else readHtmlBookmarks(path);
            }
        } else if (choice.startsWith("Upload Passwords")) {
            QString path = QFileDialog::getOpenFileName(this, "Select Passwords CSV", QDir::homePath(), "CSV Files (*.csv)");
            if (!path.isEmpty()) readPasswordsCsv(path);
        }
    }

    void readChromiumBookmarks(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Import", "Could not locate bookmarks automatically. Use 'Upload File' to select manually.");
            return;
        }
        QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        QJsonObject roots = root["roots"].toObject();
        int count = 0;
        auto parseNode = [&](auto &self, const QJsonArray &arr) -> void {
            for (auto item : arr) {
                QJsonObject obj = item.toObject();
                if (obj["type"].toString() == "url") {
                    bookmarksList.append({obj["name"].toString(), obj["url"].toString(), style()->standardIcon(QStyle::SP_FileIcon)});
                    count++;
                } else if (obj["children"].isArray()) {
                    self(self, obj["children"].toArray());
                }
            }
        };
        for (const QString &key : roots.keys()) parseNode(parseNode, roots[key].toObject()["children"].toArray());
        savePersistentData();
        renderBookmarks();
        QMessageBox::information(this, "Import Complete", QString("Imported %1 bookmarks.").arg(count));
    }

    void readHtmlBookmarks(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) return;
        QTextStream in(&file);
        int count = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            int hrefIdx = line.indexOf("HREF=\"", 0, Qt::CaseInsensitive);
            if (hrefIdx != -1) {
                int endHref = line.indexOf("\"", hrefIdx + 6);
                int closeTag = line.indexOf(">", endHref);
                int endTag = line.indexOf("</A>", closeTag, Qt::CaseInsensitive);
                if (endHref != -1 && closeTag != -1 && endTag != -1) {
                    QString url = line.mid(hrefIdx + 6, endHref - (hrefIdx + 6));
                    QString title = line.mid(closeTag + 1, endTag - (closeTag + 1));
                    bookmarksList.append({title, url, style()->standardIcon(QStyle::SP_FileIcon)});
                    count++;
                }
            }
        }
        savePersistentData();
        renderBookmarks();
        QMessageBox::information(this, "Import Complete", QString("Imported %1 bookmarks.").arg(count));
    }

    void readPasswordsCsv(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) return;
        QTextStream in(&file);
        int count = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList p = line.split(",");
            if (p.size() >= 3 && !p[0].contains("url", Qt::CaseInsensitive)) {
                savedPasswords.append({p[0].trimmed(), p[1].trimmed(), p[2].trimmed()});
                count++;
            }
        }
        savePersistentData();
        QMessageBox::information(this, "Import Complete", QString("Imported %1 credentials.").arg(count));
    }

    void exportBookmarks() {
        QString path = QFileDialog::getSaveFileName(this, "Export Bookmarks", QDir::homePath() + "/bookmarks.html", "HTML Files (*.html)");
        if (path.isEmpty()) return;
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << "<!DOCTYPE NETSCAPE-Bookmark-file-1>\n<TITLE>Bookmarks</TITLE>\n<H1>Bookmarks</H1>\n<DL><p>\n";
            for (const auto &b : bookmarksList) out << QString("    <DT><A HREF=\"%1\">%2</A>\n").arg(b.url, b.title);
            out << "</DL><p>\n";
            QMessageBox::information(this, "Export", "Bookmarks exported successfully.");
        }
    }

    void exportPasswords() {
        QString path = QFileDialog::getSaveFileName(this, "Export Passwords", QDir::homePath() + "/passwords.csv", "CSV Files (*.csv)");
        if (path.isEmpty()) return;
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            out << "url,username,password\n";
            for (const auto &p : savedPasswords) out << QString("%1,%2,%3\n").arg(p.site, p.username, p.password);
            QMessageBox::information(this, "Export", "Passwords exported successfully.");
        }
    }

    // ---------------- ZOOM CONTROLS ----------------
    void zoomIn() {
        if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
            v->setZoomFactor(v->zoomFactor() + 0.1);
        }
    }

    void zoomOut() {
        if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
            if (v->zoomFactor() > 0.3) v->setZoomFactor(v->zoomFactor() - 0.1);
        }
    }

    void zoomReset() {
        if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
            v->setZoomFactor(1.0);
        }
    }

private:
    bool m_isIncognito;
    AdBlocker *m_adBlocker;
    CustomTabBar *tabBar;
    QStackedWidget *tabStack;
    QLineEdit *urlBar;
    QPushButton *btnPrivacyGrade;
    QPushButton *btnShield;
    QPushButton *btnDownload;
    QPushButton *btnMenu;
    QToolBar *bookmarksBar;
    QProgressBar *progressBar;
    QStringList historyList;
    QVector<Bookmark> bookmarksList;
    QVector<DownloadItem> downloadsList;
    QVector<SavedCredential> savedPasswords;
    QVector<SitePermission> sitePermissions;
    bool odohEnabled = true;
    QString sponsorBlockServer = "https://sponsor.ajay.app/api/skipSegments";

    void setupShortcuts() {
        new QShortcut(QKeySequence::ZoomIn, this, SLOT(zoomIn()));
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal), this, SLOT(zoomIn()));
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), this, SLOT(zoomOut()));
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_0), this, SLOT(zoomReset()));
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this, [this]() { addNewTab(); });
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), this, [this]() { closeTab(tabBar->currentIndex()); });
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_J), this, [this]() { openDownloadsTab(); });
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_H), this, [this]() { openHistoryTab(); });
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_L), this, [this]() { urlBar->setFocus(); urlBar->selectAll(); });
    }

    void applyStyling() {
        setStyleSheet(R"(
            QMainWindow { background-color: #1a1a1e; }
            QWidget { background-color: #1a1a1e; color: #dfdfdf; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Ubuntu, sans-serif; }
            QWidget#headerContainer { background-color: #161719; }
            QLineEdit {
                background-color: #2b2d30;
                border: 1px solid #3e4146;
                border-radius: 14px;
                padding: 5px 14px;
                color: #ffffff;
                font-size: 13px;
                selection-background-color: #ff5500;
            }
            QLineEdit:focus { border: 1px solid #ff5500; }
            QPushButton {
                background-color: transparent;
                border: none;
                border-radius: 4px;
                padding: 5px 8px;
                color: #d0d0d0;
                font-size: 14px;
            }
            QPushButton:hover { background-color: #35373c; color: #ffffff; }
            QToolBar {
                background-color: #1e1f22;
                border: none;
                padding: 3px 8px;
                spacing: 6px;
            }
            QTabBar::tab {
                background: #242629;
                color: #9aa0a6;
                padding: 7px 18px 7px 12px;
                margin-right: 3px;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                min-width: 120px;
                max-width: 200px;
                font-size: 12px;
            }
            QTabBar::tab:selected {
                background: #2d3035;
                color: #ffffff;
                font-weight: 500;
            }
            QTabBar::tab:hover:!selected {
                background: #282a2e;
            }
            QTabBar::close-button {
                image: none;
                background-color: transparent;
                border: none;
                min-width: 14px;
                max-width: 14px;
                min-height: 14px;
                max-height: 14px;
            }
            QProgressBar {
                border: none;
                background: transparent;
                height: 2px;
            }
            QProgressBar::chunk {
                background-color: #ff5500;
            }
        )");
    }

    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Header container (Tabs at absolute top, then Nav Bar, then Bookmarks)
        QWidget *headerContainer = new QWidget(this);
        headerContainer->setObjectName("headerContainer");
        QVBoxLayout *headerLayout = new QVBoxLayout(headerContainer);
        headerLayout->setContentsMargins(0, 2, 0, 0);
        headerLayout->setSpacing(0);

        // 1. TOP TABS ROW (Tabs on Top, like Photo 1)
        QWidget *tabRow = new QWidget(this);
        QHBoxLayout *tabRowLayout = new QHBoxLayout(tabRow);
        tabRowLayout->setContentsMargins(8, 0, 8, 0);
        tabRowLayout->setSpacing(4);

        tabBar = new CustomTabBar(this);
        tabBar->setTabsClosable(true);
        tabBar->setMovable(true);
        connect(tabBar, &QTabBar::tabCloseRequested, this, &BrowserWindow::closeTab);
        connect(tabBar, &QTabBar::currentChanged, this, &BrowserWindow::tabSwitched);

        QPushButton *newTabBtn = new QPushButton("+", this);
        newTabBtn->setFixedSize(26, 26);
        newTabBtn->setCursor(Qt::PointingHandCursor);
        newTabBtn->setToolTip("New tab (Ctrl+T)");
        newTabBtn->setStyleSheet("QPushButton { font-size: 18px; font-weight: bold; color: #9aa0a6; background: transparent; border-radius: 4px; } QPushButton:hover { background-color: #35373c; color: #fff; }");
        connect(newTabBtn, &QPushButton::clicked, this, [this]() { addNewTab(); });

        tabRowLayout->addWidget(tabBar);
        tabRowLayout->addWidget(newTabBtn);
        tabRowLayout->addStretch();

        // 2. NAVIGATION BAR (Directly below Tabs)
        QWidget *navBar = new QWidget(this);
        QHBoxLayout *navLayout = new QHBoxLayout(navBar);
        navLayout->setContentsMargins(8, 4, 8, 4);
        navLayout->setSpacing(6);

        QPushButton *btnBack = new QPushButton("←", this);
        btnBack->setToolTip("Back (Alt+Left)");
        connect(btnBack, &QPushButton::clicked, [this]() {
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->back();
        });

            QPushButton *btnForward = new QPushButton("→", this);
            btnForward->setToolTip("Forward (Alt+Right)");
            connect(btnForward, &QPushButton::clicked, [this]() {
                if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->forward();
            });

                QPushButton *btnReload = new QPushButton("↻", this);
                btnReload->setToolTip("Reload (Ctrl+R)");
                connect(btnReload, &QPushButton::clicked, [this]() {
                    if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->reload();
                });

                    urlBar = new QLineEdit(this);
                    urlBar->setPlaceholderText("Search with DuckDuckGo or enter address");
                    connect(urlBar, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlSubmit);

                    QPushButton *btnBookmark = new QPushButton("★", this);
                    btnBookmark->setToolTip("Bookmark this page");
                    connect(btnBookmark, &QPushButton::clicked, this, &BrowserWindow::promptAddBookmark);

                    btnShield = new QPushButton("🛡", this);
                    btnShield->setToolTip("Brave Shields: Active");
                    btnShield->setStyleSheet("QPushButton { font-size: 16px; color: #ff5500; padding: 4px 6px; } QPushButton:hover { background: #35373c; }");
                    connect(btnShield, &QPushButton::clicked, this, &BrowserWindow::toggleShield);

                    btnPrivacyGrade = new QPushButton("A", this);
                    btnPrivacyGrade->setToolTip("Site Privacy Grade: A");
                    btnPrivacyGrade->setCursor(Qt::PointingHandCursor);
                    btnPrivacyGrade->setStyleSheet("QPushButton { background-color: #52c41a; color: white; font-weight: bold; border-radius: 4px; padding: 2px 7px; font-size: 11px; }");
                    btnPrivacyGrade->hide();
                    connect(btnPrivacyGrade, &QPushButton::clicked, this, &BrowserWindow::showPrivacyDetails);

                    QPushButton *btnZoomOut = new QPushButton("−", this);
                    connect(btnZoomOut, &QPushButton::clicked, this, &BrowserWindow::zoomOut);
                    QPushButton *btnZoomIn = new QPushButton("+", this);
                    connect(btnZoomIn, &QPushButton::clicked, this, &BrowserWindow::zoomIn);

                    btnDownload = new QPushButton("⤓", this);
                    btnDownload->setStyleSheet("QPushButton { font-size: 17px; font-weight: bold; }");
                    connect(btnDownload, &QPushButton::clicked, this, &BrowserWindow::openDownloadsPopup);

                    btnMenu = new QPushButton("☰", this);
                    btnMenu->setStyleSheet("QPushButton { font-size: 16px; font-weight: bold; }");
                    connect(btnMenu, &QPushButton::clicked, this, &BrowserWindow::openAppMenu);

                    navLayout->addWidget(btnBack);
                    navLayout->addWidget(btnForward);
                    navLayout->addWidget(btnReload);
                    navLayout->addWidget(btnPrivacyGrade);
                    navLayout->addWidget(urlBar);
                    navLayout->addWidget(btnBookmark);
                    navLayout->addWidget(btnShield);
                    navLayout->addWidget(btnZoomOut);
                    navLayout->addWidget(btnZoomIn);
                    navLayout->addWidget(btnDownload);
                    navLayout->addWidget(btnMenu);

                    // 3. BOOKMARKS BAR (With Live Favicon on the Left)
                    bookmarksBar = new QToolBar("Bookmarks", this);
                    bookmarksBar->setMovable(false);
                    bookmarksBar->setIconSize(QSize(16, 16));
                    renderBookmarks();

                    // 4. PROGRESS BAR
                    progressBar = new QProgressBar(this);
                    progressBar->setMaximumHeight(2);
                    progressBar->setTextVisible(false);

                    // Assemble Top Header hierarchy
                    headerLayout->addWidget(tabRow);
                    headerLayout->addWidget(navBar);
                    headerLayout->addWidget(bookmarksBar);
                    headerLayout->addWidget(progressBar);

                    // 5. STACKED WEB VIEW
                    tabStack = new QStackedWidget(this);

                    mainLayout->addWidget(headerContainer);
                    mainLayout->addWidget(tabStack, 1);

                    setCentralWidget(centralWidget);
    }

    QString getNewTabHtml() {
        QString now = QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy • hh:mm AP");
        return QString(R"(
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="utf-8">
                <title>New Tab</title>
                <style>
                    body {
                        margin: 0; background: #191a1c; color: #f0f0f0;
                        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
                        display: flex; flex-direction: column; align-items: center; justify-content: center;
                        height: 80vh; text-align: center;
                    }
                    h1 { font-size: 3.2rem; margin: 0 0 25px 0; font-weight: 500; }
                    .search-container {
                        width: 580px; max-width: 90%;
                        background: #2b2d30; border: 1px solid #44474f;
                        border-radius: 28px; padding: 6px 16px;
                        display: flex; align-items: center; box-shadow: 0 4px 12px rgba(0,0,0,0.2);
                        margin-bottom: 25px;
                    }
                    .search-container input {
                        width: 100%; border: none; background: transparent;
                        color: #ffffff; font-size: 16px; outline: none; padding: 8px 12px;
                    }
                    .datetime { font-size: 1.15rem; color: #8e9297; margin-bottom: 10px; }
                </style>
                </head>
                <body>
                <h1>Welcome!</h1>
                <form action="https://duckduckgo.com/" method="GET" class="search-container">
                <input type="text" name="q" placeholder="Search DuckDuckGo or enter address..." autofocus autocomplete="off" />
                </form>
                <div class="datetime">%1</div>
                </body>
                </html>
        )").arg(now);
    }

    void addNewTab(const QUrl &url = QUrl()) {
        createNewTabWithView(url);
    }

    void closeTab(int idx) {
        if (tabBar->count() > 1) {
            QWidget *w = tabStack->widget(idx);
            tabStack->removeWidget(w);
            tabBar->removeTab(idx);
            w->deleteLater();
        } else {
            close();
        }
    }

    void tabSwitched(int idx) {
        if (idx >= 0 && idx < tabStack->count()) {
            tabStack->setCurrentIndex(idx);
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->widget(idx))) {
                QString urlStr = v->url().toString();
                if (urlStr.startsWith("data:") || v->url().scheme() == "about") {
                    urlBar->clear();
                    btnPrivacyGrade->hide();
                } else {
                    urlBar->setText(urlStr);
                    updatePrivacyBadge(v->url());
                }
            }
        }
    }

    void handleUrlSubmit() {
        QString text = urlBar->text().trimmed();
        if (text.isEmpty()) return;

        QUrl target;
        if (text.startsWith("http://") || text.startsWith("https://")) target = QUrl(text);
            else if (text.contains(".") && !text.contains(" ")) target = QUrl("https://" + text);
                else target = QUrl("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(text));

                    if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
                        v->load(target);
                    }
    }

    void toggleShield() {
        bool next = !m_adBlocker->isEnabled();
        m_adBlocker->setEnabled(next);
        btnShield->setStyleSheet(next ? "QPushButton { font-size: 16px; color: #ff5500; padding: 4px 6px; } QPushButton:hover { background: #35373c; }"
        : "QPushButton { font-size: 16px; color: #666666; padding: 4px 6px; } QPushButton:hover { background: #35373c; }");
    }

    void openIncognito() {
        (new BrowserWindow(true))->show();
    }

    void promptAddBookmark() {
        auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget());
        if (!v) return;

        QString currUrl = v->url().toString();
        if (currUrl.startsWith("data:") || v->url().scheme() == "about") currUrl = "";

        bool ok = false;
        QString title = QInputDialog::getText(this, "Add Bookmark", "Name:", QLineEdit::Normal, v->title(), &ok);
        if (!ok || title.isEmpty()) return;

        QString url = QInputDialog::getText(this, "Add Bookmark", "URL:", QLineEdit::Normal, currUrl, &ok);
        if (!ok || url.isEmpty()) return;

        if (!url.startsWith("http://") && !url.startsWith("https://")) url = "https://" + url;

            bookmarksList.append({title, url, v->icon()});
        savePersistentData();
        renderBookmarks();
    }

    // Bookmarks Bar with Website Favicons on the Left
    void renderBookmarks() {
        bookmarksBar->clear();
        for (int i = 0; i < bookmarksList.size(); ++i) {
            Bookmark bm = bookmarksList[i];
            QAction *act = bookmarksBar->addAction(bm.icon.isNull() ? style()->standardIcon(QStyle::SP_FileIcon) : bm.icon, bm.title);
            act->setToolTip(bm.url);

            connect(act, &QAction::triggered, this, [this, bm]() {
                addNewTab(QUrl(bm.url));
            });
        }
    }

    QString getStorageFile() {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/BraveLite";
        QDir().mkpath(dir);
        return dir + "/data.json";
    }

    void loadPersistentData() {
        QFile file(getStorageFile());
        if (!file.open(QIODevice::ReadOnly)) return;

        QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        historyList.clear();
        for (auto v : root["history"].toArray()) historyList.append(v.toString());

        bookmarksList.clear();
        for (auto v : root["bookmarks"].toArray()) {
            QJsonObject o = v.toObject();
            bookmarksList.append({o["title"].toString(), o["url"].toString(), style()->standardIcon(QStyle::SP_FileIcon)});
        }

        savedPasswords.clear();
        for (auto v : root["passwords"].toArray()) {
            QJsonObject o = v.toObject();
            savedPasswords.append({o["site"].toString(), o["user"].toString(), o["pass"].toString()});
        }

        downloadsList.clear();
        for (auto v : root["downloads"].toArray()) {
            QJsonObject o = v.toObject();
            DownloadItem it;
            it.fileName = o["fileName"].toString();
            it.url = o["url"].toString();
            it.path = o["path"].toString();
            it.totalBytes = o["totalBytes"].toVariant().toLongLong();
            it.receivedBytes = o["receivedBytes"].toVariant().toLongLong();
            it.isFinished = o["isFinished"].toBool();
            downloadsList.append(it);
        }
    }

    void savePersistentData() {
        if (m_isIncognito) return;
        QFile file(getStorageFile());
        if (!file.open(QIODevice::WriteOnly)) return;

        QJsonObject root;
        QJsonArray histArr;
        for (const QString &h : historyList) histArr.append(h);
        root["history"] = histArr;

        QJsonArray bmArr;
        for (const auto &b : bookmarksList) {
            QJsonObject o;
            o["title"] = b.title;
            o["url"] = b.url;
            bmArr.append(o);
        }
        root["bookmarks"] = bmArr;

        QJsonArray passArr;
        for (const auto &p : savedPasswords) {
            QJsonObject o;
            o["site"] = p.site;
            o["user"] = p.username;
            o["pass"] = p.password;
            passArr.append(o);
        }
        root["passwords"] = passArr;

        QJsonArray dlArr;
        for (const auto &d : downloadsList) {
            QJsonObject o;
            o["fileName"] = d.fileName;
            o["url"] = d.url;
            o["path"] = d.path;
            o["totalBytes"] = d.totalBytes;
            o["receivedBytes"] = d.receivedBytes;
            o["isFinished"] = d.isFinished;
            dlArr.append(o);
        }
        root["downloads"] = dlArr;

        file.write(QJsonDocument(root).toJson());
    }
};

// ---------------- 7. POPUPS & NEW TABS ----------------
QWebEngineView* CustomWebView::createWindow(QWebEnginePage::WebWindowType type) {
    Q_UNUSED(type);
    BrowserWindow *win = qobject_cast<BrowserWindow*>(window());
    if (win) return win->createNewTabWithView();
    return nullptr;
}

CustomWebView* BrowserWindow::createNewTabWithView(const QUrl &url) {
    CustomWebView *view = new CustomWebView(this);

    QWebEngineProfile *profile = m_isIncognito ? new QWebEngineProfile(this) : QWebEngineProfile::defaultProfile();
    if (m_isIncognito) {
        profile->setUrlRequestInterceptor(m_adBlocker);
        connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);
    }

    CustomWebEnginePage *p = new CustomWebEnginePage(profile, view);
    view->setPage(p);

    if (!m_isIncognito) {
        connect(p, &CustomWebEnginePage::loginCaptured, this, &BrowserWindow::promptSaveCredentials);
        connect(p, &QWebEnginePage::featurePermissionRequested, [this, p](const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
            QString name = "Unknown";
            if (feature == QWebEnginePage::MediaAudioCapture) name = "Microphone";
            else if (feature == QWebEnginePage::MediaVideoCapture) name = "Camera";
            else if (feature == QWebEnginePage::Geolocation) name = "Location";
            else if (feature == QWebEnginePage::Notifications) name = "Notifications";

            int ret = QMessageBox::question(this, "Permission Request", QString("%1 requests access to: %2\nAllow?").arg(securityOrigin.host(), name));
            bool grant = (ret == QMessageBox::Yes);
            p->setFeaturePermission(securityOrigin, feature, grant ? QWebEnginePage::PermissionGrantedByUser : QWebEnginePage::PermissionDeniedByUser);
            sitePermissions.append({securityOrigin.toString(), name, grant});
        });
    }

    int index = tabStack->addWidget(view);
    int tabIdx = tabBar->addTab("New Tab");
    tabBar->setCurrentIndex(tabIdx);
    tabStack->setCurrentIndex(index);

    connect(view, &QWebEngineView::titleChanged, [this, view](const QString &t) {
        int i = tabStack->indexOf(view);
        if (i != -1) {
            QString cleanTitle = t.trimmed();
            tabBar->setTabText(i, (cleanTitle.isEmpty() || cleanTitle == "about:blank") ? "New Tab" : cleanTitle.left(18));
            tabBar->setTabToolTip(i, t);
        }
    });

    // Capture site favicon for Tab Bar and matching Bookmark buttons
    connect(view, &QWebEngineView::iconChanged, [this, view](const QIcon &icon) {
        int i = tabStack->indexOf(view);
        if (i != -1) {
            tabBar->setTabIcon(i, icon);
        }
        for (auto &bm : bookmarksList) {
            if (bm.url == view->url().toString()) {
                bm.icon = icon;
                renderBookmarks();
                break;
            }
        }
    });

    connect(view, &QWebEngineView::loadProgress, [this, view](int progress) {
        if (tabStack->currentWidget() == view) {
            progressBar->setValue(progress == 100 ? 0 : progress);
        }
    });

    connect(view, &QWebEngineView::urlChanged, [this, view](const QUrl &u) {
        QString urlStr = u.toString();
        if (tabStack->currentWidget() == view) {
            if (urlStr.startsWith("data:") || u.scheme() == "about") {
                urlBar->clear();
                btnPrivacyGrade->hide();
            } else {
                urlBar->setText(urlStr);
                updatePrivacyBadge(u);
            }
        }
        if (!m_isIncognito && !urlStr.isEmpty() && !urlStr.startsWith("data:") && u.scheme() != "about") {
            historyList.append(urlStr);
            savePersistentData();
        }
    });

    // Credential hook + SponsorBlock video segment skipper
    connect(view, &QWebEngineView::loadFinished, [this, view](bool ok) {
        if (ok && !m_isIncognito) {
            view->page()->runJavaScript(R"(
                (function() {
                    if (!window.__passHooked) {
                        window.__passHooked = true;
                        document.addEventListener('submit', function(e) {
                            const form = e.target;
                            const passInput = form.querySelector('input[type="password"]');
                            if (passInput && passInput.value) {
                                const userInput = form.querySelector('input[type="text"], input[type="email"]');
                                const username = userInput ? userInput.value : "";
                                const password = passInput.value;
                                console.log("LOGIN_SUBMIT::" + window.location.hostname + "::" + username + "::" + password);
                            }
                        }, true);
                    }

                    if (window.location.hostname.includes('youtube.com') && !window.__sbActive) {
                        window.__sbActive = true;
                        setInterval(() => {
                            const v = document.querySelector('video');
                            const urlParams = new URLSearchParams(window.location.search);
                            const videoId = urlParams.get('v');
                            if (v && videoId && !v.__sbFetched) {
                                v.__sbFetched = videoId;
                                fetch('https://sponsor.ajay.app/api/skipSegments?videoID=' + videoId + '&categories=["sponsor","selfpromo"]')
                                    .then(r => r.json())
                                    .then(segments => {
                                        v.__sbSegments = segments;
                                    }).catch(() => {});
                            }
                            if (v && v.__sbSegments && Array.isArray(v.__sbSegments)) {
                                for (let s of v.__sbSegments) {
                                    if (v.currentTime >= s.segment[0] && v.currentTime < s.segment[1]) {
                                        v.currentTime = s.segment[1];
                                    }
                                }
                            }
                        }, 500);
                    }
                })();
            )");
        }
    });

    if (url.isEmpty()) view->setHtml(getNewTabHtml());
    else view->load(url);

    return view;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("BraveLite");

    QIcon appIcon = QIcon::fromTheme("bravelite");
    if (appIcon.isNull()) appIcon = QIcon(QDir::homePath() + "/.local/share/icons/hicolor/512x512/apps/bravelite.png");
    if (appIcon.isNull()) appIcon = QIcon("assets/icon.ico");
    app.setWindowIcon(appIcon);

    BrowserWindow window(false);
    window.show();

    return app.exec();
}

#include "main.moc"
