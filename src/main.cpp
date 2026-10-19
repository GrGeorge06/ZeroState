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
#include <QElapsedTimer>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineFullScreenRequest>
#include <QWebEngineHistory>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestInfo>
#include <QWebEngineDownloadRequest>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineCookieStore>
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <QWebEnginePermission>
#endif
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLocalServer>
#include <QLocalSocket>
#include <QCryptographicHash>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QKeySequenceEdit>
#include <QButtonGroup>
#include <QClipboard>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
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
#include <QSet>
#include <QStackedWidget>
#include <QToolButton>
#include <QStyle>
#include <QStyleFactory>
#include <QProcess>
#include <QSettings>
#include <QScrollArea>
#include <QSysInfo>
#include <QScreen>
#include <QRegularExpression>
#include <QPointer>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <functional>
#include <memory>

#if defined(__linux__) || defined(__unix__)
#include <execinfo.h>
#include <unistd.h>
#endif

// ---------------- EXTENSIVE CRASH & DIAGNOSTIC LOGGER ----------------
namespace CrashLogger {
    static QElapsedTimer s_appUptime;
    static std::function<QStringList()> s_openTabsProvider;
    static std::function<QString()> s_activeStateProvider;

    inline QString crashLogDir() {
        QString base = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        if (base.isEmpty()) base = QDir::homePath() + "/.config";
        QString dir = base + "/BraveLite/CrashLogs";
        QDir().mkpath(dir);
        return dir;
    }

    inline QString readLinuxMemoryUsage() {
        #if defined(__linux__)
        QFile f("/proc/self/status");
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString content = QString::fromUtf8(f.readAll());
            QStringList lines = content.split('\n');
            QStringList memLines;
            for (const QString &l : lines) {
                if (l.startsWith("VmPeak:") || l.startsWith("VmSize:") ||
                    l.startsWith("VmRSS:") || l.startsWith("Threads:")) {
                    memLines.append(l.trimmed());
                    }
            }
            return memLines.join(" | ");
        }
        #endif
        return "N/A";
    }

    inline QString captureStackTrace() {
        #if defined(__linux__) || defined(__unix__)
        void *array[64];
        int size = backtrace(array, 64);
        char **messages = backtrace_symbols(array, size);
        QStringList trace;
        for (int i = 0; i < size && messages != nullptr; ++i) {
            trace.append(QString("  #%1 %2").arg(i, 2, 10, QChar('0')).arg(QString::fromUtf8(messages[i])));
        }
        free(messages);
        return trace.join("\n");
        #else
        return "  [Native stack trace unavailable on this OS build]";
        #endif
    }

    inline void writeCrashEntry(const QString &reason, const QString &details = QString()) {
        QString dir = crashLogDir();
        QString stamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss-zzz");
        QString filePath = QString("%1/crash_%2.log").arg(dir, stamp);
        QFile f(filePath);
        if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&f);
            out << "====================================================================\n";
            out << "BraveLite v1.11 Extensive Crash & Diagnostic Report\n";
            out << "====================================================================\n";
            out << "Timestamp:        " << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << "\n";
            out << "Uptime (seconds): " << (s_appUptime.isValid() ? QString::number(s_appUptime.elapsed() / 1000.0, 'f', 2) : "0") << "s\n";
            out << "Application PID:  " << QCoreApplication::applicationPid() << "\n";
            out << "OS / Kernel:      " << QSysInfo::prettyProductName() << " (" << QSysInfo::kernelType() << " " << QSysInfo::kernelVersion() << ")\n";
            out << "CPU Architecture: " << QSysInfo::currentCpuArchitecture() << "\n";
            out << "Qt Version:       " << qVersion() << "\n";
            out << "Chromium Flags:   " << QString::fromUtf8(qgetenv("QTWEBENGINE_CHROMIUM_FLAGS")) << "\n";
            out << "Memory / Threads: " << readLinuxMemoryUsage() << "\n";
            if (QGuiApplication::primaryScreen()) {
                QSize sz = QGuiApplication::primaryScreen()->size();
                out << "Primary Display:  " << sz.width() << "x" << sz.height()
                << " @ " << QGuiApplication::primaryScreen()->devicePixelRatio() << "x DPR\n";
            }
            out << "--------------------------------------------------------------------\n";
            out << "Reason:           " << reason << "\n";
            if (!details.isEmpty()) {
                out << "Details:          " << details << "\n";
            }
            if (s_activeStateProvider) {
                out << "Browser State:    " << s_activeStateProvider() << "\n";
            }
            if (s_openTabsProvider) {
                QStringList tabs = s_openTabsProvider();
                out << "Open Tabs (" << tabs.size() << "):\n";
                for (int i = 0; i < tabs.size(); ++i) {
                    out << "  [" << i << "] " << tabs[i] << "\n";
                }
            }
            out << "--------------------------------------------------------------------\n";
            out << "Call Stack Backtrace:\n" << captureStackTrace() << "\n";
            out << "====================================================================\n\n";
            f.flush();
        }
    }

    inline void posixSignalHandler(int sig) {
        QString sigName = "Unknown Signal";
        if (sig == SIGSEGV) sigName = "SIGSEGV (Segmentation Fault)";
        else if (sig == SIGABRT) sigName = "SIGABRT (Abort)";
        else if (sig == SIGFPE)  sigName = "SIGFPE (Floating Point Exception)";
        else if (sig == SIGILL)  sigName = "SIGILL (Illegal Instruction)";
        writeCrashEntry("Fatal Signal Received", QString("Signal %1: %2").arg(sig).arg(sigName));
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }

    inline void qtMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg) {
        if (type == QtFatalMsg) {
            writeCrashEntry("QtFatalMsg",
                            QString("%1 (%2:%3, %4)").arg(msg,
                                                          ctx.file ? ctx.file : "unknown",
                                                          QString::number(ctx.line),
                                                          ctx.function ? ctx.function : "unknown"));
        }
    }

    inline void installCrashHandlers() {
        s_appUptime.start();
        crashLogDir();
        std::signal(SIGSEGV, posixSignalHandler);
        std::signal(SIGABRT, posixSignalHandler);
        std::signal(SIGFPE,  posixSignalHandler);
        std::signal(SIGILL,  posixSignalHandler);
        std::set_terminate([]() {
            writeCrashEntry("Unhandled C++ Exception (std::terminate)");
            std::_Exit(EXIT_FAILURE);
        });
        qInstallMessageHandler(qtMessageHandler);
    }
}

// ---------------- 1. AD, SPAM REDIRECT & DEVICE TELEMETRY BLOCKER ----------------
class AdBlocker : public QWebEngineUrlRequestInterceptor {
    Q_OBJECT
public:
    explicit AdBlocker(QObject *parent = nullptr) : QWebEngineUrlRequestInterceptor(parent), m_enabled(true) {
        for (const char *d : {
            "freshmarketer.com", "claritybt.freshmarketer.com", "fwtracks.freshmarketer.com",
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
            "hotjar.com", "clarity.ms", "stats.wp.com", "yandex.ru", "metrika.yandex.ru",
            "onclickads.net", "exoclick.com", "syndication.exoclick.com", "juicyads.com",
            "trafficjunky.net", "adcash.com", "hilltopads.net", "popcash.net", "popmyads.com",
            "clickadu.com", "adsterra.com", "highperformanceformat.com", "profitablecpmrate.com",
            "pushwhy.com", "pushengage.com", "richpush.co", "evadav.com", "ero-advertising.com",
            "bidvertiser.com", "chitika.com", "infolinks.com", "viglink.com", "skimresources.com",
            "redirectvoluum.com", "bemobtrk.com", "binom.org", "voluum.com", "network.nutaku.net"
        }) {
            blockSet.insert(QString::fromLatin1(d));
        }
    }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    int getBlockedCount(const QString &host) const {
        return siteBlockedCounts.value(host, 0);
    }

    // Fast O(1) domain suffix lookup instead of linear string scanning on every network packet
    bool isHostBlocked(const QString &host) const {
        QString h = host.toLower().trimmed();
        if (h.isEmpty()) return false;
        while (!h.isEmpty()) {
            if (blockSet.contains(h)) return true;
            int dot = h.indexOf('.');
            if (dot == -1) break;
            h = h.mid(dot + 1);
        }
        return false;
    }

signals:
    void trackerBlocked(const QString &originHost, int totalBlocked);

protected:
    void interceptRequest(QWebEngineUrlRequestInfo &info) override {
        if (!m_enabled) return;
        QString host = info.requestUrl().host().toLower();
        QString fullUrl = info.requestUrl().toString().toLower();
        QString initiator = info.initiator().host().toLower();

        bool shouldBlock = isHostBlocked(host);

        if (!shouldBlock) {
            if (fullUrl.contains("pagead.js") ||
                fullUrl.contains("freshmarketer.com") ||
                fullUrl.contains("/pagead/") || fullUrl.contains("/widget/ads") ||
                fullUrl.contains("/ads.js") || fullUrl.contains("/adserver/") ||
                fullUrl.contains("googleads.g.doubleclick.net") ||
                fullUrl.contains("/api/stats/ads") ||
                fullUrl.contains("/get_midroll_info") ||
                fullUrl.contains("/youtubei/v1/player/ad_break") ||
                fullUrl.contains("pubads.g.doubleclick.net") ||
                (host.contains("youtube.com") &&
                (fullUrl.contains("&adformat=") || fullUrl.contains("?adformat=") ||
                fullUrl.contains("/pagead/lvz") || fullUrl.contains("/ptracking")))) {
                shouldBlock = true;
                }
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
    QSet<QString> blockSet;
    QMap<QString, int> siteBlockedCounts;
};

// ---------------- 2. CUSTOM WEB ENGINE PAGE ----------------
class CustomWebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit CustomWebEnginePage(QWebEngineProfile *profile, AdBlocker *blocker, QObject *parent = nullptr)
    : QWebEnginePage(profile, parent), m_blocker(blocker) {}

signals:
    void loginCaptured(const QString &site, const QString &user, const QString &pass);
    void autofillRequested(const QString &site);
    void linkHoverSummarizeRequested(const QString &url, const QString &linkText);
    void braveliteSearchRequested(const QString &query);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override {
        if (url.scheme() == "bravelite" && url.host() == "search") {
            QUrlQuery q(url);
            emit braveliteSearchRequested(q.queryItemValue("q"));
            return false;
        }

        if (m_blocker && m_blocker->isEnabled()) {
            QString targetHost = url.host().toLower();
            if (m_blocker->isHostBlocked(targetHost)) {
                return false;
            }
            if (isMainFrame && type == QWebEnginePage::NavigationTypeRedirect) {
                QString currentHost = this->url().host().toLower();
                if (!currentHost.isEmpty() && !targetHost.isEmpty() && currentHost != targetHost) {
                    if (m_blocker->isHostBlocked(targetHost) ||
                        url.toString().contains("clickid=", Qt::CaseInsensitive) ||
                        url.toString().contains("afu.php", Qt::CaseInsensitive) ||
                        url.toString().contains("popunder", Qt::CaseInsensitive)) {
                        return false;
                        }
                }
            }
        }
        return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
    }

    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override {
        Q_UNUSED(level); Q_UNUSED(lineNumber); Q_UNUSED(sourceID);
        if (message.startsWith("LOGIN_SUBMIT_JSON::")) {
            QString jsonPart = message.mid(QString("LOGIN_SUBMIT_JSON::").length());
            QJsonObject obj = QJsonDocument::fromJson(jsonPart.toUtf8()).object();
            QString site = obj["site"].toString();
            QString user = obj["user"].toString();
            QString pass = obj["pass"].toString();
            if (!site.isEmpty() && !pass.isEmpty()) {
                emit loginCaptured(site, user, pass);
            }
        } else if (message.startsWith("BRAVELITE_AUTOFILL_PROMPT::")) {
            QString site = message.mid(QString("BRAVELITE_AUTOFILL_PROMPT::").length()).trimmed();
            if (!site.isEmpty()) {
                emit autofillRequested(site);
            }
        } else if (message.startsWith("BRAVELITE_HOVER_LINK::")) {
            QString jsonPart = message.mid(QString("BRAVELITE_HOVER_LINK::").length());
            QJsonObject obj = QJsonDocument::fromJson(jsonPart.toUtf8()).object();
            QString targetUrl = obj["url"].toString();
            QString text = obj["text"].toString();
            if (!targetUrl.isEmpty()) {
                emit linkHoverSummarizeRequested(targetUrl, text);
            }
        } else if (message.startsWith("BRAVELITE_SEARCH::")) {
            QString q = message.mid(QString("BRAVELITE_SEARCH::").length()).trimmed();
            if (!q.isEmpty()) {
                emit braveliteSearchRequested(q);
            }
        }
    }

private:
    AdBlocker *m_blocker;
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
        menu->setStyleSheet("QMenu { background: #242529; color: #fff; border: 1px solid #3c4043; padding: 4px; } QMenu::item { padding: 6px 18px; border-radius: 4px; } QMenu::item:selected { background: #ff5500; }");
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

// ---------------- 4. BOOKMARK TOOLBUTTON ----------------
class BookmarkToolButton : public QToolButton {
    Q_OBJECT
public:
    explicit BookmarkToolButton(int index, QWidget *parent = nullptr)
    : QToolButton(parent), m_index(index) {}

    int bookmarkIndex() const { return m_index; }

signals:
    void middleClicked();
    void bookmarkMoved(int fromIndex, int toIndex);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragStartPos = event->pos();
            m_dragging = false;
        }
        QToolButton::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            if (!m_dragging && (event->pos() - m_dragStartPos).manhattanLength() > QApplication::startDragDistance()) {
                m_dragging = true;
                setCursor(Qt::ClosedHandCursor);
            }
            if (m_dragging && parentWidget()) {
                QPoint posInBar = parentWidget()->mapFromGlobal(event->globalPosition().toPoint());
                QWidget *target = parentWidget()->childAt(posInBar);
                if (auto *otherBtn = qobject_cast<BookmarkToolButton*>(target)) {
                    int targetIdx = otherBtn->bookmarkIndex();
                    if (targetIdx != m_index) {
                        emit bookmarkMoved(m_index, targetIdx);
                        return;
                    }
                }
            }
        }
        QToolButton::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (m_dragging) {
            m_dragging = false;
            setCursor(Qt::PointingHandCursor);
            setDown(false);
            event->accept();
            return;
        }
        if (event->button() == Qt::MiddleButton && rect().contains(event->pos())) {
            emit middleClicked();
            event->accept();
            return;
        }
        QToolButton::mouseReleaseEvent(event);
    }

private:
    int m_index;
    QPoint m_dragStartPos;
    bool m_dragging = false;
};

// ---------------- 5. HOVER-REVEAL CLOSE BUTTON ----------------
class TabCloseButton : public QPushButton {
    Q_OBJECT
public:
    explicit TabCloseButton(QWidget *parent = nullptr) : QPushButton(parent) {
        setFixedSize(16, 16);
        setCursor(Qt::PointingHandCursor);
        setToolTip("Close Tab");
        setText("✕");
        updateState(false);
    }

    void updateState(bool hovered) {
        setEnabled(hovered);
        if (hovered) {
            setStyleSheet(
                "QPushButton { background-color: #e81123; border: none; border-radius: 8px; "
                "color: white; font-size: 9px; font-weight: bold; margin: 0; padding: 0; } "
                "QPushButton:hover { background-color: #ff5460; }"
            );
        } else {
            setStyleSheet("QPushButton { background: transparent; border: none; color: transparent; }");
        }
    }
};

// ---------------- 6. CUSTOM TAB BAR ----------------
class CustomTabBar : public QTabBar {
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr) : QTabBar(parent) {
        setMouseTracking(true);
        setExpanding(false);
    }

signals:
    void tabDetachRequested(int index, const QPoint &globalPos);

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

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragIndex = tabAt(event->pos());
            m_dragStartPos = event->pos();
            m_detachedDuringDrag = false;
        } else if (event->button() == Qt::MiddleButton) {
            int idx = tabAt(event->pos());
            if (idx != -1) {
                emit tabCloseRequested(idx);
                event->accept();
                return;
            }
        }
        QTabBar::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        int hovered = tabAt(event->pos());
        for (int i = 0; i < count(); ++i) {
            if (auto *btn = qobject_cast<TabCloseButton*>(tabButton(i, QTabBar::RightSide))) {
                btn->updateState(i == hovered);
            }
        }

        if ((event->buttons() & Qt::LeftButton) && m_dragIndex >= 0 && !m_detachedDuringDrag) {
            int verticalDist = qAbs(event->pos().y() - m_dragStartPos.y());
            if (verticalDist > 42) {
                m_detachedDuringDrag = true;
                int idx = m_dragIndex;
                m_dragIndex = -1;
                emit tabDetachRequested(idx, event->globalPosition().toPoint());
                return;
            }
        }

        QTabBar::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        m_dragIndex = -1;
        m_detachedDuringDrag = false;
        QTabBar::mouseReleaseEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        QTabBar::leaveEvent(event);
        for (int i = 0; i < count(); ++i) {
            if (auto *btn = qobject_cast<TabCloseButton*>(tabButton(i, QTabBar::RightSide))) {
                btn->updateState(false);
            }
        }
    }

private:
    int m_dragIndex = -1;
    QPoint m_dragStartPos;
    bool m_detachedDuringDrag = false;
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

class DownloadItem : public QObject {
    Q_OBJECT
public:
    explicit DownloadItem(QObject *parent = nullptr) : QObject(parent) {}

    QString fileName;
    QString url;
    QString path;
    qint64 totalBytes = 0;
    qint64 receivedBytes = 0;
    double speedBytesPerSec = 0.0;
    bool isFinished = false;
    bool isPaused = false;
    bool isCanceled = false;
    QPointer<QWebEngineDownloadRequest> request;
    QElapsedTimer speedTimer;
    qint64 lastBytes = 0;

signals:
    void updated();
};

struct SitePermission {
    QString origin;
    QString permission;
    bool granted;
};

// ---------------- 7. MAIN BROWSER WINDOW ----------------
class BrowserWindow : public QMainWindow {
    Q_OBJECT
public:
    static QList<BrowserWindow*> s_openWindows;

    BrowserWindow(bool isIncognito = false, bool openDefaultTab = true, const QList<QUrl> &startupUrls = {}, QWidget *parent = nullptr)
    : QMainWindow(parent), m_isIncognito(isIncognito), m_profile(nullptr) {

        s_openWindows.append(this);
        setAttribute(Qt::WA_DeleteOnClose, true);

        resize(1280, 840);
        setWindowTitle(m_isIncognito ? "BraveLite v1.11 [Incognito]" : "BraveLite v1.11");

        QIcon appIcon = QIcon::fromTheme("bravelite");
        if (appIcon.isNull()) appIcon = QIcon(QDir::homePath() + "/.local/share/icons/hicolor/512x512/apps/bravelite.png");
        if (appIcon.isNull()) appIcon = QIcon("assets/bravelite.ico");
        if (appIcon.isNull()) appIcon = QIcon("assets/icon.ico");
        setWindowIcon(appIcon);

        m_netManager = new QNetworkAccessManager(this);
        m_adBlocker = new AdBlocker(this);
        connect(m_adBlocker, &AdBlocker::trackerBlocked, this, &BrowserWindow::onTrackerBlocked);

        detectHardwareGpu();
        initDefaultKeybinds();
        setupBrowserProfile();

        if (!m_isIncognito) {
            loadPersistentData();
            if (aiEnabled) {
                ensureOllamaServerRunning();
            }
        }

        setupUI();
        applyThemeProfile(currentThemeProfile);
        rebuildShortcuts();

        CrashLogger::s_openTabsProvider = [this]() {
            QStringList urls;
            if (!tabStack) return urls;
            for (int i = 0; i < tabStack->count(); ++i) {
                if (auto *v = qobject_cast<CustomWebView*>(tabStack->widget(i))) {
                    urls.append(v->url().toString());
                } else {
                    urls.append("[Internal Tab: " + tabBar->tabText(i) + "]");
                }
            }
            return urls;
        };
        CrashLogger::s_activeStateProvider = [this]() {
            return QString("Theme=%1 | Shield=%2 | AI=%3 (Model=%4, GPU=%5) | SearchEngine=%6 | Incognito=%7")
            .arg(currentThemeProfile,
                 m_adBlocker->isEnabled() ? "ON" : "OFF",
                 aiEnabled ? "ON" : "OFF",
                 ollamaModel,
                 aiUseGpu ? "Enabled" : "CPU-Only",
                 defaultSearchEngine,
                 m_isIncognito ? "Yes" : "No");
        };

        if (!m_isIncognito) {
            preloadAllBookmarkIcons();
        }

        if (!startupUrls.isEmpty()) {
            for (const QUrl &u : startupUrls) {
                addNewTab(u);
            }
        } else if (openDefaultTab) {
            if (!m_isIncognito && startupContinueWhereLeftOff && !lastSessionUrls.isEmpty()) {
                for (const QString &u : lastSessionUrls) {
                    addNewTab(QUrl(u));
                }
            } else {
                addNewTab();
            }
        }
    }

    ~BrowserWindow() override {
        s_openWindows.removeAll(this);
        if (!m_isIncognito) savePersistentData();
    }

    AdBlocker* adBlocker() const { return m_adBlocker; }
    CustomTabBar* getTabBar() const { return tabBar; }

    CustomWebView* createNewTabWithView(const QUrl &url = QUrl());

    void openExternalUrlInExistingWindow(const QUrl &url) {
        addNewTab(url);
        if (isMinimized()) {
            setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        }
        show();
        raise();
        activateWindow();
    }

    void attachExistingWebView(CustomWebView *view, const QString &title, const QIcon &icon) {
        if (!view) return;
        view->setParent(this);
        bindWebViewSignals(view);

        int index = tabStack->addWidget(view);
        int tabIdx = tabBar->addTab(icon, title.isEmpty() ? "Tab" : title.left(18));
        tabBar->setCurrentIndex(tabIdx);
        tabStack->setCurrentIndex(index);
        view->show();
        activateWindow();
        raise();
    }

signals:
    void downloadAdded(DownloadItem *item);

protected:
    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::WindowStateChange) {
            if (!isFullScreen() && !m_inFullScreenTransition) {
                m_wasMaximizedBeforeFullScreen = isMaximized();
            }
        }
        QMainWindow::changeEvent(event);
    }

public slots:
    void handleTabDetach(int idx, const QPoint &globalPos) {
        QWidget *w = tabStack->widget(idx);
        CustomWebView *view = qobject_cast<CustomWebView*>(w);
        if (!view) return;

        QString tabTitle = tabBar->tabText(idx);
        QIcon tabIcon = tabBar->tabIcon(idx);

        for (BrowserWindow *otherWin : s_openWindows) {
            if (otherWin == this || otherWin->m_isIncognito != this->m_isIncognito) continue;
            QRect otherTabBarRect(otherWin->getTabBar()->mapToGlobal(QPoint(0, 0)), otherWin->getTabBar()->size());
            otherTabBarRect.adjust(-20, -25, 60, 25);
            if (otherTabBarRect.contains(globalPos)) {
                view->disconnect();
                if (view->page()) view->page()->disconnect();
                tabStack->removeWidget(view);
                tabBar->removeTab(idx);
                otherWin->attachExistingWebView(view, tabTitle, tabIcon);
                if (tabBar->count() == 0) close();
                return;
            }
        }

        if (tabBar->count() <= 1) {
            if (isMaximized()) showNormal();
            move(globalPos - QPoint(120, 20));
            return;
        }

        view->disconnect();
        if (view->page()) view->page()->disconnect();
        tabStack->removeWidget(view);
        tabBar->removeTab(idx);

        BrowserWindow *newWin = new BrowserWindow(m_isIncognito, false);
        newWin->resize(size());
        newWin->move(globalPos - QPoint(100, 20));
        newWin->show();
        newWin->attachExistingWebView(view, tabTitle, tabIcon);
    }

    void promptAutofillCredentials(const QString &site) {
        if (!autofillEnabled) return;
        auto *view = qobject_cast<CustomWebView*>(tabStack->currentWidget());
        if (!view) return;

        QString host = site.toLower();
        QVector<SavedCredential> matches;
        for (const auto &cred : savedPasswords) {
            QString savedHost = cred.site.toLower();
            if (savedHost == host || host.endsWith("." + savedHost) || savedHost.endsWith("." + host)) {
                matches.append(cred);
            }
        }
        if (matches.isEmpty()) return;

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Autofill Credentials");
        msgBox.setText("Do you want to fill credentials?");
        msgBox.setInformativeText(QString("Account: %1\nSite: %2")
        .arg(matches.first().username.isEmpty() ? "(saved password)" : matches.first().username, site));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setStyleSheet("QMessageBox { background-color: #242529; color: #fff; } QLabel { color: #fff; } QPushButton { background: #35373c; color: #fff; border-radius: 6px; padding: 6px 16px; font-weight: bold; } QPushButton:hover { background: #ff5500; }");

        if (msgBox.exec() == QMessageBox::Yes) {
            fillCredentialsIntoView(view, matches.first());
        }
    }

    void promptSaveCredentials(const QString &site, const QString &user, const QString &pass) {
        if (m_isIncognito || site.isEmpty() || pass.isEmpty()) return;

        int existingIndex = -1;
        for (int i = 0; i < savedPasswords.size(); ++i) {
            if (savedPasswords[i].site.compare(site, Qt::CaseInsensitive) == 0 &&
                savedPasswords[i].username == user) {
                if (savedPasswords[i].password == pass) {
                    return;
                }
                existingIndex = i;
            break;
                }
        }

        QDialog dlg(this);
        dlg.setWindowTitle(existingIndex != -1 ? "Update Password?" : "Save Password?");
        dlg.resize(380, 190);
        dlg.setStyleSheet("background-color: #242529; color: #fff;");
        QFormLayout form(&dlg);

        QLabel header(existingIndex != -1
        ? QString("Update saved password for %1?").arg(site)
        : QString("Do you want BraveLite to save this login for %1?").arg(site));
        header.setWordWrap(true);
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
        QPushButton btnSave(existingIndex != -1 ? "Update" : "Save", &dlg);
        btnSave.setStyleSheet("background-color: #ff5500; font-weight: bold; border-radius: 4px; padding: 6px 14px; color: #fff;");
        QPushButton btnNever("Not Now", &dlg);
        btnNever.setStyleSheet("background-color: #35373c; border-radius: 4px; padding: 6px 14px; color: #d0d0d0;");
        btns.addWidget(&btnNever);
        btns.addWidget(&btnSave);
        form.addRow(&btns);

        connect(&btnSave, &QPushButton::clicked, [&]() {
            if (existingIndex != -1) {
                savedPasswords[existingIndex].username = userEdit.text();
                savedPasswords[existingIndex].password = passEdit.text();
            } else {
                savedPasswords.append({site, userEdit.text(), passEdit.text()});
            }
            savePersistentData();
            dlg.accept();
        });
        connect(&btnNever, &QPushButton::clicked, &dlg, &QDialog::reject);

        dlg.exec();
    }

    // ---------------- BRAVELITE AI: RELIABLE HOVER LINK SUMMARIZER ----------------
    void handleLinkHoverSummarize(const QString &targetUrl, const QString &linkText) {
        if (!aiEnabled || !aiHoverSummaryEnabled) return;
        auto *view = qobject_cast<CustomWebView*>(tabStack->currentWidget());
        if (!view || !view->page()) return;

        QUrl url(targetUrl);
        if (!url.isValid() || (!url.scheme().startsWith("http"))) return;

        // Cancel any previous in-flight hover fetch so rapid mouse movements don't queue up
        if (m_activeHoverReply) {
            m_activeHoverReply->abort();
            m_activeHoverReply->deleteLater();
            m_activeHoverReply = nullptr;
        }

        QJsonObject initPayload;
        initPayload["url"] = targetUrl;
        initPayload["summary"] = QString("✨ BraveLite AI (%1) is summarizing link...").arg(ollamaModel);
        QString initJson = QString::fromUtf8(QJsonDocument(initPayload).toJson(QJsonDocument::Compact));
        view->page()->runJavaScript(
            QString("if (window.__updateBraveLiteAiTooltipObj) window.__updateBraveLiteAiTooltipObj(%1);").arg(initJson)
        );

        QNetworkRequest pageReq(url);
        pageReq.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        pageReq.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
        pageReq.setTransferTimeout(2200); // Fast 2.2s timeout so slow sites still get summarized immediately via URL/title

        QNetworkReply *pageReply = m_netManager->get(pageReq);
        m_activeHoverReply = pageReply;
        QPointer<CustomWebView> safeView(view);

        connect(pageReply, &QNetworkReply::finished, this, [this, pageReply, safeView, targetUrl, linkText]() {
            if (m_activeHoverReply == pageReply) m_activeHoverReply = nullptr;

            QString extractedTitle;
            QString extractedText;
            if (pageReply->error() == QNetworkReply::NoError) {
                QString rawHtml = QString::fromUtf8(pageReply->read(24576));
                QRegularExpression titleRe("<title[^>]*>([\\s\\S]*?)</title>", QRegularExpression::CaseInsensitiveOption);
                auto tm = titleRe.match(rawHtml);
                if (tm.hasMatch()) extractedTitle = tm.captured(1).simplified();

                rawHtml.remove(QRegularExpression("<script[^>]*>[\\s\\S]*?</script>", QRegularExpression::CaseInsensitiveOption));
                rawHtml.remove(QRegularExpression("<style[^>]*>[\\s\\S]*?</style>", QRegularExpression::CaseInsensitiveOption));
                rawHtml.remove(QRegularExpression("<[^>]+>"));
                extractedText = rawHtml.simplified().left(900);
            }
            pageReply->deleteLater();

            QString prompt = QString(
                "Give a 2-sentence summary of what this link leads to. Be direct.\n"
                "Link URL: %1\nAnchor Text: %2\nPage Title: %3\nPage Excerpt: %4"
            ).arg(targetUrl,
                  linkText.isEmpty() ? "(none)" : linkText,
                  extractedTitle.isEmpty() ? "(unknown)" : extractedTitle,
                  extractedText.isEmpty() ? "(Summarize based on URL and Anchor Text)" : extractedText);

            queryOllamaAsync(prompt, [safeView, targetUrl](const QString &aiResponse, bool ok) {
                if (!safeView || !safeView->page()) return;
                QString display = ok
                ? ("✨ BraveLite AI: " + aiResponse.trimmed())
                : ("⚠ BraveLite AI: " + aiResponse);
                QJsonObject payload;
                payload["url"] = targetUrl;
                payload["summary"] = display;
                QString jsonArg = QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));
                safeView->page()->runJavaScript(
                    QString("if (window.__updateBraveLiteAiTooltipObj) window.__updateBraveLiteAiTooltipObj(%1);").arg(jsonArg)
                );
            });
        });
    }

    // ---------------- "BraveLite Engine": CARBON COPY OF DUCKDUCKGO + GOOGLE AI OVERVIEW ----------------
    void runBraveLiteSearch(const QString &query) {
        if (query.trimmed().isEmpty()) return;
        CustomWebView *view = qobject_cast<CustomWebView*>(tabStack->currentWidget());
        if (!view) {
            view = createNewTabWithView();
        }

        urlBar->setText("bravelite://search?q=" + query);
        int i = tabStack->indexOf(view);
        if (i != -1) tabBar->setTabText(i, (query.left(12) + " at BraveLite Engine"));

        QString escapedQuery = query.toHtmlEscaped();
        QString encodedQuery = QString::fromUtf8(QUrl::toPercentEncoding(query));

        // Carbon copy of Google's AI Overview component (4-color Gemini sparkle, shimmer loader, Show more toggle, source cards)
        QString googleAiOverviewHtml = (aiEnabled && aiSearchOverviewEnabled) ? QString(
            "<div class='g-ai-wrapper' id='aiOverviewCard'>"
            "  <div class='g-ai-header'>"
            "    <div class='g-ai-title-row'>"
            "      <svg class='g-sparkle' viewBox='0 0 24 24' width='22' height='22'>"
            "        <path fill='#4285F4' d='M12 2L14.4 9.6L22 12L14.4 14.4L12 22L9.6 14.4L2 12L9.6 9.6L12 2Z'/>"
            "        <circle fill='#EA4335' cx='19' cy='5' r='2'/>"
            "        <circle fill='#FBBC04' cx='5' cy='19' r='1.5'/>"
            "        <circle fill='#34A853' cx='18' cy='19' r='1.5'/>"
            "      </svg>"
            "      <span class='g-ai-label'>AI Overview</span>"
            "      <span class='g-ai-meta'>BraveLite AI • %1 (%2)</span>"
            "    </div>"
            "  </div>"
            "  <div class='g-ai-grid'>"
            "    <div class='g-ai-main' id='aiOverviewContainer'>"
            "      <div id='aiOverviewText' class='g-ai-text'>"
            "        <div class='shimmer-line' style='width:92%;'></div>"
            "        <div class='shimmer-line' style='width:84%;'></div>"
            "        <div class='shimmer-line' style='width:68%;'></div>"
            "      </div>"
            "      <button id='aiShowMoreBtn' class='g-ai-more-btn' style='display:none;' onclick='window.__toggleAiExpand()'>Show more ▾</button>"
            "    </div>"
            "    <div class='g-ai-sources' id='aiSourcesRail'></div>"
            "  </div>"
            "  <div class='g-ai-footer'>"
            "    <span>Generative AI is local & private via Ollama (%1)</span>"
            "    <span>BraveLite Engine</span>"
            "  </div>"
            "</div>"
        ).arg(ollamaModel.toHtmlEscaped(), aiUseGpu ? "GPU Accelerated" : "CPU") : QString();

        // Carbon copy of DuckDuckGo Dark Theme UI + Navigation + Instant Results
        QString ddgSearchPageHtml = QString(
            "<!DOCTYPE html><html><head><meta charset='utf-8'>"
            "<title>%1 at BraveLite Engine</title>"
            "<style>"
            "* { box-sizing: border-box; }"
            "body { margin: 0; background: #1c1c1c; color: #eeeeee; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }"
            "a { text-decoration: none; }"
            /* DuckDuckGo Top Header */
            ".ddg-header { background: #1c1c1c; border-bottom: 1px solid #2e2e2e; padding: 14px 24px 0 24px; position: sticky; top: 0; z-index: 100; }"
            ".ddg-top-row { display: flex; align-items: center; gap: 22px; max-width: 1120px; margin-bottom: 12px; }"
            ".ddg-logo { display: flex; align-items: center; gap: 10px; font-size: 19px; font-weight: 700; color: #fff; cursor: pointer; white-space: nowrap; }"
            ".ddg-logo-badge { width: 34px; height: 34px; border-radius: 50%; background: #de5833; display: flex; align-items: center; justify-content: center; font-size: 19px; }"
            ".ddg-search-form { flex: 1; max-width: 630px; display: flex; align-items: center; background: #2b2b2b; border: 1px solid #3d3d3d; border-radius: 8px; height: 42px; padding: 0 6px 0 16px; box-shadow: 0 2px 6px rgba(0,0,0,0.25); }"
            ".ddg-search-form:focus-within { border-color: #de5833; background: #242424; }"
            ".ddg-search-form input { flex: 1; background: transparent; border: none; outline: none; color: #fff; font-size: 15px; }"
            ".ddg-clear-btn { background: transparent; border: none; color: #888; font-size: 16px; cursor: pointer; padding: 4px 8px; }"
            ".ddg-clear-btn:hover { color: #fff; }"
            ".ddg-submit-btn { background: transparent; border: none; color: #de5833; font-size: 17px; cursor: pointer; padding: 6px 10px; font-weight: bold; }"
            /* DuckDuckGo Sub-Navigation Tabs */
            ".ddg-nav-tabs { display: flex; gap: 24px; padding-left: 172px; font-size: 13.5px; }"
            ".ddg-tab { color: #aaaaaa; padding-bottom: 10px; border-bottom: 3px solid transparent; cursor: pointer; font-weight: 500; }"
            ".ddg-tab:hover { color: #ffffff; }"
            ".ddg-tab.active { color: #ffffff; border-bottom-color: #de5833; font-weight: 600; }"
            /* Main Content Area aligned with search input */
            ".ddg-main { padding: 20px 24px 60px 196px; max-width: 1060px; }"
            "@media (max-width: 860px) { .ddg-nav-tabs { padding-left: 0; } .ddg-main { padding-left: 24px; } }"
            /* Carbon Copy of Google AI Overview */
            ".g-ai-wrapper { background: linear-gradient(180deg, #1f242d 0%, #1b1e24 100%); border: 1px solid #333a48; border-radius: 16px; padding: 20px 24px 14px 24px; margin-bottom: 26px; max-width: 780px; box-shadow: 0 4px 20px rgba(0,0,0,0.3); }"
            ".g-ai-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 14px; }"
            ".g-ai-title-row { display: flex; align-items: center; gap: 10px; }"
            ".g-ai-label { font-size: 16px; font-weight: 600; color: #e8eaed; letter-spacing: 0.2px; }"
            ".g-ai-meta { font-size: 11.5px; color: #9aa0a6; background: rgba(255,255,255,0.06); padding: 3px 9px; border-radius: 12px; }"
            ".g-ai-grid { display: flex; gap: 20px; align-items: flex-start; }"
            ".g-ai-main { flex: 1; min-width: 0; }"
            ".g-ai-text { font-size: 15px; line-height: 1.65; color: #e8eaed; white-space: pre-wrap; overflow: hidden; transition: max-height 0.25s ease; }"
            ".g-ai-text.collapsed { max-height: 165px; mask-image: linear-gradient(to bottom, black 65%, transparent 100%); }"
            ".g-ai-more-btn { margin-top: 10px; width: 100%; background: #282e3a; border: 1px solid #3c4454; color: #8ab4f8; border-radius: 20px; padding: 8px 16px; font-size: 13.5px; font-weight: 600; cursor: pointer; }"
            ".g-ai-more-btn:hover { background: #313846; }"
            ".g-ai-sources { width: 210px; display: flex; flex-direction: column; gap: 8px; flex-shrink: 0; }"
            ".g-source-chip { display: block; background: #262b36; border: 1px solid #363d4c; border-radius: 10px; padding: 8px 10px; color: #e8eaed; font-size: 12px; transition: background 0.15s; }"
            ".g-source-chip:hover { background: #303746; border-color: #8ab4f8; }"
            ".g-source-title { font-weight: 600; color: #8ab4f8; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; margin-bottom: 3px; }"
            ".g-source-domain { color: #9aa0a6; font-size: 11px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }"
            ".g-ai-footer { display: flex; justify-content: space-between; margin-top: 14px; padding-top: 10px; border-top: 1px solid #2c323f; font-size: 11.5px; color: #8e9297; }"
            "@keyframes shimmer { 0% { background-position: -400px 0; } 100% { background-position: 400px 0; } }"
            ".shimmer-line { height: 14px; margin-bottom: 10px; border-radius: 6px; background: linear-gradient(90deg, #282e3b 25%, #3a4356 50%, #282e3b 75%); background-size: 800px 100%; animation: shimmer 1.5s infinite linear; }"
            /* DuckDuckGo Organic Search Results */
            ".ddg-results { max-width: 680px; }"
            ".ddg-result { padding: 14px 16px; margin: 0 -16px 10px -16px; border-radius: 8px; transition: background 0.12s; }"
            ".ddg-result:hover { background: #242424; }"
            ".ddg-site-row { display: flex; align-items: center; gap: 8px; margin-bottom: 5px; font-size: 13px; color: #b0b0b0; }"
            ".ddg-fav { width: 16px; height: 16px; border-radius: 3px; }"
            ".ddg-title { font-size: 19px; line-height: 1.3; color: #7399ff; font-weight: 500; display: block; margin-bottom: 5px; }"
            ".ddg-title:hover { text-decoration: underline; }"
            ".ddg-title:visited { color: #a986ff; }"
            ".ddg-snippet { font-size: 14px; line-height: 1.55; color: #cccccc; }"
            "</style></head><body>"
            "<div class='ddg-header'>"
            "  <div class='ddg-top-row'>"
            "    <div class='ddg-logo' onclick=\"window.location.href='https://duckduckgo.com'\">"
            "      <div class='ddg-logo-badge'>🦁</div>"
            "      <span>BraveLite Engine</span>"
            "    </div>"
            "    <form class='ddg-search-form' onsubmit=\"event.preventDefault(); console.log('BRAVELITE_SEARCH::' + document.getElementById('qInput').value);\">"
            "      <input id='qInput' type='text' value=\"%1\" placeholder='Search privately with BraveLite Engine...' autocomplete='off' />"
            "      <button type='button' class='ddg-clear-btn' onclick=\"var i=document.getElementById('qInput'); i.value=''; i.focus();\">✕</button>"
            "      <button type='submit' class='ddg-submit-btn'>🔍</button>"
            "    </form>"
            "  </div>"
            "  <div class='ddg-nav-tabs'>"
            "    <a class='ddg-tab active' href='#'>🔍 All</a>"
            "    <a class='ddg-tab' href='https://duckduckgo.com/?q=%3&iax=images&ia=images'>🖼 Images</a>"
            "    <a class='ddg-tab' href='https://duckduckgo.com/?q=%3&iax=videos&ia=videos'>🎬 Videos</a>"
            "    <a class='ddg-tab' href='https://duckduckgo.com/?q=%3&iar=news&ia=news'>📰 News</a>"
            "    <a class='ddg-tab' href='https://duckduckgo.com/?q=%3&iaxm=maps&ia=web'>🗺 Maps</a>"
            "  </div>"
            "</div>"
            "<div class='ddg-main'>"
            "  %2"
            "  <div class='ddg-results' id='webResults'>"
            "    <div class='ddg-result'><div class='ddg-snippet'>Searching BraveLite Engine for <b>%1</b>...</div></div>"
            "  </div>"
            "</div>"
            "<script>"
            "window.__aiExpanded = false;"
            "window.__toggleAiExpand = function() {"
            "  var t = document.getElementById('aiOverviewText');"
            "  var b = document.getElementById('aiShowMoreBtn');"
            "  if (!t || !b) return;"
            "  window.__aiExpanded = !window.__aiExpanded;"
            "  if (window.__aiExpanded) { t.classList.remove('collapsed'); b.textContent = 'Show less ▴'; }"
            "  else { t.classList.add('collapsed'); b.textContent = 'Show more ▾'; }"
            "};"
            "window.__setAiOverview = function(text) {"
            "  var el = document.getElementById('aiOverviewText');"
            "  var btn = document.getElementById('aiShowMoreBtn');"
            "  if (!el) return;"
            "  el.textContent = text;"
            "  if (text && text.length > 360) {"
            "    el.classList.add('collapsed');"
            "    if (btn) btn.style.display = 'block';"
            "  } else if (btn) {"
            "    btn.style.display = 'none';"
            "  }"
            "};"
            "window.__setWebResults = function(items) {"
            "  var container = document.getElementById('webResults');"
            "  var rail = document.getElementById('aiSourcesRail');"
            "  if (!container) return;"
            "  container.innerHTML = '';"
            "  if (rail) rail.innerHTML = '';"
            "  if (!items || items.length === 0) {"
            "    container.innerHTML = '<div class=\"ddg-result\"><div class=\"ddg-snippet\">No instant results returned. <a class=\"ddg-title\" href=\"https://duckduckgo.com/?q=%3\">View on DuckDuckGo ➔</a></div></div>';"
            "    return;"
            "  }"
            "  items.forEach(function(it, idx) {"
            "    var host = '';"
            "    try { host = (new URL(it.url)).hostname; } catch(e) { host = it.url; }"
            "    var fav = 'https://www.google.com/s2/favicons?sz=32&domain_url=' + encodeURIComponent(it.url);"
            "    var card = document.createElement('div');"
            "    card.setAttribute('class', 'ddg-result');"
            "    card.innerHTML = '<div class=\"ddg-site-row\"><img class=\"ddg-fav\" src=\"' + fav + '\" /><span>' + host + '</span></div>' +"
            "                     '<a class=\"ddg-title\" href=\"' + it.url + '\">' + it.title + '</a>' +"
            "                     '<div class=\"ddg-snippet\">' + it.snippet + '</div>';"
            "    container.appendChild(card);"
            "    if (rail && idx < 3) {"
            "      var chip = document.createElement('a');"
            "      chip.setAttribute('class', 'g-source-chip');"
            "      chip.href = it.url;"
            "      chip.innerHTML = '<div class=\"g-source-title\">' + it.title + '</div><div class=\"g-source-domain\">🔗 ' + host + '</div>';"
            "      rail.appendChild(chip);"
            "    }"
            "  });"
            "};"
            "</script></body></html>"
        ).arg(escapedQuery, googleAiOverviewHtml, encodedQuery);

        view->setHtml(ddgSearchPageHtml, QUrl("https://search.bravelite.local/"));

        QPointer<CustomWebView> safeView(view);

        // Fetch DuckDuckGo results first, render them immediately, AND feed snippets to Google-style AI Overview
        QUrl ddgUrl("https://html.duckduckgo.com/html/?q=" + QUrl::toPercentEncoding(query));
        QNetworkRequest req(ddgUrl);
        req.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
        QNetworkReply *reply = m_netManager->get(req);

        connect(reply, &QNetworkReply::finished, this, [this, reply, safeView, query]() {
            if (!safeView || !safeView->page()) {
                reply->deleteLater();
                return;
            }
            QJsonArray resultsArr;
            QStringList groundingContext;
            if (reply->error() == QNetworkReply::NoError) {
                QString html = QString::fromUtf8(reply->readAll());
                QRegularExpression re(
                    "<a[^>]*result__a[^>]*href=\"([^\"]+)\"[^>]*>([\\s\\S]*?)</a>[\\s\\S]*?"
                    "<a[^>]*result__snippet[^>]*>([\\s\\S]*?)</a>",
                                      QRegularExpression::CaseInsensitiveOption
                );
                auto it = re.globalMatch(html);
                int count = 0;
                while (it.hasNext() && count < 10) {
                    auto match = it.next();
                    QString rawHref = match.captured(1);
                    if (rawHref.contains("uddg=")) {
                        QUrl parsed("https://duckduckgo.com" + rawHref);
                        QUrlQuery q(parsed);
                        QString uddg = q.queryItemValue("uddg", QUrl::FullyDecoded);
                        if (!uddg.isEmpty()) rawHref = uddg;
                    }
                    QString title = match.captured(2);
                    title.remove(QRegularExpression("<[^>]+>"));
                    title = title.simplified();

                    QString snippet = match.captured(3);
                    snippet.remove(QRegularExpression("<[^>]+>"));
                    snippet = snippet.simplified();

                    if (rawHref.startsWith("http")) {
                        QJsonObject item;
                        item["url"] = rawHref;
                        item["title"] = title;
                        item["snippet"] = snippet;
                        resultsArr.append(item);
                        if (count < 4) {
                            groundingContext.append(QString("[%1] %2: %3").arg(count + 1).arg(title, snippet));
                        }
                        count++;
                    }
                }
            }
            reply->deleteLater();

            QString jsonStr = QString::fromUtf8(QJsonDocument(resultsArr).toJson(QJsonDocument::Compact));
            QTimer::singleShot(100, safeView, [safeView, jsonStr]() {
                if (safeView && safeView->page()) {
                    safeView->page()->runJavaScript(QString("if (window.__setWebResults) window.__setWebResults(%1);").arg(jsonStr));
                }
            });

            // Now generate the Google-style AI Overview grounded in the actual web results
            if (aiEnabled && aiSearchOverviewEnabled) {
                QString aiPrompt = QString(
                    "Write a Google AI Overview style answer for the query: \"%1\".\n"
                    "Start with a clear 2-sentence summary, followed by 3 concise bullet points (•) with key facts.\n"
                    "Use these live web search results for accuracy:\n%2"
                ).arg(query, groundingContext.isEmpty() ? "(No snippets available; answer from general knowledge)" : groundingContext.join("\n"));

                queryOllamaAsync(aiPrompt, [safeView](const QString &answer, bool ok) {
                    if (!safeView || !safeView->page()) return;
                    QString out = ok
                    ? answer.trimmed()
                    : ("BraveLite AI: " + answer);
                    QJsonArray arr{out};
                    QString escaped = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
                    escaped = escaped.mid(1, escaped.length() - 2);
                    safeView->page()->runJavaScript(QString("if (window.__setAiOverview) window.__setAiOverview(%1);").arg(escaped));
                });
            }
        });
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
        if (url.isEmpty() || url.scheme() == "about" || url.toString().startsWith("data:") || url.host().endsWith(".local")) {
            btnPrivacyGrade->hide();
            return;
        }

        btnPrivacyGrade->show();
        QString host = url.host().toLower();
        int blocked = m_adBlocker->getBlockedCount(host);
        bool isHttps = (url.scheme() == "https");

        QString grade = "A";
        QString color = "#52c41a";

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
        }

        btnPrivacyGrade->setText(grade);
        btnPrivacyGrade->setToolTip(QString("Privacy Grade: %1\nBlocked trackers: %2\nEncrypted: %3")
        .arg(grade, QString::number(blocked), isHttps ? "Yes (HTTPS)" : "No (Insecure HTTP)"));
        btnPrivacyGrade->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: #ffffff; font-weight: bold; border-radius: 5px; padding: 3px 8px; font-size: 11px; border: none; } "
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
                                     "<b>Blocked Trackers & Ads:</b> %3<br>"
                                     "<b>Encrypted Connection:</b> %4<br><br>"
                                     "<i>Protected by BraveLite Shield.</i>")
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

            DownloadItem *item = new DownloadItem(this);
            item->fileName = QFileInfo(path).fileName();
            item->url = download->url().toString();
            item->path = path;
            item->totalBytes = download->totalBytes();
            item->receivedBytes = download->receivedBytes();
            item->request = download;
            item->speedTimer.start();
            item->lastBytes = 0;
            downloadsList.append(item);

            btnDownload->setStyleSheet("QPushButton { background-color: #2b2d30; border: 1px solid #ff5500; border-radius: 6px; padding: 4px 10px; font-size: 14px; color: #ff5500; font-weight: bold; }");

            connect(download, &QWebEngineDownloadRequest::receivedBytesChanged, item, [item, download]() {
                qint64 nowBytes = download->receivedBytes();
                qint64 elapsedMs = item->speedTimer.elapsed();
                if (elapsedMs >= 250) {
                    qint64 deltaBytes = nowBytes - item->lastBytes;
                    item->speedBytesPerSec = (deltaBytes * 1000.0) / elapsedMs;
                    item->lastBytes = nowBytes;
                    item->speedTimer.restart();
                }
                item->receivedBytes = nowBytes;
                item->totalBytes = download->totalBytes();
                emit item->updated();
            });

            connect(download, &QWebEngineDownloadRequest::stateChanged, item, [this, item](QWebEngineDownloadRequest::DownloadState state) {
                if (state == QWebEngineDownloadRequest::DownloadCompleted) {
                    item->isFinished = true;
                    item->isPaused = false;
                    item->speedBytesPerSec = 0.0;
                    btnDownload->setStyleSheet("QPushButton { background-color: #2b2d30; border: 1px solid #52c41a; border-radius: 6px; padding: 4px 10px; font-size: 14px; color: #52c41a; font-weight: bold; }");
                    savePersistentData();
                } else if (state == QWebEngineDownloadRequest::DownloadCancelled ||
                    state == QWebEngineDownloadRequest::DownloadInterrupted) {
                    item->isCanceled = true;
                item->speedBytesPerSec = 0.0;
                savePersistentData();
                    }
                    emit item->updated();
            });

            connect(download, &QWebEngineDownloadRequest::isPausedChanged, item, [item, download]() {
                item->isPaused = download->isPaused();
                if (item->isPaused) item->speedBytesPerSec = 0.0;
                emit item->updated();
            });

            emit downloadAdded(item);
            savePersistentData();
        } else {
            download->cancel();
        }
    }

    QWidget* createDownloadCardWidget(DownloadItem *d, QWidget *parent) {
        QWidget *card = new QWidget(parent);
        card->setStyleSheet("QWidget#dlCard { background: #242529; border-radius: 8px; border: 1px solid #32353b; }");
        card->setObjectName("dlCard");
        QHBoxLayout *cLayout = new QHBoxLayout(card);
        cLayout->setContentsMargins(20, 16, 20, 16);

        QLabel *icon = new QLabel("📁", card);
        icon->setStyleSheet("font-size: 28px; padding-right: 12px; background: transparent;");

        QVBoxLayout *infoLayout = new QVBoxLayout();
        QLabel *title = new QLabel(d->fileName, card);
        title->setStyleSheet("font-size: 15px; font-weight: bold; color: #ffffff; background: transparent;");

        QUrl origin(d->url);
        QLabel *host = new QLabel("From " + (origin.host().isEmpty() ? d->url : ("https://" + origin.host())), card);
        host->setStyleSheet("font-size: 12px; color: #8e9297; background: transparent;");

        QLabel *statusLabel = new QLabel(card);
        statusLabel->setStyleSheet("font-size: 13px; font-weight: bold; margin-top: 4px; background: transparent;");

        QProgressBar *pbar = new QProgressBar(card);
        pbar->setFixedHeight(6);
        pbar->setTextVisible(false);
        pbar->setRange(0, 100);
        pbar->setStyleSheet("QProgressBar { background: #3c4043; border: none; border-radius: 3px; } QProgressBar::chunk { background: #ff5500; border-radius: 3px; }");

        infoLayout->addWidget(title);
        infoLayout->addWidget(host);
        infoLayout->addWidget(statusLabel);
        infoLayout->addWidget(pbar);

        QHBoxLayout *actionsLayout = new QHBoxLayout();
        actionsLayout->setSpacing(8);

        QPushButton *btnPauseResume = new QPushButton("Pause", card);
        QPushButton *btnCancel = new QPushButton("Cancel", card);
        QPushButton *btnCopyLink = new QPushButton("🔗 Copy Link", card);
        QPushButton *folderBtn = new QPushButton("📂 Open Folder", card);

        QString actionBtnStyle = "QPushButton { background: #35373c; border: 1px solid #484b52; border-radius: 6px; padding: 6px 12px; color: #fff; font-size: 12px; font-weight: bold; } QPushButton:hover { background: #ff5500; border-color: #ff5500; }";
        btnPauseResume->setStyleSheet(actionBtnStyle);
        btnCancel->setStyleSheet("QPushButton { background: #35373c; border: 1px solid #484b52; border-radius: 6px; padding: 6px 12px; color: #ff6b6b; font-size: 12px; font-weight: bold; } QPushButton:hover { background: #e81123; color: #fff; }");
        btnCopyLink->setStyleSheet(actionBtnStyle);
        folderBtn->setStyleSheet(actionBtnStyle);

        actionsLayout->addWidget(btnPauseResume);
        actionsLayout->addWidget(btnCancel);
        actionsLayout->addWidget(btnCopyLink);
        actionsLayout->addWidget(folderBtn);

        auto syncCardUI = [d, statusLabel, pbar, btnPauseResume, btnCancel]() {
            if (d->isFinished) {
                pbar->hide();
                btnPauseResume->hide();
                btnCancel->hide();
                double mbTot = d->totalBytes > 0 ? (d->totalBytes / (1024.0 * 1024.0)) : (d->receivedBytes / (1024.0 * 1024.0));
                statusLabel->setText(QString("✔ Done (%1 MB)").arg(mbTot, 0, 'f', 2));
                statusLabel->setStyleSheet("color: #52c41a; font-weight: bold; font-size: 13px; margin-top: 4px; background: transparent;");
            } else if (d->isCanceled) {
                pbar->hide();
                btnPauseResume->hide();
                btnCancel->hide();
                statusLabel->setText("✖ Canceled");
                statusLabel->setStyleSheet("color: #f5222d; font-weight: bold; font-size: 13px; margin-top: 4px; background: transparent;");
            } else {
                pbar->show();
                bool hasActiveReq = !d->request.isNull();
                btnPauseResume->setVisible(hasActiveReq);
                btnCancel->setVisible(hasActiveReq);
                btnPauseResume->setText(d->isPaused ? "Resume" : "Pause");

                double mbRec = d->receivedBytes / (1024.0 * 1024.0);
                double mbTot = d->totalBytes / (1024.0 * 1024.0);
                double mbSpeed = d->speedBytesPerSec / (1024.0 * 1024.0);
                QString speedStr = mbSpeed >= 0.1
                ? QString("%1 MB/s").arg(mbSpeed, 0, 'f', 2)
                : QString("%1 KB/s").arg(d->speedBytesPerSec / 1024.0, 0, 'f', 0);

                if (d->isPaused) {
                    statusLabel->setText(QString("⏸ Paused — %1 MB of %2 MB").arg(mbRec, 0, 'f', 1).arg(mbTot, 0, 'f', 1));
                    statusLabel->setStyleSheet("color: #faad14; font-weight: bold; font-size: 13px; margin-top: 4px; background: transparent;");
                } else if (d->totalBytes > 0) {
                    int pct = qBound(0, static_cast<int>((d->receivedBytes * 100) / d->totalBytes), 100);
                    pbar->setRange(0, 100);
                    pbar->setValue(pct);
                    statusLabel->setText(QString("Downloading (%1%) — %2 MB / %3 MB • %4")
                    .arg(pct).arg(mbRec, 0, 'f', 1).arg(mbTot, 0, 'f', 1).arg(speedStr));
                    statusLabel->setStyleSheet("color: #ff9c6e; font-size: 13px; margin-top: 4px; background: transparent;");
                } else {
                    pbar->setRange(0, 0);
                    statusLabel->setText(QString("Downloading — %1 MB received • %2").arg(mbRec, 0, 'f', 1).arg(speedStr));
                    statusLabel->setStyleSheet("color: #ff9c6e; font-size: 13px; margin-top: 4px; background: transparent;");
                }
            }
        };

        syncCardUI();
        connect(d, &DownloadItem::updated, card, syncCardUI);

        connect(btnPauseResume, &QPushButton::clicked, card, [d]() {
            if (!d->request) return;
            if (d->request->isPaused()) d->request->resume();
            else d->request->pause();
        });

            connect(btnCancel, &QPushButton::clicked, card, [d]() {
                if (d->request) d->request->cancel();
                else {
                    d->isCanceled = true;
                    emit d->updated();
                }
            });

            connect(btnCopyLink, &QPushButton::clicked, card, [d, btnCopyLink]() {
                QApplication::clipboard()->setText(d->url);
                btnCopyLink->setText("✔ Copied!");
            });

            QString path = d->path;
            connect(folderBtn, &QPushButton::clicked, [path]() {
                QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).path()));
            });

            cLayout->addWidget(icon);
            cLayout->addLayout(infoLayout, 1);
            cLayout->addLayout(actionsLayout);
            return card;
    }

    void openDownloadsPopup() {
        QMenu menu(this);
        menu.setStyleSheet("QMenu { background: #282a2d; color: #fff; padding: 6px; border: 1px solid #3c4043; } QMenu::item { padding: 6px 20px; } QMenu::item:selected { background: #35373c; }");

        if (downloadsList.isEmpty()) {
            menu.addAction("No recent downloads")->setEnabled(false);
        } else {
            for (int i = downloadsList.size() - 1; i >= qMax(0, (int)downloadsList.size() - 4); --i) {
                DownloadItem *d = downloadsList[i];
                QString status = d->isFinished ? "✔ " : (d->isCanceled ? "✖ " : "⏳ ");
                menu.addAction(status + d->fileName);
            }
        }
        menu.addSeparator();
        QAction *moreAct = menu.addAction("Show All Downloads (Ctrl+J)");
        connect(moreAct, &QAction::triggered, this, &BrowserWindow::openDownloadsTab);

        menu.exec(btnDownload->mapToGlobal(QPoint(0, btnDownload->height() + 4)));
    }

    void openDownloadsTab() {
        QWidget *page = new QWidget(this);
        page->setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(60, 40, 60, 40);
        layout->setSpacing(18);

        QLabel *todayLabel = new QLabel("Downloads History & Live Progress", page);
        todayLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;");
        layout->addWidget(todayLabel);

        QScrollArea *scroll = new QScrollArea(page);
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

        QWidget *cardsContainer = new QWidget(scroll);
        QVBoxLayout *cardsLayout = new QVBoxLayout(cardsContainer);
        cardsLayout->setSpacing(14);

        for (int i = downloadsList.size() - 1; i >= 0; --i) {
            cardsLayout->addWidget(createDownloadCardWidget(downloadsList[i], cardsContainer));
        }
        cardsLayout->addStretch();

        connect(this, &BrowserWindow::downloadAdded, cardsContainer, [this, cardsContainer, cardsLayout](DownloadItem *newItem) {
            cardsLayout->insertWidget(0, createDownloadCardWidget(newItem, cardsContainer));
        });

        scroll->setWidget(cardsContainer);
        layout->addWidget(scroll, 1);

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

    bool isCurrentlyDefaultBrowser() const {
        if (m_isDefaultBrowserSaved) return true;
        #ifdef Q_OS_WIN
        QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice", QSettings::NativeFormat);
        QString progId = reg.value("ProgId").toString();
        if (progId.contains("BraveLite", Qt::CaseInsensitive) || progId.contains("fedorabrowser", Qt::CaseInsensitive)) {
            return true;
        }
        #else
        QProcess proc;
        proc.start("xdg-settings", {"get", "default-web-browser"});
        if (proc.waitForFinished(1000)) {
            QString out = QString::fromUtf8(proc.readAllStandardOutput()).trimmed().toLower();
            if (out.contains("bravelite") || out.contains("mybrowser") || out.contains("fedorabrowser")) {
                return true;
            }
        }
        #endif
        return false;
    }

    QWidget* createGetStartedWidget(QWidget *parent = nullptr) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(18);

        QWidget *defaultCard = new QWidget(container);
        defaultCard->setStyleSheet("background: #242529; border: 1px solid #32353b; border-radius: 8px;");
        QHBoxLayout *defLayout = new QHBoxLayout(defaultCard);
        defLayout->setContentsMargins(20, 18, 20, 18);

        QVBoxLayout *defTextLayout = new QVBoxLayout();
        QLabel *defTitle = new QLabel("Default browser", defaultCard);
        defTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff; border: none;");

        bool alreadyDefault = isCurrentlyDefaultBrowser();
        QLabel *defSub = new QLabel(alreadyDefault
        ? "BraveLite is currently your default web browser."
        : "Make BraveLite your default web browser for opening web links.", defaultCard);
        defSub->setStyleSheet("font-size: 13px; color: #9aa0a6; border: none;");
        defTextLayout->addWidget(defTitle);
        defTextLayout->addWidget(defSub);

        QPushButton *btnMakeDefault = new QPushButton(alreadyDefault ? "✔ Default Browser" : "Make default", defaultCard);
        btnMakeDefault->setCursor(Qt::PointingHandCursor);
        if (alreadyDefault) {
            btnMakeDefault->setStyleSheet("QPushButton { background: #52c41a; color: #fff; font-weight: bold; padding: 8px 18px; border-radius: 6px; border: none; }");
            btnMakeDefault->setEnabled(false);
        } else {
            btnMakeDefault->setStyleSheet("QPushButton { background: #ff5500; color: #fff; font-weight: bold; padding: 8px 18px; border-radius: 6px; border: none; } QPushButton:hover { background: #ff6a22; }");
        }

        connect(btnMakeDefault, &QPushButton::clicked, this, [this, btnMakeDefault, defSub]() {
            #ifdef Q_OS_WIN
            QDesktopServices::openUrl(QUrl("ms-settings:defaultapps"));
            #else
            QProcess::execute("xdg-settings", {"set", "default-web-browser", "bravelite.desktop"});
            #endif
            m_isDefaultBrowserSaved = true;
            savePersistentData();
            defSub->setText("BraveLite is currently your default web browser.");
            btnMakeDefault->setText("✔ Default Browser");
            btnMakeDefault->setStyleSheet("QPushButton { background: #52c41a; color: #fff; font-weight: bold; padding: 8px 18px; border-radius: 6px; border: none; }");
            btnMakeDefault->setEnabled(false);
        });

        defLayout->addLayout(defTextLayout, 1);
        defLayout->addWidget(btnMakeDefault);
        layout->addWidget(defaultCard);

        QWidget *startupCard = new QWidget(container);
        startupCard->setStyleSheet("background: #242529; border: 1px solid #32353b; border-radius: 8px;");
        QVBoxLayout *startLayout = new QVBoxLayout(startupCard);
        startLayout->setContentsMargins(20, 18, 20, 18);
        startLayout->setSpacing(12);

        QLabel *startTitle = new QLabel("On startup", startupCard);
        startTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff; border: none;");
        startLayout->addWidget(startTitle);

        QRadioButton *rbNewTab = new QRadioButton("Open the New Tab page", startupCard);
        QRadioButton *rbContinue = new QRadioButton("Continue where you left off", startupCard);
        rbNewTab->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none; padding: 4px;");
        rbContinue->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none; padding: 4px;");

        if (startupContinueWhereLeftOff) rbContinue->setChecked(true);
        else rbNewTab->setChecked(true);

        connect(rbNewTab, &QRadioButton::toggled, this, [this](bool checked) {
            if (checked) {
                startupContinueWhereLeftOff = false;
                savePersistentData();
            }
        });
        connect(rbContinue, &QRadioButton::toggled, this, [this](bool checked) {
            if (checked) {
                startupContinueWhereLeftOff = true;
                savePersistentData();
            }
        });

        startLayout->addWidget(rbNewTab);
        startLayout->addWidget(rbContinue);
        layout->addWidget(startupCard);

        QWidget *searchCard = new QWidget(container);
        searchCard->setStyleSheet("background: #242529; border: 1px solid #32353b; border-radius: 8px;");
        QHBoxLayout *searchLayout = new QHBoxLayout(searchCard);
        searchLayout->setContentsMargins(20, 18, 20, 18);

        QVBoxLayout *searchTextLayout = new QVBoxLayout();
        QLabel *sTitle = new QLabel("Search engine", searchCard);
        sTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff; border: none;");
        QLabel *sSub = new QLabel("Choose between BraveLite Engine (DuckDuckGo UI + Google-style AI Overview) or standard DuckDuckGo.", searchCard);
        sSub->setStyleSheet("font-size: 13px; color: #9aa0a6; border: none;");
        searchTextLayout->addWidget(sTitle);
        searchTextLayout->addWidget(sSub);

        QComboBox *engineCombo = new QComboBox(searchCard);
        engineCombo->addItems({"BraveLite Engine", "DuckDuckGo"});
        engineCombo->setCurrentText(defaultSearchEngine);
        engineCombo->setStyleSheet("QComboBox { background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 14px; color: #fff; font-size: 13px; }");
        connect(engineCombo, &QComboBox::currentTextChanged, this, [this](const QString &val) {
            defaultSearchEngine = val;
            savePersistentData();
        });

        searchLayout->addLayout(searchTextLayout, 1);
        searchLayout->addWidget(engineCombo);
        layout->addWidget(searchCard);

        layout->addStretch(1);
        return container;
    }

    // ---------------- GPU HARDWARE DETECTION FOR BRAVELITE AI ----------------
    void detectHardwareGpu() {
        m_detectedGpuName.clear();
        #ifdef Q_OS_WIN
        QProcess p;
        p.start("wmic", {"path", "win32_VideoController", "get", "name"});
        if (p.waitForFinished(1200)) {
            QStringList lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
            for (const QString &l : lines) {
                QString clean = l.trimmed();
                if (!clean.isEmpty() && !clean.startsWith("Name", Qt::CaseInsensitive)) {
                    m_detectedGpuName = clean;
                    break;
                }
            }
        }
        #else
        // 1. Check NVIDIA GPU via nvidia-smi
        QString nvSmi = QStandardPaths::findExecutable("nvidia-smi");
        if (!nvSmi.isEmpty()) {
            QProcess p;
            p.start(nvSmi, {"--query-gpu=name", "--format=csv,noheader"});
            if (p.waitForFinished(1000) && p.exitCode() == 0) {
                m_detectedGpuName = QString::fromUtf8(p.readAllStandardOutput()).trimmed().split('\n').first();
            }
        }
        // 2. Check Linux PCI / DRI GPU devices via lspci
        if (m_detectedGpuName.isEmpty()) {
            QString lspci = QStandardPaths::findExecutable("lspci");
            if (!lspci.isEmpty()) {
                QProcess p;
                p.start(lspci, {});
                if (p.waitForFinished(1000)) {
                    QStringList lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n');
                    for (const QString &l : lines) {
                        if (l.contains("VGA compatible controller", Qt::CaseInsensitive) ||
                            l.contains("3D controller", Qt::CaseInsensitive) ||
                            l.contains("Display controller", Qt::CaseInsensitive)) {
                            int colon = l.indexOf(": ", 8);
                        m_detectedGpuName = (colon != -1) ? l.mid(colon + 2).trimmed() : l.trimmed();
                        break;
                            }
                    }
                }
            }
        }
        // 3. Fallback check for /dev/dri/renderD128
        if (m_detectedGpuName.isEmpty() && QFileInfo::exists("/dev/dri/renderD128")) {
            m_detectedGpuName = "Linux DRI Hardware GPU (/dev/dri/renderD128)";
        }
        #endif
    }

    void ensureOllamaServerRunning() {
        QString ollamaBin = QStandardPaths::findExecutable("ollama");
        if (!ollamaBin.isEmpty()) {
            QProcess::startDetached(ollamaBin, {"serve"});
        }
    }

    QWidget* createBraveLiteAiDashboardWidget(QWidget *parent = nullptr) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(14);

        QWidget *card = new QWidget(container);
        card->setStyleSheet("background: #242529; border: 1px solid #32353b; border-radius: 8px;");
        QVBoxLayout *cLayout = new QVBoxLayout(card);
        cLayout->setContentsMargins(20, 18, 20, 18);
        cLayout->setSpacing(12);

        QCheckBox *chkMaster = new QCheckBox("Enable BraveLite AI (Powered locally via Ollama)", card);
        chkMaster->setChecked(aiEnabled);
        chkMaster->setStyleSheet("font-size: 15px; font-weight: bold; color: #ff6a22; border: none;");

        QCheckBox *chkOverview = new QCheckBox("Enable Google-style AI Overview on BraveLite Engine Search Results", card);
        chkOverview->setChecked(aiSearchOverviewEnabled);
        chkOverview->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none;");

        QCheckBox *chkHover = new QCheckBox("Summarize webpages when hovering mouse over links", card);
        chkHover->setChecked(aiHoverSummaryEnabled);
        chkHover->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none;");

        QString gpuStatusText = m_detectedGpuName.isEmpty()
        ? "Use GPU Acceleration if available (No dedicated GPU detected — will fallback to CPU)"
        : QString("Use GPU Acceleration — Detected: %1").arg(m_detectedGpuName);
        QCheckBox *chkGpu = new QCheckBox(gpuStatusText, card);
        chkGpu->setChecked(aiUseGpu && !m_detectedGpuName.isEmpty());
        chkGpu->setStyleSheet("font-size: 14px; font-weight: 600; color: #52c41a; border: none;");

        cLayout->addWidget(chkMaster);
        cLayout->addWidget(chkOverview);
        cLayout->addWidget(chkHover);
        cLayout->addWidget(chkGpu);

        QFormLayout *form = new QFormLayout();
        form->setSpacing(10);

        QLineEdit *endpointEdit = new QLineEdit(ollamaEndpoint, card);
        endpointEdit->setPlaceholderText("http://127.0.0.1:11434");
        endpointEdit->setStyleSheet("background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 10px; color: #fff;");

        QComboBox *modelCombo = new QComboBox(card);
        modelCombo->setEditable(true);
        QStringList presetModels = {"smollm2:360m", "smollm2:135m", "qwen2.5:0.5b", "llama3.2:1b", "phi3:mini", "gemma2:2b", "tinyllama"};
        if (!presetModels.contains(ollamaModel.toLower())) {
            modelCombo->addItem(ollamaModel.toLower());
        }
        modelCombo->addItems(presetModels);
        modelCombo->setCurrentText(ollamaModel.toLower());
        modelCombo->setStyleSheet("QComboBox { background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 10px; color: #fff; }");

        QLineEdit *sysPromptEdit = new QLineEdit(aiSystemPrompt, card);
        sysPromptEdit->setStyleSheet("background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 10px; color: #fff;");

        form->addRow("Ollama Endpoint URL:", endpointEdit);
        form->addRow("Light LLM Model Name:", modelCombo);
        form->addRow("System Prompt:", sysPromptEdit);
        cLayout->addLayout(form);

        QLabel *statusLbl = new QLabel("Status: Ready. GPU detection & local Ollama controls are active.", card);
        statusLbl->setWordWrap(true);
        statusLbl->setStyleSheet("font-size: 13px; color: #9aa0a6; border: none;");
        cLayout->addWidget(statusLbl);

        QProgressBar *pullProgressBar = new QProgressBar(card);
        pullProgressBar->setFixedHeight(8);
        pullProgressBar->setRange(0, 100);
        pullProgressBar->setValue(0);
        pullProgressBar->setTextVisible(false);
        pullProgressBar->setStyleSheet("QProgressBar { background: #3c4043; border: none; border-radius: 4px; } QProgressBar::chunk { background: #ff5500; border-radius: 4px; }");
        pullProgressBar->hide();
        cLayout->addWidget(pullProgressBar);

        QHBoxLayout *btnRow1 = new QHBoxLayout();
        QPushButton *btnSaveAi = new QPushButton("Save AI Configuration", card);
        QPushButton *btnStartServer = new QPushButton("🚀 Start Local Ollama Server", card);
        QPushButton *btnPullModel = new QPushButton("⬇ Pull / Download Selected Model", card);

        QHBoxLayout *btnRow2 = new QHBoxLayout();
        QPushButton *btnFetchModels = new QPushButton("🔄 Detect Installed Models", card);
        QPushButton *btnRedetectGpu = new QPushButton("🖥 Re-Scan GPU Hardware", card);
        QPushButton *btnTestAi = new QPushButton("✨ Test BraveLite AI", card);

        btnSaveAi->setStyleSheet("QPushButton { background: #ff5500; color: #fff; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: none; }");
        btnStartServer->setStyleSheet("QPushButton { background: #2b5b84; color: #fff; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: none; } QPushButton:hover { background: #356fa1; }");
        btnPullModel->setStyleSheet("QPushButton { background: #2d6a4f; color: #fff; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: none; } QPushButton:hover { background: #40916c; }");
        btnFetchModels->setStyleSheet("QPushButton { background: #35373c; color: #fff; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: 1px solid #484b52; }");
        btnRedetectGpu->setStyleSheet("QPushButton { background: #35373c; color: #fff; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: 1px solid #484b52; }");
        btnTestAi->setStyleSheet("QPushButton { background: #35373c; color: #fff; font-weight: bold; padding: 8px 14px; border-radius: 6px; border: 1px solid #484b52; }");

        btnRow1->addWidget(btnSaveAi);
        btnRow1->addWidget(btnStartServer);
        btnRow1->addWidget(btnPullModel);
        btnRow1->addStretch();

        btnRow2->addWidget(btnFetchModels);
        btnRow2->addWidget(btnRedetectGpu);
        btnRow2->addWidget(btnTestAi);
        btnRow2->addStretch();

        cLayout->addLayout(btnRow1);
        cLayout->addLayout(btnRow2);

        connect(chkMaster, &QCheckBox::toggled, this, [this](bool v) { aiEnabled = v; savePersistentData(); });
        connect(chkOverview, &QCheckBox::toggled, this, [this](bool v) { aiSearchOverviewEnabled = v; savePersistentData(); });
        connect(chkHover, &QCheckBox::toggled, this, [this](bool v) { aiHoverSummaryEnabled = v; savePersistentData(); });
        connect(chkGpu, &QCheckBox::toggled, this, [this](bool v) { aiUseGpu = v; savePersistentData(); });

        connect(btnRedetectGpu, &QPushButton::clicked, card, [this, chkGpu, statusLbl]() {
            detectHardwareGpu();
            if (!m_detectedGpuName.isEmpty()) {
                chkGpu->setText("Use GPU Acceleration — Detected: " + m_detectedGpuName);
                chkGpu->setChecked(true);
                aiUseGpu = true;
                savePersistentData();
                statusLbl->setText("✔ GPU Detected: " + m_detectedGpuName + " (Ollama num_gpu acceleration enabled).");
                statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
            } else {
                chkGpu->setText("Use GPU Acceleration if available (No dedicated GPU detected)");
                statusLbl->setText("⚠ No dedicated GPU found; BraveLite AI will run in lightweight CPU mode.");
                statusLbl->setStyleSheet("font-size: 13px; color: #faad14; border: none;");
            }
        });

        connect(btnSaveAi, &QPushButton::clicked, card, [this, endpointEdit, modelCombo, sysPromptEdit, statusLbl]() {
            ollamaEndpoint = endpointEdit->text().trimmed();
            if (ollamaEndpoint.endsWith("/")) ollamaEndpoint.chop(1);
            ollamaModel = modelCombo->currentText().trimmed().toLower();
            modelCombo->setCurrentText(ollamaModel);
            aiSystemPrompt = sysPromptEdit->text().trimmed();
            savePersistentData();
            statusLbl->setText("✔ BraveLite AI configuration saved (Model: " + ollamaModel + ").");
            statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
        });

        connect(btnStartServer, &QPushButton::clicked, card, [statusLbl]() {
            QString ollamaBin = QStandardPaths::findExecutable("ollama");
            if (ollamaBin.isEmpty()) {
                statusLbl->setText("✖ 'ollama' binary was not found on your system PATH. Install Ollama first (curl -fsSL https://ollama.com/install.sh | sh).");
                statusLbl->setStyleSheet("font-size: 13px; color: #f5222d; border: none;");
                return;
            }
            bool started = QProcess::startDetached(ollamaBin, {"serve"});
            if (started) {
                statusLbl->setText("✔ Launched 'ollama serve' in the background! Now click '⬇ Pull / Download Selected Model' if you haven't downloaded the model yet.");
                statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
            } else {
                statusLbl->setText("⚠ Could not spawn 'ollama serve' (it may already be running).");
                statusLbl->setStyleSheet("font-size: 13px; color: #faad14; border: none;");
            }
        });

        connect(btnPullModel, &QPushButton::clicked, card, [this, card, endpointEdit, modelCombo, statusLbl, pullProgressBar]() {
            ensureOllamaServerRunning();
            QString base = endpointEdit->text().trimmed();
            if (base.endsWith("/")) base.chop(1);
            QString modelToPull = modelCombo->currentText().trimmed().toLower();
            modelCombo->setCurrentText(modelToPull);
            ollamaModel = modelToPull;
            savePersistentData();

            QNetworkRequest req(QUrl(base + "/api/pull"));
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            QJsonObject body;
            body["name"] = modelToPull;
            body["stream"] = true;

            pullProgressBar->setRange(0, 100);
            pullProgressBar->setValue(0);
            pullProgressBar->show();
            statusLbl->setText("⏳ Downloading model '" + modelToPull + "' from Ollama registry...");
            statusLbl->setStyleSheet("font-size: 13px; color: #ff9c6e; border: none;");

            QNetworkReply *rep = m_netManager->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
            connect(rep, &QNetworkReply::readyRead, card, [rep, statusLbl, pullProgressBar, modelToPull]() {
                while (rep->canReadLine()) {
                    QByteArray line = rep->readLine().trimmed();
                    if (line.isEmpty()) continue;
                    QJsonObject obj = QJsonDocument::fromJson(line).object();
                    QString status = obj["status"].toString();
                    qint64 completed = obj["completed"].toVariant().toLongLong();
                    qint64 total = obj["total"].toVariant().toLongLong();
                    if (total > 0) {
                        int pct = qBound(0, static_cast<int>((completed * 100) / total), 100);
                        pullProgressBar->setValue(pct);
                        statusLbl->setText(QString("⏳ Pulling %1: %2 (%3%)").arg(modelToPull, status).arg(pct));
                    } else if (!status.isEmpty()) {
                        statusLbl->setText(QString("⏳ Pulling %1: %2").arg(modelToPull, status));
                    }
                }
            });

            connect(rep, &QNetworkReply::finished, card, [rep, statusLbl, pullProgressBar, modelToPull]() {
                pullProgressBar->hide();
                if (rep->error() == QNetworkReply::NoError) {
                    statusLbl->setText("✔ Model '" + modelToPull + "' is downloaded and ready! Click '✨ Test BraveLite AI' to verify.");
                    statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
                } else {
                    statusLbl->setText("✖ Failed to pull '" + modelToPull + "': " + rep->errorString() + " (Ensure Ollama is installed and running).");
                    statusLbl->setStyleSheet("font-size: 13px; color: #f5222d; border: none;");
                }
                rep->deleteLater();
            });
        });

        connect(btnFetchModels, &QPushButton::clicked, card, [this, card, endpointEdit, modelCombo, statusLbl]() {
            ensureOllamaServerRunning();
            QString base = endpointEdit->text().trimmed();
            if (base.endsWith("/")) base.chop(1);
            QNetworkRequest req(QUrl(base + "/api/tags"));
            QNetworkReply *rep = m_netManager->get(req);
            statusLbl->setText("⏳ Querying Ollama /api/tags...");
            connect(rep, &QNetworkReply::finished, card, [rep, modelCombo, statusLbl]() {
                if (rep->error() == QNetworkReply::NoError) {
                    QJsonObject obj = QJsonDocument::fromJson(rep->readAll()).object();
                    QJsonArray models = obj["models"].toArray();
                    if (!models.isEmpty()) {
                        modelCombo->clear();
                        for (auto m : models) {
                            modelCombo->addItem(m.toObject()["name"].toString());
                        }
                        statusLbl->setText(QString("✔ Found %1 installed local Ollama model(s)!").arg(models.size()));
                        statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
                    } else {
                        statusLbl->setText("⚠ Ollama server is running, but no models are downloaded yet. Click '⬇ Pull / Download Selected Model' above!");
                        statusLbl->setStyleSheet("font-size: 13px; color: #faad14; border: none;");
                    }
                } else {
                    statusLbl->setText("✖ Could not connect to Ollama at " + rep->url().toString() + " (" + rep->errorString() + "). Click '🚀 Start Local Ollama Server' first.");
                    statusLbl->setStyleSheet("font-size: 13px; color: #f5222d; border: none;");
                }
                rep->deleteLater();
            });
        });

        connect(btnTestAi, &QPushButton::clicked, card, [this, endpointEdit, modelCombo, sysPromptEdit, statusLbl]() {
            ensureOllamaServerRunning();
            ollamaEndpoint = endpointEdit->text().trimmed();
            if (ollamaEndpoint.endsWith("/")) ollamaEndpoint.chop(1);
            ollamaModel = modelCombo->currentText().trimmed().toLower();
            modelCombo->setCurrentText(ollamaModel);
            aiSystemPrompt = sysPromptEdit->text().trimmed();
            savePersistentData();
            statusLbl->setText(QString("⏳ Asking BraveLite AI (%1 on %2)...").arg(ollamaModel, aiUseGpu ? "GPU" : "CPU"));
            statusLbl->setStyleSheet("font-size: 13px; color: #ff9c6e; border: none;");
            queryOllamaAsync("Say hello in one short sentence as BraveLite AI.", [statusLbl](const QString &resp, bool ok) {
                if (ok) {
                    statusLbl->setText("✨ BraveLite AI: " + resp.trimmed());
                    statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
                } else {
                    statusLbl->setText("✖ " + resp);
                    statusLbl->setStyleSheet("font-size: 13px; color: #f5222d; border: none;");
                }
            });
        });

        layout->addWidget(card);
        layout->addStretch(1);
        return container;
    }

    QWidget* createThemeProfilesWidget(QWidget *parent = nullptr) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(14);

        QWidget *card = new QWidget(container);
        card->setStyleSheet("background: #242529; border: 1px solid #32353b; border-radius: 8px;");
        QVBoxLayout *cLayout = new QVBoxLayout(card);
        cLayout->setContentsMargins(20, 18, 20, 18);
        cLayout->setSpacing(12);

        QLabel *lbl = new QLabel("Select a Browser Theme Profile:", card);
        lbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff; border: none;");
        cLayout->addWidget(lbl);

        QStringList themes = {
            "BraveLite Dark",
            "Midnight OLED",
            "Cyberpunk Neon",
            "Nordic Frost",
            "Catppuccin Mocha"
        };

        for (const QString &themeName : themes) {
            QRadioButton *rb = new QRadioButton(themeName, card);
            rb->setChecked(currentThemeProfile == themeName);
            rb->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none; padding: 6px;");
            connect(rb, &QRadioButton::toggled, this, [this, themeName](bool checked) {
                if (checked) {
                    currentThemeProfile = themeName;
                    applyThemeProfile(themeName);
                    savePersistentData();
                }
            });
            cLayout->addWidget(rb);
        }

        layout->addWidget(card);
        layout->addStretch(1);
        return container;
    }

    QWidget* createKeybindsWidget(QWidget *parent = nullptr) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(12);

        struct ActionLabel { QString id; QString name; };
        QList<ActionLabel> actions = {
            {"NewTab", "Open New Tab"},
            {"CloseTab", "Close Current Tab"},
            {"NewWindow", "Open New Window"},
            {"Incognito", "Open Private / Incognito Window"},
            {"History", "Open Browsing History"},
            {"Downloads", "Open Downloads Tab"},
            {"FocusUrl", "Focus & Select Address Bar"},
            {"Reload", "Reload Current Page"},
            {"ZoomIn", "Zoom In"},
            {"ZoomOut", "Zoom Out"},
            {"ZoomReset", "Reset Zoom (100%)"}
        };

        QTableWidget *table = new QTableWidget(actions.size(), 2, container);
        table->setHorizontalHeaderLabels({"Action", "Keybind Shortcut (Click to Edit)"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setStyleSheet(
            "QTableWidget { background-color: #242529; border: 1px solid #32353b; border-radius: 8px; color: #fff; gridline-color: #32353b; font-size: 13px; }"
            "QHeaderView::section { background-color: #2b2d30; color: #fff; font-weight: bold; padding: 8px; border: none; border-bottom: 1px solid #32353b; }"
        );

        for (int i = 0; i < actions.size(); ++i) {
            QTableWidgetItem *nameItem = new QTableWidgetItem(actions[i].name);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 0, nameItem);

            QKeySequenceEdit *seqEdit = new QKeySequenceEdit(QKeySequence(keybindsMap.value(actions[i].id)), table);
            seqEdit->setStyleSheet("QKeySequenceEdit { background: #1e1f22; color: #ff6a22; border: 1px solid #3c4043; border-radius: 4px; padding: 4px; font-weight: bold; }");
            QString actId = actions[i].id;
            connect(seqEdit, &QKeySequenceEdit::keySequenceChanged, this, [this, actId](const QKeySequence &seq) {
                if (!seq.isEmpty()) {
                    keybindsMap[actId] = seq.toString();
                    rebuildShortcuts();
                    savePersistentData();
                }
            });
            table->setCellWidget(i, 1, seqEdit);
        }

        layout->addWidget(table, 1);

        QHBoxLayout *btnRow = new QHBoxLayout();
        QPushButton *btnResetKeys = new QPushButton("Reset Keybinds to Default", container);
        btnResetKeys->setStyleSheet("QPushButton { background: #35373c; color: #fff; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: 1px solid #484b52; }");
        connect(btnResetKeys, &QPushButton::clicked, this, [this, table, actions]() {
            initDefaultKeybinds();
            rebuildShortcuts();
            savePersistentData();
            for (int i = 0; i < actions.size(); ++i) {
                if (auto *ed = qobject_cast<QKeySequenceEdit*>(table->cellWidget(i, 1))) {
                    ed->setKeySequence(QKeySequence(keybindsMap.value(actions[i].id)));
                }
            }
        });
        btnRow->addWidget(btnResetKeys);
        btnRow->addStretch();
        layout->addLayout(btnRow);

        return container;
    }

    QWidget* createPasswordManagerWidget(QWidget *parent = nullptr) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(12);

        QHBoxLayout *topControls = new QHBoxLayout();
        QCheckBox *chkAutofill = new QCheckBox("Prompt to autofill saved logins when clicking username/password boxes", container);
        chkAutofill->setChecked(autofillEnabled);
        chkAutofill->setStyleSheet("font-size: 14px; color: #e0e0e0;");
        connect(chkAutofill, &QCheckBox::toggled, this, [this](bool checked) {
            autofillEnabled = checked;
            savePersistentData();
        });

        QCheckBox *chkShowPass = new QCheckBox("Show passwords in plain text", container);
        chkShowPass->setStyleSheet("font-size: 14px; color: #e0e0e0;");

        topControls->addWidget(chkAutofill);
        topControls->addStretch();
        topControls->addWidget(chkShowPass);
        layout->addLayout(topControls);

        QLineEdit *searchFilter = new QLineEdit(container);
        searchFilter->setPlaceholderText("🔍 Filter saved logins by website or username...");
        searchFilter->setStyleSheet("background: #2b2d30; border: 1px solid #3c4043; border-radius: 8px; padding: 8px 12px; color: #fff;");
        layout->addWidget(searchFilter);

        QTableWidget *table = new QTableWidget(0, 3, container);
        table->setHorizontalHeaderLabels({"Website (Domain)", "Username / Email", "Password"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setStyleSheet(
            "QTableWidget { background-color: #242529; border: 1px solid #32353b; border-radius: 8px; color: #fff; gridline-color: #32353b; font-size: 13px; }"
            "QHeaderView::section { background-color: #2b2d30; color: #fff; font-weight: bold; padding: 8px; border: none; border-bottom: 1px solid #32353b; }"
            "QTableWidget::item { padding: 6px; }"
            "QTableWidget::item:selected { background-color: #ff5500; color: #fff; }"
        );
        layout->addWidget(table, 1);

        auto refreshTable = [this, table, chkShowPass, searchFilter]() {
            QString filter = searchFilter->text().trimmed().toLower();
            bool showPlain = chkShowPass->isChecked();
            table->setRowCount(0);

            for (int i = 0; i < savedPasswords.size(); ++i) {
                const auto &cred = savedPasswords[i];
                if (!filter.isEmpty() && !cred.site.toLower().contains(filter) && !cred.username.toLower().contains(filter)) {
                    continue;
                }
                int row = table->rowCount();
                table->insertRow(row);

                QTableWidgetItem *siteItem = new QTableWidgetItem(cred.site);
                siteItem->setData(Qt::UserRole, i);
                table->setItem(row, 0, siteItem);
                table->setItem(row, 1, new QTableWidgetItem(cred.username));
                table->setItem(row, 2, new QTableWidgetItem(showPlain ? cred.password : QString("••••••••••••")));
            }
        };

        connect(chkShowPass, &QCheckBox::toggled, container, refreshTable);
        connect(searchFilter, &QLineEdit::textChanged, container, refreshTable);
        refreshTable();

        QHBoxLayout *btnRow = new QHBoxLayout();
        QPushButton *btnAdd = new QPushButton("+ Add Login", container);
        QPushButton *btnEdit = new QPushButton("Edit Selected", container);
        QPushButton *btnCopyUser = new QPushButton("Copy Username", container);
        QPushButton *btnCopyPass = new QPushButton("Copy Password", container);
        QPushButton *btnFillTab = new QPushButton("🔑 Fill Current Tab", container);
        QPushButton *btnDelete = new QPushButton("Delete", container);

        QString primaryStyle = "QPushButton { background: #ff5500; font-weight: bold; padding: 8px 14px; border-radius: 6px; color: #fff; border: none; } QPushButton:hover { background: #ff6a22; }";
        QString secondaryStyle = "QPushButton { background: #35373c; font-weight: bold; padding: 8px 14px; border-radius: 6px; color: #fff; border: 1px solid #484b52; } QPushButton:hover { background: #45484f; }";
        QString dangerStyle = "QPushButton { background: #e81123; font-weight: bold; padding: 8px 14px; border-radius: 6px; color: #fff; border: none; } QPushButton:hover { background: #ff3b4b; }";

        btnAdd->setStyleSheet(primaryStyle);
        btnEdit->setStyleSheet(secondaryStyle);
        btnCopyUser->setStyleSheet(secondaryStyle);
        btnCopyPass->setStyleSheet(secondaryStyle);
        btnFillTab->setStyleSheet(secondaryStyle);
        btnDelete->setStyleSheet(dangerStyle);

        btnRow->addWidget(btnAdd);
        btnRow->addWidget(btnEdit);
        btnRow->addWidget(btnCopyUser);
        btnRow->addWidget(btnCopyPass);
        btnRow->addWidget(btnFillTab);
        btnRow->addStretch();
        btnRow->addWidget(btnDelete);
        layout->addLayout(btnRow);

        connect(btnAdd, &QPushButton::clicked, container, [this, container, refreshTable]() {
            bool ok = false;
            QString site = QInputDialog::getText(container, "Add Saved Login", "Website domain (e.g. github.com):", QLineEdit::Normal, "", &ok).trimmed();
            if (!ok || site.isEmpty()) return;
            if (site.startsWith("http://") || site.startsWith("https://")) {
                site = QUrl(site).host();
            }

            QString user = QInputDialog::getText(container, "Add Saved Login", "Username / Email:", QLineEdit::Normal, "", &ok).trimmed();
            if (!ok) return;

            QString pass = QInputDialog::getText(container, "Add Saved Login", "Password:", QLineEdit::Password, "", &ok);
            if (!ok || pass.isEmpty()) return;

            savedPasswords.append({site, user, pass});
            savePersistentData();
            refreshTable();
        });

        connect(btnEdit, &QPushButton::clicked, container, [this, container, table, refreshTable]() {
            int row = table->currentRow();
            if (row < 0) return;
            int idx = table->item(row, 0)->data(Qt::UserRole).toInt();
            if (idx < 0 || idx >= savedPasswords.size()) return;

            bool ok = false;
            QString site = QInputDialog::getText(container, "Edit Login", "Website domain:", QLineEdit::Normal, savedPasswords[idx].site, &ok).trimmed();
            if (!ok || site.isEmpty()) return;

            QString user = QInputDialog::getText(container, "Edit Login", "Username / Email:", QLineEdit::Normal, savedPasswords[idx].username, &ok).trimmed();
            if (!ok) return;

            QString pass = QInputDialog::getText(container, "Edit Login", "Password:", QLineEdit::Normal, savedPasswords[idx].password, &ok);
            if (!ok || pass.isEmpty()) return;

            savedPasswords[idx] = {site, user, pass};
            savePersistentData();
            refreshTable();
        });

        connect(btnCopyUser, &QPushButton::clicked, container, [this, table]() {
            int row = table->currentRow();
            if (row < 0) return;
            int idx = table->item(row, 0)->data(Qt::UserRole).toInt();
            if (idx >= 0 && idx < savedPasswords.size()) {
                QApplication::clipboard()->setText(savedPasswords[idx].username);
            }
        });

        connect(btnCopyPass, &QPushButton::clicked, container, [this, table]() {
            int row = table->currentRow();
            if (row < 0) return;
            int idx = table->item(row, 0)->data(Qt::UserRole).toInt();
            if (idx >= 0 && idx < savedPasswords.size()) {
                QApplication::clipboard()->setText(savedPasswords[idx].password);
            }
        });

        connect(btnFillTab, &QPushButton::clicked, container, [this]() {
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
                promptAutofillCredentials(v->url().host());
            }
        });

        connect(btnDelete, &QPushButton::clicked, container, [this, table, refreshTable]() {
            int row = table->currentRow();
            if (row < 0) return;
            int idx = table->item(row, 0)->data(Qt::UserRole).toInt();
            if (idx >= 0 && idx < savedPasswords.size()) {
                savedPasswords.removeAt(idx);
                savePersistentData();
                refreshTable();
            }
        });

        return container;
    }

    void openPasswordManagerDialog() {
        QDialog dlg(this);
        dlg.setWindowTitle("BraveLite Self-Hosted Password Manager");
        dlg.resize(740, 460);
        dlg.setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QVBoxLayout *layout = new QVBoxLayout(&dlg);
        layout->setContentsMargins(20, 20, 20, 20);

        QLabel *title = new QLabel("🔑 Saved Passwords & Autofill Vault", &dlg);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #fff; margin-bottom: 6px;");
        layout->addWidget(title);

        layout->addWidget(createPasswordManagerWidget(&dlg), 1);
        dlg.exec();
    }

    void openSettingsTab() {
        QWidget *page = new QWidget(this);
        page->setStyleSheet("background-color: #1a1a1e; color: #fff;");
        QHBoxLayout *hLayout = new QHBoxLayout(page);
        hLayout->setContentsMargins(40, 30, 40, 20);
        hLayout->setSpacing(30);

        QListWidget *sidebar = new QListWidget(page);
        sidebar->setMaximumWidth(240);
        sidebar->setStyleSheet(R"(
            QListWidget { background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 8px; outline: none; font-size: 14px; }
            QListWidget::item { padding: 11px 16px; border-radius: 6px; color: #9aa0a6; }
            QListWidget::item:selected { background-color: #ff5500; color: #ffffff; font-weight: bold; }
        )");

        sidebar->addItem("Get started");
        sidebar->addItem("BraveLite AI (Ollama)");
        sidebar->addItem("Theme Profiles");
        sidebar->addItem("Keybinds");
        sidebar->addItem("Autofill and passwords");
        sidebar->addItem("Bookmarks Manager");
        sidebar->addItem("DNS over Oblivious HTTP");
        sidebar->addItem("SponsorBlock Configuration");
        sidebar->addItem("Permissions Dashboard");
        sidebar->addItem("Downloads & Crash Logs");
        sidebar->addItem("Delete browsing data");
        sidebar->addItem("Reset settings");

        QWidget *content = new QWidget(page);
        QVBoxLayout *cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(10, 0, 10, 0);

        QLabel *detailTitle = new QLabel(content);
        detailTitle->setStyleSheet("font-size: 22px; font-weight: bold; margin-bottom: 12px; color: #ffffff;");

        QStackedWidget *detailStack = new QStackedWidget(content);

        detailStack->addWidget(createGetStartedWidget(detailStack));
        detailStack->addWidget(createBraveLiteAiDashboardWidget(detailStack));
        detailStack->addWidget(createThemeProfilesWidget(detailStack));
        detailStack->addWidget(createKeybindsWidget(detailStack));

        QWidget *passPage = new QWidget(detailStack);
        QVBoxLayout *passLayout = new QVBoxLayout(passPage);
        passLayout->setContentsMargins(0, 0, 0, 0);
        passLayout->addWidget(createPasswordManagerWidget(passPage), 1);
        QHBoxLayout *passIoRow = new QHBoxLayout();
        QPushButton *btnImportPass = new QPushButton("Import Passwords / Bookmarks", passPage);
        QPushButton *btnExportPass = new QPushButton("Export Passwords (CSV)", passPage);
        btnImportPass->setStyleSheet("QPushButton { background: #ff5500; font-weight: bold; padding: 8px 16px; border-radius: 6px; color: #fff; border: none; }");
        btnExportPass->setStyleSheet("QPushButton { background: #35373c; font-weight: bold; padding: 8px 16px; border-radius: 6px; color: #fff; border: 1px solid #484b52; }");
        connect(btnImportPass, &QPushButton::clicked, this, &BrowserWindow::importTop5Browsers);
        connect(btnExportPass, &QPushButton::clicked, this, &BrowserWindow::exportPasswords);
        passIoRow->addWidget(btnImportPass);
        passIoRow->addWidget(btnExportPass);
        passIoRow->addStretch();
        passLayout->addLayout(passIoRow);
        detailStack->addWidget(passPage);

        QWidget *listPage = new QWidget(detailStack);
        QVBoxLayout *listLayout = new QVBoxLayout(listPage);
        listLayout->setContentsMargins(0, 0, 0, 0);

        QListWidget *itemsList = new QListWidget(listPage);
        itemsList->setStyleSheet("background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 12px; color: #fff; font-size: 14px;");

        QWidget *btnRow = new QWidget(listPage);
        QHBoxLayout *btnLayout = new QHBoxLayout(btnRow);
        btnLayout->setContentsMargins(0, 8, 0, 0);
        btnLayout->setSpacing(10);

        QPushButton *actionBtn1 = new QPushButton(btnRow);
        QPushButton *actionBtn2 = new QPushButton(btnRow);
        actionBtn1->setStyleSheet("QPushButton { background: #ff5500; font-weight: bold; padding: 8px 16px; border-radius: 6px; color: #fff; border: none; }");
        actionBtn2->setStyleSheet("QPushButton { background: #35373c; font-weight: bold; padding: 8px 16px; border-radius: 6px; color: #fff; border: 1px solid #484b52; }");
        btnLayout->addWidget(actionBtn1);
        btnLayout->addWidget(actionBtn2);
        btnLayout->addStretch();

        listLayout->addWidget(itemsList, 1);
        listLayout->addWidget(btnRow);
        detailStack->addWidget(listPage);

        QHBoxLayout *footerLayout = new QHBoxLayout();
        footerLayout->addStretch();
        QLabel *signatureLabel = new QLabel("made by GrGeorge", content);
        signatureLabel->setStyleSheet("font-size: 11px; color: #6e7278; margin-top: 6px;");
        footerLayout->addWidget(signatureLabel);

        cLayout->addWidget(detailTitle);
        cLayout->addWidget(detailStack, 1);
        cLayout->addLayout(footerLayout);

        auto showCategoryPtr = std::make_shared<std::function<void(int)>>();
        *showCategoryPtr = [=](int row) {
            itemsList->clear();
            actionBtn1->disconnect();
            actionBtn2->disconnect();
            actionBtn1->hide();
            actionBtn2->hide();

            if (row == 0) {
                detailTitle->setText("Get started");
                detailStack->setCurrentIndex(0);
                return;
            }
            if (row == 1) {
                detailTitle->setText("BraveLite AI (Local Ollama & GPU Configuration)");
                detailStack->setCurrentIndex(1);
                return;
            }
            if (row == 2) {
                detailTitle->setText("Theme Profiles");
                detailStack->setCurrentIndex(2);
                return;
            }
            if (row == 3) {
                detailTitle->setText("Keybinds & Shortcuts");
                detailStack->setCurrentIndex(3);
                return;
            }
            if (row == 4) {
                detailTitle->setText("Autofill and passwords");
                detailStack->setCurrentIndex(4);
                return;
            }

            detailStack->setCurrentIndex(5);

            if (row == 5) {
                detailTitle->setText("Bookmarks Manager (Drag bookmarks on bar to reorder)");
                for (const auto &b : bookmarksList) {
                    itemsList->addItem(b.title + " (" + b.url + ")");
                }
                actionBtn1->setText("Import Bookmarks (HTML/JSON/Browser)");
                actionBtn2->setText("Export Bookmarks (HTML)");
                actionBtn1->show();
                actionBtn2->show();
                connect(actionBtn1, &QPushButton::clicked, this, &BrowserWindow::importTop5Browsers);
                connect(actionBtn2, &QPushButton::clicked, this, &BrowserWindow::exportBookmarks);
            } else if (row == 6) {
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
            } else if (row == 7) {
                detailTitle->setText("SponsorBlock Configuration");
                itemsList->addItem("Status: Active (Auto-skips sponsors & ads on YouTube)");
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
            } else if (row == 8) {
                detailTitle->setText("Permissions Dashboard");
                itemsList->addItem("Inspect and revoke camera, mic, and location permissions.");
                actionBtn1->setText("Open Dashboard Tab");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, this, &BrowserWindow::openPermissionsTab);
            } else if (row == 9) {
                detailTitle->setText("Downloads & Extensive Crash Logs");
                itemsList->addItem("Downloads folder: " + QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
                itemsList->addItem("Crash Logs folder: " + CrashLogger::crashLogDir());
                actionBtn1->setText("Open Crash Logs Folder");
                actionBtn2->setText("Generate Diagnostic Snapshot Log");
                actionBtn1->show();
                actionBtn2->show();
                connect(actionBtn1, &QPushButton::clicked, []() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(CrashLogger::crashLogDir()));
                });
                connect(actionBtn2, &QPushButton::clicked, [this]() {
                    CrashLogger::writeCrashEntry("Manual User Diagnostic Snapshot", "Triggered from Settings -> Downloads & Crash Logs");
                    QMessageBox::information(this, "Diagnostic Log Saved", "Detailed diagnostic report written to:\n" + CrashLogger::crashLogDir());
                });
            } else if (row == 10) {
                detailTitle->setText("Delete browsing data");
                itemsList->addItem("Clear Cookies and Web Cache");
                actionBtn1->setText("Delete All Cookies");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this]() {
                    if (m_profile) m_profile->cookieStore()->deleteAllCookies();
                    QMessageBox::information(this, "Cleared", "Cookies and storage cleared.");
                });
            } else if (row == 11) {
                detailTitle->setText("Reset settings");
                itemsList->addItem("Restore BraveLite defaults to pristine condition");
                actionBtn1->setText("Reset All Browser Data");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this]() {
                    QFile::remove(getStorageFile());
                    historyList.clear();
                    bookmarksList.clear();
                    savedPasswords.clear();
                    m_isDefaultBrowserSaved = false;
                    initDefaultKeybinds();
                    rebuildShortcuts();
                    qDeleteAll(downloadsList);
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

        menu.addAction("New tab\t" + keybindsMap.value("NewTab"), this, [this]() { addNewTab(); });
        menu.addAction("New window\t" + keybindsMap.value("NewWindow"), this, [this]() { (new BrowserWindow(false))->show(); });
        menu.addAction("New private window\t" + keybindsMap.value("Incognito"), this, &BrowserWindow::openIncognito);
        menu.addSeparator();

        QMenu *zoomMenu = menu.addMenu("Zoom");
        zoomMenu->addAction("Zoom In\t" + keybindsMap.value("ZoomIn"), this, &BrowserWindow::zoomIn);
        zoomMenu->addAction("Zoom Out\t" + keybindsMap.value("ZoomOut"), this, &BrowserWindow::zoomOut);
        zoomMenu->addAction("Reset Zoom (100%)\t" + keybindsMap.value("ZoomReset"), this, &BrowserWindow::zoomReset);

        menu.addAction("✨ BraveLite AI Settings", this, &BrowserWindow::openSettingsTab);
        menu.addAction("Passwords & Autofill", this, &BrowserWindow::openPasswordManagerDialog);
        menu.addAction("History\t" + keybindsMap.value("History"), this, &BrowserWindow::openHistoryTab);
        menu.addAction("Bookmarks Manager", this, &BrowserWindow::openSettingsTab);
        menu.addAction("Downloads\t" + keybindsMap.value("Downloads"), this, &BrowserWindow::openDownloadsTab);
        menu.addAction("Site Permissions", this, &BrowserWindow::openPermissionsTab);

        menu.addAction("Delete browsing data...\tCtrl+Shift+Del", this, [this]() {
            if (QMessageBox::question(this, "Clear Data", "Delete all cookies and history?") == QMessageBox::Yes) {
                if (m_profile) m_profile->cookieStore()->deleteAllCookies();
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
                    QString url = obj["url"].toString();
                    bookmarksList.append({obj["name"].toString(), url, loadCachedFavicon(QUrl(url))});
                    count++;
                } else if (obj["children"].isArray()) {
                    self(self, obj["children"].toArray());
                }
            }
        };
        for (const QString &key : roots.keys()) parseNode(parseNode, roots[key].toObject()["children"].toArray());
        savePersistentData();
        renderBookmarks();
        preloadAllBookmarkIcons();
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
                    bookmarksList.append({title, url, loadCachedFavicon(QUrl(url))});
                    count++;
                }
            }
        }
        savePersistentData();
        renderBookmarks();
        preloadAllBookmarkIcons();
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
            if (p.size() >= 3 && !p[0].contains("url", Qt::CaseInsensitive) && !p[0].contains("name", Qt::CaseInsensitive)) {
                QString rawSite = p[0].trimmed();
                if (rawSite.startsWith("http://") || rawSite.startsWith("https://")) {
                    rawSite = QUrl(rawSite).host();
                }
                savedPasswords.append({rawSite, p[1].trimmed(), p[2].trimmed()});
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
    bool m_isDefaultBrowserSaved = false;
    bool m_wasMaximizedBeforeFullScreen = false;
    bool m_inFullScreenTransition = false;
    QWebEngineProfile *m_profile;
    QNetworkAccessManager *m_netManager;
    QPointer<QNetworkReply> m_activeHoverReply;
    QSet<QString> m_pendingFaviconHosts;
    AdBlocker *m_adBlocker;
    QWidget *headerContainer;
    CustomTabBar *tabBar;
    QStackedWidget *tabStack;
    QLineEdit *urlBar;
    QPushButton *btnPrivacyGrade;
    QPushButton *btnShield;
    QPushButton *btnPasswords;
    QPushButton *btnDownload;
    QPushButton *btnMenu;
    QToolBar *bookmarksBar;
    QProgressBar *progressBar;
    QStringList historyList;
    QStringList lastSessionUrls;
    QVector<Bookmark> bookmarksList;
    QVector<DownloadItem*> downloadsList;
    QVector<SavedCredential> savedPasswords;
    QVector<SitePermission> sitePermissions;
    QList<QShortcut*> activeShortcuts;
    QMap<QString, QString> keybindsMap;

    bool autofillEnabled = true;
    bool startupContinueWhereLeftOff = false;
    bool odohEnabled = true;
    QString sponsorBlockServer = "https://sponsor.ajay.app/api/skipSegments";
    QString currentThemeProfile = "BraveLite Dark";
    QString defaultSearchEngine = "BraveLite Engine";

    bool aiEnabled = true;
    bool aiSearchOverviewEnabled = true;
    bool aiHoverSummaryEnabled = true;
    bool aiUseGpu = true;
    QString m_detectedGpuName;
    QString ollamaEndpoint = "http://127.0.0.1:11434";
    QString ollamaModel = "smollm2:360m";
    QString aiSystemPrompt = "You are BraveLite AI, a fast, privacy-first browser assistant. Be concise and accurate.";

    void initDefaultKeybinds() {
        keybindsMap["NewTab"] = "Ctrl+T";
        keybindsMap["CloseTab"] = "Ctrl+W";
        keybindsMap["NewWindow"] = "Ctrl+N";
        keybindsMap["Incognito"] = "Ctrl+Shift+N";
        keybindsMap["History"] = "Ctrl+H";
        keybindsMap["Downloads"] = "Ctrl+J";
        keybindsMap["FocusUrl"] = "Ctrl+L";
        keybindsMap["Reload"] = "Ctrl+R";
        keybindsMap["ZoomIn"] = "Ctrl++";
        keybindsMap["ZoomOut"] = "Ctrl+-";
        keybindsMap["ZoomReset"] = "Ctrl+0";
    }

    void rebuildShortcuts() {
        qDeleteAll(activeShortcuts);
        activeShortcuts.clear();

        auto bindKey = [this](const QString &keyStr, std::function<void()> fn) {
            if (keyStr.isEmpty()) return;
            QShortcut *s = new QShortcut(QKeySequence(keyStr), this);
            connect(s, &QShortcut::activated, this, fn);
            activeShortcuts.append(s);
        };

        bindKey(keybindsMap.value("NewTab"), [this]() { addNewTab(); });
        bindKey(keybindsMap.value("CloseTab"), [this]() { closeTab(tabBar->currentIndex()); });
        bindKey(keybindsMap.value("NewWindow"), [this]() { (new BrowserWindow(false))->show(); });
        bindKey(keybindsMap.value("Incognito"), [this]() { openIncognito(); });
        bindKey(keybindsMap.value("History"), [this]() { openHistoryTab(); });
        bindKey(keybindsMap.value("Downloads"), [this]() { openDownloadsTab(); });
        bindKey(keybindsMap.value("FocusUrl"), [this]() { urlBar->setFocus(); urlBar->selectAll(); });
        bindKey(keybindsMap.value("Reload"), [this]() {
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->reload();
        });
            bindKey(keybindsMap.value("ZoomIn"), [this]() { zoomIn(); });
            bindKey("Ctrl+=", [this]() { zoomIn(); });
            bindKey(keybindsMap.value("ZoomOut"), [this]() { zoomOut(); });
            bindKey(keybindsMap.value("ZoomReset"), [this]() { zoomReset(); });
            bindKey("Esc", [this]() {
                if (isFullScreen()) {
                    if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
                        v->page()->triggerAction(QWebEnginePage::ExitFullScreen);
                    }
                    exitBrowserFullScreen();
                }
            });
    }

    void queryOllamaAsync(const QString &prompt, std::function<void(const QString&, bool)> callback) {
        QString endpoint = ollamaEndpoint.trimmed();
        if (endpoint.endsWith("/")) endpoint.chop(1);
        QUrl apiUrl(endpoint + "/api/generate");

        QNetworkRequest req(apiUrl);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject options;
        // Pass GPU acceleration option (-1 offloads all layers to GPU if available; 0 forces CPU)
        options["num_gpu"] = (aiUseGpu && !m_detectedGpuName.isEmpty()) ? -1 : 0;

        QJsonObject body;
        body["model"] = ollamaModel.trimmed().toLower();
        body["system"] = aiSystemPrompt;
        body["prompt"] = prompt;
        body["stream"] = false;
        body["options"] = options;

        QNetworkReply *reply = m_netManager->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
        connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
            QByteArray raw = reply->readAll();
            if (reply->error() == QNetworkReply::NoError) {
                QJsonObject obj = QJsonDocument::fromJson(raw).object();
                if (obj.contains("error")) {
                    callback("Ollama error: " + obj["error"].toString() + " — Click '⬇ Pull / Download Selected Model' in Settings.", false);
                } else {
                    QString resp = obj["response"].toString();
                    callback(resp, !resp.isEmpty());
                }
            } else {
                QJsonObject errObj = QJsonDocument::fromJson(raw).object();
                if (errObj.contains("error")) {
                    callback("Model '" + ollamaModel + "' is not downloaded yet (" + errObj["error"].toString() + "). Click '⬇ Pull / Download Selected Model' in Settings!", false);
                } else {
                    callback("Cannot reach Ollama server at " + ollamaEndpoint + " (" + reply->errorString() + "). Click '🚀 Start Local Ollama Server' in Settings!", false);
                }
            }
            reply->deleteLater();
        });
    }

    QString getConfigDir() const {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/BraveLite";
        QDir().mkpath(dir);
        return dir;
    }

    void setupBrowserProfile() {
        if (m_isIncognito) {
            m_profile = new QWebEngineProfile(this);
        } else {
            static QWebEngineProfile *sharedPersistentProfile = nullptr;
            if (!sharedPersistentProfile) {
                sharedPersistentProfile = new QWebEngineProfile("BraveLiteProfile", qApp);
                QString storagePath = getConfigDir() + "/Storage";
                QString cachePath = getConfigDir() + "/Cache";
                QDir().mkpath(storagePath);
                QDir().mkpath(cachePath);

                sharedPersistentProfile->setPersistentStoragePath(storagePath);
                sharedPersistentProfile->setCachePath(cachePath);
                sharedPersistentProfile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);
                sharedPersistentProfile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
            }
            m_profile = sharedPersistentProfile;
        }

        m_profile->setUrlRequestInterceptor(m_adBlocker);
        connect(m_profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload, Qt::UniqueConnection);

        QWebEngineSettings *settings = m_profile->settings();
        settings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
        settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
        settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        settings->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);

        if (!m_isIncognito && m_profile->scripts()->find("BraveLiteCosmeticAndYTShield").isEmpty()) {
            QWebEngineScript shieldScript;
            shieldScript.setName("BraveLiteCosmeticAndYTShield");
            shieldScript.setInjectionPoint(QWebEngineScript::DocumentReady);
            shieldScript.setWorldId(QWebEngineScript::MainWorld);
            shieldScript.setRunsOnSubFrames(false);

            // Debloated 0% Idle CPU AdBlock & YouTube Shield: Uses pure CSS + native HTML5 video events instead of 100ms polling loops
            QString jsCode =
            "(function() {\n"
            "  try {\n"
            "    const root = document.head || document.documentElement;\n"
            "    if (root && !document.getElementById('__bravelite_cosmetic')) {\n"
            "      const s = document.createElement('style');\n"
            "      s.id = '__bravelite_cosmetic';\n"
            "      s.textContent = '.adbox, .banner_ads, .adsbox, .textads, div[id*=\"google_ads\"], div[id*=\"ad-slot\"], #ad-container, .adsbygoogle, ytd-ad-slot-renderer, ytd-in-feed-ad-layout-renderer, ytd-banner-promo-renderer, ytd-statement-banner-renderer, ytd-masthead-ad-v3-renderer, ytd-primetime-promo-renderer, ytd-engagement-panel-section-list-renderer[target-id=\"engagement-panel-ads\"], ytd-merch-shelf-renderer, ytd-action-companion-ad-renderer, .ytd-mealbar-promo-renderer, .ytp-ad-overlay-container, .ytp-ad-message-container, .ytp-ad-image-overlay, .ytp-ad-text-overlay, .ytp-ad-player-overlay-layout, #masthead-ad, #player-ads, .video-ads { display: none !important; visibility: hidden !important; height: 0 !important; width: 0 !important; pointer-events: none !important; }';\n"
            "      root.appendChild(s);\n"
            "    }\n"
            "    if (window.location.hostname.includes('youtube.com')) {\n"
            "      let userMuted = false;\n"
            "      const checkVideoAd = () => {\n"
            "        if (document.hidden) return;\n"
            "        const skipBtns = document.querySelectorAll('.ytp-ad-skip-button, .ytp-ad-skip-button-modern, .ytp-skip-ad-button, .videoAdUiSkipButton, .ytp-ad-overlay-close-button');\n"
            "        for (let i = 0; i < skipBtns.length; i++) skipBtns[i].click();\n"
            "        const player = document.querySelector('#movie_player, .html5-video-player');\n"
            "        const video = document.querySelector('video.html5-main-video, video');\n"
            "        if (!player || !video) return;\n"
            "        const isAd = player.classList.contains('ad-showing') || player.classList.contains('ad-interrupting');\n"
            "        if (isAd) {\n"
            "          video.muted = true;\n"
            "          if (Number.isFinite(video.duration) && video.duration > 0.5) video.currentTime = video.duration - 0.05;\n"
            "          else video.playbackRate = 16.0;\n"
            "          if (typeof player.skipAd === 'function') { try { player.skipAd(); } catch(e) {} }\n"
            "        } else if (video.playbackRate === 16.0) {\n"
            "          video.playbackRate = 1.0;\n"
            "          video.muted = userMuted;\n"
            "        } else {\n"
            "          userMuted = video.muted;\n"
            "        }\n"
            "      };\n"
            "      document.addEventListener('play', checkVideoAd, true);\n"
            "      document.addEventListener('loadedmetadata', checkVideoAd, true);\n"
            "      document.addEventListener('durationchange', checkVideoAd, true);\n"
            "    }\n"
            "  } catch (e) {}\n"
            "})();";

        shieldScript.setSourceCode(jsCode);
        m_profile->scripts()->insert(shieldScript);
        }
    }

    QString getFaviconCacheFile(const QUrl &url) const {
        QString favDir = getConfigDir() + "/favicons";
        QDir().mkpath(favDir);
        QString key = url.host().isEmpty() ? url.toString() : url.host().toLower();
        QString hash = QString::fromLatin1(QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Md5).toHex());
        return favDir + "/" + hash + ".png";
    }

    QIcon loadCachedFavicon(const QUrl &url) const {
        QString cacheFile = getFaviconCacheFile(url);
        if (QFile::exists(cacheFile)) {
            QPixmap px(cacheFile);
            if (!px.isNull()) {
                return QIcon(px);
            }
        }
        return QIcon();
    }

    void preloadBookmarkFavicon(int index) {
        if (index < 0 || index >= bookmarksList.size()) return;
        QUrl url(bookmarksList[index].url);
        if (!url.isValid() || url.host().isEmpty()) return;

        QIcon cached = loadCachedFavicon(url);
        if (!cached.isNull()) {
            bookmarksList[index].icon = cached;
            return;
        }

        QString hostKey = url.host().toLower();
        if (m_pendingFaviconHosts.contains(hostKey)) return;
        m_pendingFaviconHosts.insert(hostKey);

        QUrl favUrl(QString("https://www.google.com/s2/favicons?sz=64&domain_url=%1")
        .arg(QString::fromUtf8(QUrl::toPercentEncoding(url.toString()))));

        QNetworkRequest req(favUrl);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        req.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

        QNetworkReply *reply = m_netManager->get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply, url, hostKey]() {
            m_pendingFaviconHosts.remove(hostKey);
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray data = reply->readAll();
                QPixmap px;
                if (px.loadFromData(data) && !px.isNull()) {
                    px.save(getFaviconCacheFile(url), "PNG");
                    QIcon newIcon(px);
                    bool updated = false;
                    for (auto &bm : bookmarksList) {
                        if (QUrl(bm.url).host().toLower() == hostKey) {
                            bm.icon = newIcon;
                            updated = true;
                        }
                    }
                    if (updated) {
                        renderBookmarks();
                    }
                }
            }
            reply->deleteLater();
        });
    }

    void preloadAllBookmarkIcons() {
        bool anyLoadedFromCache = false;
        for (int i = 0; i < bookmarksList.size(); ++i) {
            if (bookmarksList[i].icon.isNull()) {
                QIcon cached = loadCachedFavicon(QUrl(bookmarksList[i].url));
                if (!cached.isNull()) {
                    bookmarksList[i].icon = cached;
                    anyLoadedFromCache = true;
                } else {
                    preloadBookmarkFavicon(i);
                }
            }
        }
        if (anyLoadedFromCache) {
            renderBookmarks();
        }
    }

    void setupPasswordClickPromptForView(CustomWebView *view) {
        if (!view || !view->page() || !autofillEnabled) return;

        QString host = view->url().host().toLower();
        if (host.isEmpty()) return;

        bool hasSavedForSite = false;
        for (const auto &cred : savedPasswords) {
            QString savedHost = cred.site.toLower();
            if (savedHost == host || host.endsWith("." + savedHost) || savedHost.endsWith("." + host)) {
                hasSavedForSite = true;
                break;
            }
        }
        if (!hasSavedForSite) return;

        view->page()->runJavaScript(R"(
            (function() {
                if (window.__bravelitePromptHooked) return;
                window.__bravelitePromptHooked = true;

                document.addEventListener('mousedown', function(e) {
                    const el = e.target.closest('input');
                    if (!el || el.disabled || el.readOnly) return;

                    const type = (el.getAttribute('type') || 'text').toLowerCase();
                    const isPass = (type === 'password');
                    const isUser = (type === 'email' || type === 'text' || type === 'tel');

                    if (!isPass && !isUser) return;

                    const passField = document.querySelector('input[type="password"]');
                    if (!passField) return;

                    if (el.value && passField.value) return;

                    const now = Date.now();
                    if (window.__lastAutofillPromptTime && (now - window.__lastAutofillPromptTime < 1500)) return;
                    window.__lastAutofillPromptTime = now;

                    console.log("BRAVELITE_AUTOFILL_PROMPT::" + window.location.hostname);
                }, true);
            })();
        )");
    }

    void fillCredentialsIntoView(CustomWebView *view, const SavedCredential &cred) {
        if (!view || !view->page()) return;

        QJsonObject o;
        o["user"] = cred.username;
        o["pass"] = cred.password;
        QString credJson = QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));

        QString script = QString(R"(
            (function(cred) {
                function setNativeInputValue(el, val) {
                    if (!el) return;
                    el.focus();
                    const proto = window.HTMLInputElement.prototype;
                    const descriptor = Object.getOwnPropertyDescriptor(proto, 'value');
                    if (descriptor && descriptor.set) {
                        descriptor.set.call(el, val);
                    } else {
                        el.value = val;
                    }
                    el.dispatchEvent(new Event('input', { bubbles: true }));
                    el.dispatchEvent(new Event('change', { bubbles: true }));
                }

                const passInputs = document.querySelectorAll('input[type="password"]');
                for (let i = 0; i < passInputs.length; i++) {
                    const passEl = passInputs[i];
                    if (passEl.offsetParent === null) continue;
                    const container = passEl.closest('form') || document.body;
                    const inputs = Array.from(container.querySelectorAll(
                        'input[type="email"], input[type="text"], input[type="tel"], input:not([type])'
                    ));
                    for (let j = inputs.length - 1; j >= 0; j--) {
                        const inp = inputs[j];
                        if (inp !== passEl && inp.offsetParent !== null && !inp.disabled && !inp.readOnly) {
                            if (cred.user) setNativeInputValue(inp, cred.user);
                            break;
                        }
                    }
                    setNativeInputValue(passEl, cred.pass);
                    break;
                }
            })(%1);
        )").arg(credJson);

        view->page()->runJavaScript(script);
    }

    void enterBrowserFullScreen() {
        if (!isFullScreen()) {
            m_inFullScreenTransition = true;
            m_wasMaximizedBeforeFullScreen = isMaximized() || (windowState() & Qt::WindowMaximized);
            headerContainer->hide();
            showFullScreen();
            QTimer::singleShot(150, this, [this]() {
                m_inFullScreenTransition = false;
            });
        }
    }

    void exitBrowserFullScreen() {
        m_inFullScreenTransition = true;
        headerContainer->show();

        if (m_wasMaximizedBeforeFullScreen) {
            showNormal();
            QTimer::singleShot(10, this, [this]() {
                showMaximized();
                setWindowState((windowState() & ~Qt::WindowFullScreen) | Qt::WindowMaximized | Qt::WindowActive);
                m_inFullScreenTransition = false;
            });
        } else {
            showNormal();
            setWindowState(windowState() & ~Qt::WindowFullScreen);
            m_inFullScreenTransition = false;
        }
    }

    void updateShieldButtonAppearance() {
        bool active = m_adBlocker->isEnabled();
        if (active) {
            btnShield->setText("🛡 ON");
            btnShield->setToolTip("BraveLite Shield: ON (Blocking ads, trackers & spam redirects)");
            btnShield->setStyleSheet(
                "QPushButton { background-color: #2b211d; border: 1px solid #ff5500; border-radius: 6px; "
                "color: #ff6a22; font-size: 12px; font-weight: bold; padding: 4px 10px; } "
                "QPushButton:hover { background-color: #ff5500; color: #ffffff; }"
            );
        } else {
            btnShield->setText("🛡 OFF");
            btnShield->setToolTip("BraveLite Shield: OFF (Click to enable protection)");
            btnShield->setStyleSheet(
                "QPushButton { background-color: #25272b; border: 1px solid #3e4146; border-radius: 6px; "
                "color: #8e9297; font-size: 12px; font-weight: bold; padding: 4px 10px; } "
                "QPushButton:hover { background-color: #35373c; color: #ffffff; }"
            );
        }
    }

    void applyThemeProfile(const QString &profileName) {
        QString bgMain = "#1a1a1e";
        QString bgHeader = "#161719";
        QString bgInput = "#2b2d30";
        QString bgTab = "#222428";
        QString bgTabSel = "#2d3035";
        QString accent = "#ff5500";
        QString textCol = "#dfdfdf";

        if (profileName == "Midnight OLED") {
            bgMain = "#000000";
            bgHeader = "#050507";
            bgInput = "#121316";
            bgTab = "#0d0e11";
            bgTabSel = "#1a1c22";
            accent = "#ff5500";
        } else if (profileName == "Cyberpunk Neon") {
            bgMain = "#0d0914";
            bgHeader = "#09060e";
            bgInput = "#191224";
            bgTab = "#140e1d";
            bgTabSel = "#241734";
            accent = "#00f0ff";
            textCol = "#f0e6ff";
        } else if (profileName == "Nordic Frost") {
            bgMain = "#2e3440";
            bgHeader = "#242933";
            bgInput = "#3b4252";
            bgTab = "#2e3440";
            bgTabSel = "#434c5e";
            accent = "#88c0d0";
            textCol = "#eceff4";
        } else if (profileName == "Catppuccin Mocha") {
            bgMain = "#1e1e2e";
            bgHeader = "#181825";
            bgInput = "#313244";
            bgTab = "#1e1e2e";
            bgTabSel = "#45475a";
            accent = "#cba6f7";
            textCol = "#cdd6f4";
        }

        setStyleSheet(QString(R"(
            QMainWindow { background-color: %1; }
            QWidget { background-color: %1; color: %7; font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, Roboto, Ubuntu, sans-serif; }
            QWidget#headerContainer { background-color: %2; }
            QLineEdit {
                background-color: %3;
                border: 1px solid #3e4146;
                border-radius: 15px;
                padding: 6px 14px;
                color: #ffffff;
                font-size: 13px;
                selection-background-color: %6;
            }
            QLineEdit:focus { border: 1px solid %6; background-color: %2; }
            QPushButton {
                background-color: %3;
                border: 1px solid #34373d;
                border-radius: 6px;
                padding: 5px 10px;
                color: %7;
                font-size: 13px;
            }
            QPushButton:hover {
                background-color: #35373c;
                border-color: %6;
                color: #ffffff;
            }
            QToolBar {
                background-color: %1;
                border: none;
                border-bottom: 1px solid #2a2c30;
                padding: 3px 8px;
                spacing: 4px;
            }
            QToolBar QToolButton {
                background-color: transparent;
                border: 1px solid transparent;
                border-radius: 5px;
                padding: 4px 8px;
                color: %7;
                font-size: 12px;
            }
            QToolBar QToolButton:hover {
                background-color: %5;
                border-color: %6;
                color: #ffffff;
            }
            QTabBar::tab {
                background: %4;
                color: #9aa0a6;
                padding: 7px 18px 7px 12px;
                margin-right: 4px;
                border-top-left-radius: 8px;
                border-top-right-radius: 8px;
                min-width: 120px;
                max-width: 200px;
                font-size: 12px;
            }
            QTabBar::tab:selected {
                background: %5;
                color: #ffffff;
                font-weight: 600;
                border-bottom: 2px solid %6;
            }
            QTabBar::tab:hover:!selected {
                background: %3;
            }
            QTabBar::close-button {
                image: none;
                background-color: transparent;
                border: none;
                min-width: 16px;
                max-width: 16px;
                min-height: 16px;
                max-height: 16px;
            }
            QProgressBar {
                border: none;
                background: transparent;
                height: 2px;
            }
            QProgressBar::chunk {
                background-color: %6;
            }
        )").arg(bgMain, bgHeader, bgInput, bgTab, bgTabSel, accent, textCol));

        if (btnShield) updateShieldButtonAppearance();
    }

    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        headerContainer = new QWidget(this);
        headerContainer->setObjectName("headerContainer");
        QVBoxLayout *headerLayout = new QVBoxLayout(headerContainer);
        headerLayout->setContentsMargins(0, 2, 0, 0);
        headerLayout->setSpacing(0);

        QWidget *tabRow = new QWidget(this);
        QHBoxLayout *tabRowLayout = new QHBoxLayout(tabRow);
        tabRowLayout->setContentsMargins(8, 0, 8, 0);
        tabRowLayout->setSpacing(4);

        tabBar = new CustomTabBar(this);
        tabBar->setTabsClosable(true);
        tabBar->setMovable(true);
        connect(tabBar, &QTabBar::tabCloseRequested, this, &BrowserWindow::closeTab);
        connect(tabBar, &QTabBar::currentChanged, this, &BrowserWindow::tabSwitched);
        connect(tabBar, &CustomTabBar::tabDetachRequested, this, &BrowserWindow::handleTabDetach);

        QPushButton *newTabBtn = new QPushButton("+", this);
        newTabBtn->setFixedSize(28, 28);
        newTabBtn->setCursor(Qt::PointingHandCursor);
        newTabBtn->setToolTip("New tab (Ctrl+T)");
        newTabBtn->setStyleSheet("QPushButton { font-size: 16px; font-weight: bold; color: #c8ccd0; background: #242629; border: 1px solid #34373d; border-radius: 6px; padding: 0; } QPushButton:hover { background-color: #35373c; color: #fff; }");
        connect(newTabBtn, &QPushButton::clicked, this, [this]() { addNewTab(); });

        tabRowLayout->addWidget(tabBar);
        tabRowLayout->addWidget(newTabBtn);
        tabRowLayout->addStretch();

        QWidget *navBar = new QWidget(this);
        QHBoxLayout *navLayout = new QHBoxLayout(navBar);
        navLayout->setContentsMargins(8, 5, 8, 5);
        navLayout->setSpacing(6);

        QString navBtnStyle = "QPushButton { background-color: #25272b; border: 1px solid #34373d; border-radius: 6px; padding: 4px 10px; color: #e4e6eb; font-size: 14px; font-weight: bold; } QPushButton:hover { background-color: #35373c; border-color: #4b4f57; color: #fff; }";

        QPushButton *btnBack = new QPushButton("←", this);
        btnBack->setToolTip("Back (Alt+Left)");
        btnBack->setCursor(Qt::PointingHandCursor);
        btnBack->setStyleSheet(navBtnStyle);
        connect(btnBack, &QPushButton::clicked, [this]() {
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->back();
        });

            QPushButton *btnForward = new QPushButton("→", this);
            btnForward->setToolTip("Forward (Alt+Right)");
            btnForward->setCursor(Qt::PointingHandCursor);
            btnForward->setStyleSheet(navBtnStyle);
            connect(btnForward, &QPushButton::clicked, [this]() {
                if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->forward();
            });

                QPushButton *btnReload = new QPushButton("↻", this);
                btnReload->setToolTip("Reload (Ctrl+R)");
                btnReload->setCursor(Qt::PointingHandCursor);
                btnReload->setStyleSheet(navBtnStyle);
                connect(btnReload, &QPushButton::clicked, [this]() {
                    if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->reload();
                });

                    urlBar = new QLineEdit(this);
                    urlBar->setPlaceholderText("Search with BraveLite Engine or enter address");
                    connect(urlBar, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlSubmit);

                    QPushButton *btnBookmark = new QPushButton("★", this);
                    btnBookmark->setToolTip("Bookmark this page");
                    btnBookmark->setCursor(Qt::PointingHandCursor);
                    btnBookmark->setStyleSheet(navBtnStyle);
                    connect(btnBookmark, &QPushButton::clicked, this, &BrowserWindow::promptAddBookmark);

                    btnPasswords = new QPushButton("🔑", this);
                    btnPasswords->setToolTip("Password Manager & Autofill");
                    btnPasswords->setCursor(Qt::PointingHandCursor);
                    btnPasswords->setStyleSheet(navBtnStyle);
                    connect(btnPasswords, &QPushButton::clicked, this, &BrowserWindow::openPasswordManagerDialog);

                    btnShield = new QPushButton(this);
                    btnShield->setCursor(Qt::PointingHandCursor);
                    updateShieldButtonAppearance();
                    connect(btnShield, &QPushButton::clicked, this, &BrowserWindow::toggleShield);

                    btnPrivacyGrade = new QPushButton("A", this);
                    btnPrivacyGrade->setToolTip("Site Privacy Grade: A");
                    btnPrivacyGrade->setCursor(Qt::PointingHandCursor);
                    btnPrivacyGrade->setStyleSheet("QPushButton { background-color: #52c41a; color: white; font-weight: bold; border-radius: 5px; padding: 3px 8px; font-size: 11px; border: none; }");
                    btnPrivacyGrade->hide();
                    connect(btnPrivacyGrade, &QPushButton::clicked, this, &BrowserWindow::showPrivacyDetails);

                    QPushButton *btnZoomOut = new QPushButton("−", this);
                    btnZoomOut->setToolTip("Zoom Out");
                    btnZoomOut->setCursor(Qt::PointingHandCursor);
                    btnZoomOut->setStyleSheet(navBtnStyle);
                    connect(btnZoomOut, &QPushButton::clicked, this, &BrowserWindow::zoomOut);

                    QPushButton *btnZoomIn = new QPushButton("+", this);
                    btnZoomIn->setToolTip("Zoom In");
                    btnZoomIn->setCursor(Qt::PointingHandCursor);
                    btnZoomIn->setStyleSheet(navBtnStyle);
                    connect(btnZoomIn, &QPushButton::clicked, this, &BrowserWindow::zoomIn);

                    btnDownload = new QPushButton("⤓", this);
                    btnDownload->setToolTip("Downloads (Ctrl+J)");
                    btnDownload->setCursor(Qt::PointingHandCursor);
                    btnDownload->setStyleSheet(navBtnStyle);
                    connect(btnDownload, &QPushButton::clicked, this, &BrowserWindow::openDownloadsPopup);

                    btnMenu = new QPushButton("☰", this);
                    btnMenu->setToolTip("BraveLite Menu");
                    btnMenu->setCursor(Qt::PointingHandCursor);
                    btnMenu->setStyleSheet(navBtnStyle);
                    connect(btnMenu, &QPushButton::clicked, this, &BrowserWindow::openAppMenu);

                    navLayout->addWidget(btnBack);
                    navLayout->addWidget(btnForward);
                    navLayout->addWidget(btnReload);
                    navLayout->addWidget(btnPrivacyGrade);
                    navLayout->addWidget(urlBar, 1);
                    navLayout->addWidget(btnBookmark);
                    navLayout->addWidget(btnPasswords);
                    navLayout->addWidget(btnShield);
                    navLayout->addWidget(btnZoomOut);
                    navLayout->addWidget(btnZoomIn);
                    navLayout->addWidget(btnDownload);
                    navLayout->addWidget(btnMenu);

                    bookmarksBar = new QToolBar("Bookmarks", this);
                    bookmarksBar->setMovable(false);
                    bookmarksBar->setIconSize(QSize(16, 16));
                    bookmarksBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                    renderBookmarks();

                    progressBar = new QProgressBar(this);
                    progressBar->setMaximumHeight(2);
                    progressBar->setTextVisible(false);

                    headerLayout->addWidget(tabRow);
                    headerLayout->addWidget(navBar);
                    headerLayout->addWidget(bookmarksBar);
                    headerLayout->addWidget(progressBar);

                    tabStack = new QStackedWidget(this);

                    mainLayout->addWidget(headerContainer);
                    mainLayout->addWidget(tabStack, 1);

                    setCentralWidget(centralWidget);
    }

    // Debloated Start Page: Updates clock only once per minute (0% idle CPU usage) and pauses when tab is hidden
    QString getNewTabHtml() {
        QString now = QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy • hh:mm AP");
        return QString(R"(
            <!DOCTYPE html>
            <html>
            <head>
                <meta charset="utf-8">
                <title>New Tab</title>
                <style>
                    * { box-sizing: border-box; }
                    body {
                        margin: 0; background: #1c1c1c; color: #f0f0f0;
                        font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, Roboto, sans-serif;
                        display: flex; flex-direction: column; align-items: center; justify-content: center;
                        height: 82vh; text-align: center;
                    }
                    .hero-wrapper {
                        width: 580px; max-width: 92vw;
                        display: flex; flex-direction: column; align-items: center; justify-content: center;
                    }
                    h1 {
                        width: 100%; text-align: center;
                        font-size: 3.2rem; margin: 0 0 24px 0; font-weight: 600; letter-spacing: -0.5px;
                    }
                    .search-container {
                        width: 100%;
                        background: #2b2b2b; border: 1px solid #3d3d3d;
                        border-radius: 28px; padding: 6px 18px;
                        display: flex; align-items: center; box-shadow: 0 6px 18px rgba(0,0,0,0.28);
                        margin-bottom: 24px;
                    }
                    .search-container:focus-within {
                        border-color: #de5833;
                    }
                    .search-container input {
                        width: 100%; border: none; background: transparent;
                        color: #ffffff; font-size: 16px; outline: none; padding: 8px 8px;
                        text-align: center;
                    }
                    .datetime {
                        font-size: 1.15rem; color: #a0a4ab; margin-bottom: 8px; font-weight: 500;
                    }
                    .tagline {
                        font-size: 0.95rem; color: #de5833; letter-spacing: 0.6px; font-style: italic; opacity: 0.95;
                    }
                    </style>
                    </head>
                    <body>
                    <div class="hero-wrapper">
                    <h1>Welcome!</h1>
                    <form onsubmit="event.preventDefault(); console.log('BRAVELITE_SEARCH::' + document.getElementById('startSearch').value);" class="search-container">
                    <input id="startSearch" type="text" name="q" placeholder="Search privately with BraveLite Engine or enter address..." autofocus autocomplete="off" />
                    </form>
                    <div class="datetime" id="liveClock">%1</div>
                    <div class="tagline">because privacy matters</div>
                    </div>
                    <script>
                    function updateClock() {
                        if (document.hidden) return;
                        const now = new Date();
                        const dateOpts = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' };
                        const timeOpts = { hour: '2-digit', minute: '2-digit' };
                        document.getElementById('liveClock').textContent =
                        now.toLocaleDateString('en-US', dateOpts) + ' • ' + now.toLocaleTimeString('en-US', timeOpts);
                    }
                    updateClock();
                    setInterval(updateClock, 30000);
                    </script>
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
                if (urlStr.startsWith("data:") || v->url().scheme() == "about" || v->url().host() == "search.bravelite.local") {
                    if (v->url().host() != "search.bravelite.local") urlBar->clear();
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

        if (text.startsWith("http://") || text.startsWith("https://")) {
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->load(QUrl(text));
            else addNewTab(QUrl(text));
        } else if (text.contains(".") && !text.contains(" ")) {
            QUrl target("https://" + text);
            if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->load(target);
            else addNewTab(target);
        } else {
            if (defaultSearchEngine == "BraveLite Engine") {
                runBraveLiteSearch(text);
            } else {
                QUrl target("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(text));
                if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) v->load(target);
                else addNewTab(target);
            }
        }
    }

    void toggleShield() {
        bool next = !m_adBlocker->isEnabled();
        m_adBlocker->setEnabled(next);
        updateShieldButtonAppearance();
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

            QIcon pageIcon = v->icon();
        if (!pageIcon.isNull()) {
            QPixmap px = pageIcon.pixmap(32, 32);
            if (!px.isNull()) {
                px.save(getFaviconCacheFile(QUrl(url)), "PNG");
            }
        } else {
            pageIcon = loadCachedFavicon(QUrl(url));
        }

        bookmarksList.append({title, url, pageIcon});
        int addedIdx = bookmarksList.size() - 1;
        savePersistentData();
        renderBookmarks();
        preloadBookmarkFavicon(addedIdx);
    }

    void renderBookmarks() {
        bookmarksBar->clear();
        for (int i = 0; i < bookmarksList.size(); ++i) {
            Bookmark bm = bookmarksList[i];

            QString shortTitle = bm.title.length() > 16 ? bm.title.left(16) : bm.title;
            QIcon displayIcon = bm.icon.isNull() ? style()->standardIcon(QStyle::SP_DriveNetIcon) : bm.icon;

            BookmarkToolButton *btn = new BookmarkToolButton(i, bookmarksBar);
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setIcon(displayIcon);
            btn->setIconSize(QSize(16, 16));
            btn->setText(shortTitle);
            btn->setToolTip(bm.title + "\n" + bm.url + "\n(Drag to reorder • Middle-click for new tab)");
            btn->setCursor(Qt::PointingHandCursor);

            connect(btn, &QToolButton::clicked, this, [this, bm]() {
                if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
                    v->load(QUrl(bm.url));
                } else {
                    addNewTab(QUrl(bm.url));
                }
            });

            connect(btn, &BookmarkToolButton::middleClicked, this, [this, bm]() {
                addNewTab(QUrl(bm.url));
            });

            connect(btn, &BookmarkToolButton::bookmarkMoved, this, [this](int fromIdx, int toIdx) {
                if (fromIdx >= 0 && fromIdx < bookmarksList.size() &&
                    toIdx >= 0 && toIdx < bookmarksList.size() && fromIdx != toIdx) {
                    bookmarksList.move(fromIdx, toIdx);
                savePersistentData();
                renderBookmarks();
                    }
            });

            btn->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(btn, &QToolButton::customContextMenuRequested, this, [this, i, btn](const QPoint &pos) {
                QMenu menu(btn);
                menu.setStyleSheet("QMenu { background: #242529; color: #fff; border: 1px solid #3c4043; } QMenu::item { padding: 6px 18px; } QMenu::item:selected { background: #35373c; }");
                QAction *openNewTabAct = menu.addAction("Open in new tab");
                menu.addSeparator();
                QAction *editAct = menu.addAction("Edit bookmark");
                QAction *delAct = menu.addAction("Delete bookmark");

                QAction *chosen = menu.exec(btn->mapToGlobal(pos));
                if (chosen == openNewTabAct && i < bookmarksList.size()) {
                    addNewTab(QUrl(bookmarksList[i].url));
                } else if (chosen == editAct && i < bookmarksList.size()) {
                    bool ok = false;
                    QString nTitle = QInputDialog::getText(this, "Edit Bookmark", "Name:", QLineEdit::Normal, bookmarksList[i].title, &ok);
                    if (ok && !nTitle.isEmpty()) bookmarksList[i].title = nTitle;

                    QString nUrl = QInputDialog::getText(this, "Edit Bookmark", "URL:", QLineEdit::Normal, bookmarksList[i].url, &ok);
                    if (ok && !nUrl.isEmpty()) {
                        if (!nUrl.startsWith("http://") && !nUrl.startsWith("https://")) nUrl = "https://" + nUrl;
                            bookmarksList[i].url = nUrl;
                        bookmarksList[i].icon = loadCachedFavicon(QUrl(nUrl));
                    }
                    savePersistentData();
                    renderBookmarks();
                    preloadBookmarkFavicon(i);
                } else if (chosen == delAct && i < bookmarksList.size()) {
                    bookmarksList.removeAt(i);
                    savePersistentData();
                    renderBookmarks();
                }
            });

            bookmarksBar->addWidget(btn);
        }
    }

    QString getStorageFile() const {
        return getConfigDir() + "/data.json";
    }

    void loadPersistentData() {
        QFile file(getStorageFile());
        if (!file.open(QIODevice::ReadOnly)) return;

        QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        autofillEnabled = root.value("autofillEnabled").toBool(true);
        startupContinueWhereLeftOff = root.value("startupContinueWhereLeftOff").toBool(false);
        m_isDefaultBrowserSaved = root.value("isDefaultBrowser").toBool(false);
        currentThemeProfile = root.value("themeProfile").toString("BraveLite Dark");
        defaultSearchEngine = root.value("defaultSearchEngine").toString("BraveLite Engine");
        if (defaultSearchEngine == "BraveLite Browser") defaultSearchEngine = "BraveLite Engine";

        aiEnabled = root.value("aiEnabled").toBool(true);
        aiSearchOverviewEnabled = root.value("aiSearchOverviewEnabled").toBool(true);
        aiHoverSummaryEnabled = root.value("aiHoverSummaryEnabled").toBool(true);
        aiUseGpu = root.value("aiUseGpu").toBool(true);
        ollamaEndpoint = root.value("ollamaEndpoint").toString("http://127.0.0.1:11434");
        ollamaModel = root.value("ollamaModel").toString("smollm2:360m").toLower();
        aiSystemPrompt = root.value("aiSystemPrompt").toString(aiSystemPrompt);

        if (root.contains("keybinds")) {
            QJsonObject kbObj = root["keybinds"].toObject();
            for (auto it = kbObj.begin(); it != kbObj.end(); ++it) {
                keybindsMap[it.key()] = it.value().toString();
            }
        }

        lastSessionUrls.clear();
        for (auto v : root["lastSessionUrls"].toArray()) {
            QString u = v.toString();
            if (!u.isEmpty() && !u.startsWith("data:") && !u.startsWith("about:") && !u.contains("bravelite.local")) {
                lastSessionUrls.append(u);
            }
        }

        historyList.clear();
        for (auto v : root["history"].toArray()) historyList.append(v.toString());

        bookmarksList.clear();
        for (auto v : root["bookmarks"].toArray()) {
            QJsonObject o = v.toObject();
            QString url = o["url"].toString();
            bookmarksList.append({o["title"].toString(), url, loadCachedFavicon(QUrl(url))});
        }

        savedPasswords.clear();
        for (auto v : root["passwords"].toArray()) {
            QJsonObject o = v.toObject();
            QString pass = o["pass"].toString();
            if (o["encoded"].toBool(false)) {
                pass = QString::fromUtf8(QByteArray::fromBase64(pass.toUtf8()));
            }
            savedPasswords.append({o["site"].toString(), o["user"].toString(), pass});
        }

        qDeleteAll(downloadsList);
        downloadsList.clear();
        for (auto v : root["downloads"].toArray()) {
            QJsonObject o = v.toObject();
            DownloadItem *it = new DownloadItem(this);
            it->fileName = o["fileName"].toString();
            it->url = o["url"].toString();
            it->path = o["path"].toString();
            it->totalBytes = o["totalBytes"].toVariant().toLongLong();
            it->receivedBytes = o["receivedBytes"].toVariant().toLongLong();
            it->isFinished = o["isFinished"].toBool();
            it->isCanceled = o["isCanceled"].toBool(false);
            downloadsList.append(it);
        }
    }

    void savePersistentData() {
        if (m_isIncognito) return;
        QFile file(getStorageFile());
        if (!file.open(QIODevice::WriteOnly)) return;

        QJsonObject root;
        root["autofillEnabled"] = autofillEnabled;
        root["startupContinueWhereLeftOff"] = startupContinueWhereLeftOff;
        root["isDefaultBrowser"] = m_isDefaultBrowserSaved;
        root["themeProfile"] = currentThemeProfile;
        root["defaultSearchEngine"] = defaultSearchEngine;

        root["aiEnabled"] = aiEnabled;
        root["aiSearchOverviewEnabled"] = aiSearchOverviewEnabled;
        root["aiHoverSummaryEnabled"] = aiHoverSummaryEnabled;
        root["aiUseGpu"] = aiUseGpu;
        root["ollamaEndpoint"] = ollamaEndpoint;
        root["ollamaModel"] = ollamaModel.toLower();
        root["aiSystemPrompt"] = aiSystemPrompt;

        QJsonObject kbObj;
        for (auto it = keybindsMap.begin(); it != keybindsMap.end(); ++it) {
            kbObj[it.key()] = it.value();
        }
        root["keybinds"] = kbObj;

        QJsonArray sessionArr;
        if (tabStack) {
            for (int i = 0; i < tabStack->count(); ++i) {
                if (auto *v = qobject_cast<CustomWebView*>(tabStack->widget(i))) {
                    QString u = v->url().toString();
                    if (!u.isEmpty() && !u.startsWith("data:") && v->url().scheme() != "about" && !u.contains("bravelite.local")) {
                        sessionArr.append(u);
                    }
                }
            }
        }
        root["lastSessionUrls"] = sessionArr;

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
            o["pass"] = QString::fromUtf8(p.password.toUtf8().toBase64());
            o["encoded"] = true;
            passArr.append(o);
        }
        root["passwords"] = passArr;

        QJsonArray dlArr;
        for (const DownloadItem *d : downloadsList) {
            QJsonObject o;
            o["fileName"] = d->fileName;
            o["url"] = d->url;
            o["path"] = d->path;
            o["totalBytes"] = d->totalBytes;
            o["receivedBytes"] = d->receivedBytes;
            o["isFinished"] = d->isFinished;
            o["isCanceled"] = d->isCanceled;
            dlArr.append(o);
        }
        root["downloads"] = dlArr;

        file.write(QJsonDocument(root).toJson());
    }

    void bindWebViewSignals(CustomWebView *view) {
        CustomWebEnginePage *p = qobject_cast<CustomWebEnginePage*>(view->page());
        if (p) {
            p->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

            connect(p, &QWebEnginePage::fullScreenRequested, this, [this](QWebEngineFullScreenRequest request) {
                request.accept();
                if (request.toggleOn()) {
                    enterBrowserFullScreen();
                } else {
                    exitBrowserFullScreen();
                }
            });

            connect(p, &CustomWebEnginePage::linkHoverSummarizeRequested, this, &BrowserWindow::handleLinkHoverSummarize);
            connect(p, &CustomWebEnginePage::braveliteSearchRequested, this, [this](const QString &q) {
                if (defaultSearchEngine == "BraveLite Engine") {
                    runBraveLiteSearch(q);
                } else {
                    if (auto *v = qobject_cast<CustomWebView*>(tabStack->currentWidget())) {
                        v->load(QUrl("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(q)));
                    }
                }
            });

            connect(view, &QWebEngineView::renderProcessTerminated, this, [view](QWebEnginePage::RenderProcessTerminationStatus status, int exitCode) {
                CrashLogger::writeCrashEntry("WebEngine Render Process Terminated",
                                             QString("URL: %1 | Status: %2 | ExitCode: %3")
                                             .arg(view->url().toString()).arg(status).arg(exitCode));
                if (status != QWebEnginePage::NormalTerminationStatus) {
                    QTimer::singleShot(400, view, [view]() {
                        view->reload();
                    });
                }
            });

            if (!m_isIncognito) {
                connect(p, &CustomWebEnginePage::loginCaptured, this, &BrowserWindow::promptSaveCredentials);
                connect(p, &CustomWebEnginePage::autofillRequested, this, &BrowserWindow::promptAutofillCredentials);

                #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
                connect(p, &QWebEnginePage::permissionRequested, this, [this](QWebEnginePermission perm) {
                    QString name = "Unknown";
                    using PT = QWebEnginePermission::PermissionType;
                    if (perm.permissionType() == PT::MediaAudioCapture) name = "Microphone";
                    else if (perm.permissionType() == PT::MediaVideoCapture) name = "Camera";
                    else if (perm.permissionType() == PT::Geolocation) name = "Location";
                    else if (perm.permissionType() == PT::Notifications) name = "Notifications";

                    int ret = QMessageBox::question(this, "Permission Request",
                                                    QString("%1 requests access to: %2\nAllow?").arg(perm.origin().host(), name));
                    bool grant = (ret == QMessageBox::Yes);
                    if (grant) perm.grant();
                    else perm.deny();
                    sitePermissions.append({perm.origin().toString(), name, grant});
                });
                #else
                connect(p, &QWebEnginePage::featurePermissionRequested, this, [this, p](const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
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
                #endif
            }
        }

        connect(view, &QWebEngineView::titleChanged, this, [this, view](const QString &t) {
            int i = tabStack->indexOf(view);
            if (i != -1) {
                QString cleanTitle = t.trimmed();
                tabBar->setTabText(i, (cleanTitle.isEmpty() || cleanTitle == "about:blank") ? "New Tab" : cleanTitle.left(18));
                tabBar->setTabToolTip(i, t);
            }
        });

        connect(view, &QWebEngineView::iconChanged, this, [this, view](const QIcon &icon) {
            int i = tabStack->indexOf(view);
            if (i != -1) {
                tabBar->setTabIcon(i, icon);
            }
            if (icon.isNull()) return;

            QUrl pageUrl = view->url();
            if (!pageUrl.host().isEmpty() && !pageUrl.host().endsWith(".local")) {
                QPixmap px = icon.pixmap(32, 32);
                if (!px.isNull()) {
                    px.save(getFaviconCacheFile(pageUrl), "PNG");
                }
            }

            bool updated = false;
            for (auto &bm : bookmarksList) {
                if (bm.url == pageUrl.toString() ||
                    (!pageUrl.host().isEmpty() && QUrl(bm.url).host().toLower() == pageUrl.host().toLower())) {
                    bm.icon = icon;
                updated = true;
                    }
            }
            if (updated) {
                renderBookmarks();
            }
        });

        connect(view, &QWebEngineView::loadProgress, this, [this, view](int progress) {
            if (tabStack->currentWidget() == view) {
                progressBar->setValue(progress == 100 ? 0 : progress);
            }
        });

        connect(view, &QWebEngineView::urlChanged, this, [this, view](const QUrl &u) {
            QString urlStr = u.toString();
            if (tabStack->currentWidget() == view) {
                if (urlStr.startsWith("data:") || u.scheme() == "about" || u.host() == "search.bravelite.local") {
                    if (u.host() != "search.bravelite.local") urlBar->clear();
                    btnPrivacyGrade->hide();
                } else {
                    urlBar->setText(urlStr);
                    updatePrivacyBadge(u);
                }
            }
            if (!m_isIncognito && !urlStr.isEmpty() && !urlStr.startsWith("data:") && u.scheme() != "about" && u.host() != "search.bravelite.local") {
                historyList.append(urlStr);
                savePersistentData();
            }
        });

        connect(view, &QWebEngineView::loadFinished, this, [this, view](bool ok) {
            if (ok && !m_isIncognito) {
                view->page()->runJavaScript(R"(
                    (function() {
                        if (!window.__passHooked) {
                            window.__passHooked = true;

                            function captureActiveCredentials() {
                                const passInputs = document.querySelectorAll('input[type="password"]');
                                for (let i = 0; i < passInputs.length; i++) {
                                    const passInput = passInputs[i];
                                    if (!passInput.value) continue;
                                    const container = passInput.closest('form') || document.body;
                                    let username = "";
                                    const userInputs = container.querySelectorAll(
                                        'input[type="email"], input[type="text"], input[type="tel"], input:not([type])'
                                    );
                                    for (let j = userInputs.length - 1; j >= 0; j--) {
                                        const u = userInputs[j];
                                        if (u !== passInput && u.value && u.offsetParent !== null) {
                                            username = u.value;
                                            break;
                                        }
                                    }
                                    const payload = JSON.stringify({
                                        site: window.location.hostname,
                                        user: username,
                                        pass: passInput.value
                                    });
                                    if (window.__lastCapturedPayload !== payload) {
                                        window.__lastCapturedPayload = payload;
                                        console.log("LOGIN_SUBMIT_JSON::" + payload);
                                    }
                                    break;
                                }
                            }

                            document.addEventListener('submit', captureActiveCredentials, true);
                            document.addEventListener('keydown', function(e) {
                                if (e.key === 'Enter') captureActiveCredentials();
                            }, true);
                                document.addEventListener('click', function(e) {
                                    const btn = e.target.closest('button, input[type="submit"], [role="button"]');
                                    if (btn) captureActiveCredentials();
                                }, true);
                        }

                        // Fixed Hover Link Summarizer: Ignores internal child mouseout events so the timer never cancels prematurely
                        if (!window.__braveliteHoverHooked) {
                            window.__braveliteHoverHooked = true;
                            let hoverTimer = null;
                            let activeAnchor = null;
                            let currentHoverUrl = '';

                    function ensureTooltip() {
                        let tt = document.getElementById('__bravelite_ai_tooltip');
                        if (!tt) {
                            tt = document.createElement('div');
                            tt.id = '__bravelite_ai_tooltip';
                    tt.style.cssText = 'position:fixed; z-index:2147483647; max-width:380px; background:linear-gradient(180deg,#1f242d,#181b22); color:#f0f2f5; border:1px solid #4285F4; border-radius:12px; padding:11px 15px; font-family:sans-serif; font-size:12.5px; line-height:1.5; box-shadow:0 10px 28px rgba(0,0,0,0.75); pointer-events:none; display:none;';
                    document.body.appendChild(tt);
                        }
                        return tt;
                    }

                    window.__updateBraveLiteAiTooltipObj = function(obj) {
                        if (!obj || obj.url !== currentHoverUrl) return;
                        const tt = ensureTooltip();
                        tt.textContent = obj.summary;
                        tt.style.display = 'block';
                    };

                    document.addEventListener('mouseover', function(e) {
                        const a = e.target.closest('a[href]');
                        if (!a || !a.href || !a.href.startsWith('http')) return;
                        if (activeAnchor === a) return;
                        activeAnchor = a;
                        currentHoverUrl = a.href;

                        const linkText = (a.innerText || a.textContent || a.getAttribute('aria-label') || a.title || '').trim().slice(0, 140);
                        const rect = a.getBoundingClientRect();
                        const x = Math.min(window.innerWidth - 395, Math.max(12, rect.left));
                        const y = (rect.bottom + 110 < window.innerHeight) ? (rect.bottom + 8) : Math.max(12, rect.top - 85);

                        clearTimeout(hoverTimer);
                        hoverTimer = setTimeout(function() {
                            if (activeAnchor !== a) return;
                            const tt = ensureTooltip();
                            tt.style.left = x + 'px';
                        tt.style.top = y + 'px';
                        tt.textContent = '✨ BraveLite AI is summarizing link...';
                        tt.style.display = 'block';
                        console.log('BRAVELITE_HOVER_LINK::' + JSON.stringify({ url: a.href, text: linkText }));
                        }, 450);
                    }, true);

                    document.addEventListener('mouseout', function(e) {
                        if (!activeAnchor) return;
                        if (e.relatedTarget && activeAnchor.contains(e.relatedTarget)) return;
                        clearTimeout(hoverTimer);
                        activeAnchor = null;
                        currentHoverUrl = '';
                    const tt = document.getElementById('__bravelite_ai_tooltip');
                    if (tt) tt.style.display = 'none';
                    }, true);
                        }

                        // Debloated SponsorBlock: Hooks native video timeupdate event instead of a 500ms background timer
                        if (window.location.hostname.includes('youtube.com') && !window.__sbActive) {
                            window.__sbActive = true;
                            document.addEventListener('timeupdate', function(e) {
                                const v = e.target;
                                if (!v || v.tagName !== 'VIDEO') return;
                                const urlParams = new URLSearchParams(window.location.search);
                                const videoId = urlParams.get('v');
                                if (videoId && v.__sbFetched !== videoId) {
                                    v.__sbFetched = videoId;
                                    fetch('https://sponsor.ajay.app/api/skipSegments?videoID=' + videoId + '&categories=["sponsor","selfpromo"]')
                                    .then(r => r.json())
                                    .then(segments => { v.__sbSegments = segments; })
                                    .catch(() => {});
                                }
                                if (v.__sbSegments && Array.isArray(v.__sbSegments)) {
                                    for (let s of v.__sbSegments) {
                                        if (v.currentTime >= s.segment[0] && v.currentTime < s.segment[1]) {
                                            v.currentTime = s.segment[1];
                                        }
                                    }
                                }
                            }, true);
                        }
                    })();
                )");

                setupPasswordClickPromptForView(view);
            }
        });
    }
};

QList<BrowserWindow*> BrowserWindow::s_openWindows;

// ---------------- 8. POPUP BLOCKER & NEW TABS ----------------
QWebEngineView* CustomWebView::createWindow(QWebEnginePage::WebWindowType type) {
    BrowserWindow *win = qobject_cast<BrowserWindow*>(window());
    if (!win) return nullptr;

    if (win->adBlocker() && win->adBlocker()->isEnabled() && type == QWebEnginePage::WebBrowserBackgroundTab) {
        return nullptr;
    }
    return win->createNewTabWithView();
}

CustomWebView* BrowserWindow::createNewTabWithView(const QUrl &url) {
    CustomWebView *view = new CustomWebView(this);
    CustomWebEnginePage *p = new CustomWebEnginePage(m_profile, m_adBlocker, view);
    view->setPage(p);

    int index = tabStack->addWidget(view);
    int tabIdx = tabBar->addTab("New Tab");
    tabBar->setCurrentIndex(tabIdx);
    tabStack->setCurrentIndex(index);

    bindWebViewSignals(view);

    if (url.isEmpty()) view->setHtml(getNewTabHtml());
    else view->load(url);

    return view;
}

int main(int argc, char *argv[]) {
    // Low-CPU & smooth rendering flags
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--enable-smooth-scrolling --disable-background-networking --enable-LowPriorityIframes "
            "--disable-renderer-backgrounding=false --autoplay-policy=no-user-gesture-required");

    CrashLogger::installCrashHandlers();

    QApplication app(argc, argv);
    app.setApplicationName("BraveLite");
    app.setApplicationVersion("1.11");
    app.setOrganizationName("BraveLite");

    if (QStyle *fusion = QStyleFactory::create("Fusion")) {
        app.setStyle(fusion);
    }

    QIcon appIcon = QIcon::fromTheme("bravelite");
    if (appIcon.isNull()) appIcon = QIcon(QDir::homePath() + "/.local/share/icons/hicolor/512x512/apps/bravelite.png");
    if (appIcon.isNull()) appIcon = QIcon("assets/bravelite.ico");
    if (appIcon.isNull()) appIcon = QIcon("assets/icon.ico");
    app.setWindowIcon(appIcon);

    QList<QUrl> startupUrls;
    const QStringList args = app.arguments();
    for (int i = 1; i < args.size(); ++i) {
        QString arg = args[i].trimmed();
        if (arg.isEmpty() || arg.startsWith("-")) continue;

        if (arg.startsWith("http://", Qt::CaseInsensitive) ||
            arg.startsWith("https://", Qt::CaseInsensitive) ||
            arg.startsWith("file://", Qt::CaseInsensitive)) {
            startupUrls.append(QUrl(arg));
            } else if (QFileInfo::exists(arg)) {
                startupUrls.append(QUrl::fromLocalFile(QFileInfo(arg).absoluteFilePath()));
            } else if (arg.contains(".") && !arg.contains(" ")) {
                startupUrls.append(QUrl("https://" + arg));
            }
    }

    const QString ipcServerName = "BraveLiteSingleInstanceServer_v111";
    {
        QLocalSocket socket;
        socket.connectToServer(ipcServerName);
        if (socket.waitForConnected(250)) {
            QJsonArray urlArr;
            for (const QUrl &u : startupUrls) urlArr.append(u.toString());
            if (urlArr.isEmpty()) urlArr.append("NEW_TAB");
            socket.write(QJsonDocument(urlArr).toJson(QJsonDocument::Compact));
            socket.flush();
            socket.waitForBytesWritten(250);
            return 0;
        }
    }

    QLocalServer ipcServer;
    QLocalServer::removeServer(ipcServerName);
    ipcServer.listen(ipcServerName);

    BrowserWindow *window = new BrowserWindow(false, startupUrls.isEmpty(), startupUrls);
    window->show();

    QObject::connect(&ipcServer, &QLocalServer::newConnection, [&ipcServer]() {
        while (ipcServer.hasPendingConnections()) {
            QLocalSocket *client = ipcServer.nextPendingConnection();
            QObject::connect(client, &QLocalSocket::readyRead, [client]() {
                QByteArray data = client->readAll();
                QJsonArray arr = QJsonDocument::fromJson(data).array();
                BrowserWindow *targetWin = BrowserWindow::s_openWindows.isEmpty() ? nullptr : BrowserWindow::s_openWindows.first();
                if (targetWin) {
                    for (const QJsonValue &val : arr) {
                        QString str = val.toString();
                        if (str == "NEW_TAB") targetWin->openExternalUrlInExistingWindow(QUrl());
                        else targetWin->openExternalUrlInExistingWindow(QUrl(str));
                    }
                }
                client->disconnectFromServer();
                client->deleteLater();
            });
        }
    });

    return app.exec();
}

#include "main.moc"
