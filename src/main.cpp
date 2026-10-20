// ============================================================================
// BraveLite v1.5 - Part 1 of 4
// ============================================================================
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
#include <QSplitter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineFullScreenRequest>
#include <QWebEngineHistory>
#include <QWebEngineContextMenuRequest>
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
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QFont>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <functional>
#include <memory>

#if defined(__linux__) || defined(__unix__)
#include <execinfo.h>
#include <unistd.h>
#endif

// ---------------- CRISP CROSS-PLATFORM VECTOR ICON RENDERER (Fixes Windows & Linux Icons) ----------------
namespace BrowserIcons {
    inline QIcon drawIcon(const QString &type, const QColor &color = QColor("#e4e6eb")) {
        QPixmap px(32, 32);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing, true);
        QPen pen(color, 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);

        if (type == "back") {
            p.drawLine(25, 16, 7, 16);
            p.drawLine(14, 9, 7, 16);
            p.drawLine(14, 23, 7, 16);
        } else if (type == "forward") {
            p.drawLine(7, 16, 25, 16);
            p.drawLine(18, 9, 25, 16);
            p.drawLine(18, 23, 25, 16);
        } else if (type == "reload") {
            p.drawArc(6, 6, 20, 20, 45 * 16, 285 * 16);
            p.drawLine(23, 5, 25, 11);
            p.drawLine(19, 11, 25, 11);
        } else if (type == "split") {
            p.drawRoundedRect(4, 6, 24, 20, 3, 3);
            p.drawLine(16, 6, 16, 26);
            p.fillRect(17, 8, 10, 16, color);
        } else if (type == "star") {
            QPainterPath star;
            star.moveTo(16, 4);
            star.lineTo(19.5, 11.5);
            star.lineTo(28, 12.5);
            star.lineTo(21.5, 18.2);
            star.lineTo(23.3, 26.5);
            star.lineTo(16, 22.3);
            star.lineTo(8.7, 26.5);
            star.lineTo(10.5, 18.2);
            star.lineTo(4, 12.5);
            star.lineTo(12.5, 11.5);
            star.closeSubpath();
            p.drawPath(star);
        } else if (type == "key") {
            p.drawEllipse(6, 10, 10, 10);
            p.drawLine(16, 15, 27, 15);
            p.drawLine(23, 15, 23, 20);
            p.drawLine(27, 15, 27, 20);
        } else if (type == "shield") {
            QPainterPath sh;
            sh.moveTo(16, 4);
            sh.lineTo(26, 8);
            sh.lineTo(26, 16);
            sh.quadTo(26, 24, 16, 28);
            sh.quadTo(6, 24, 6, 16);
            sh.lineTo(6, 8);
            sh.closeSubpath();
            p.drawPath(sh);
        } else if (type == "download") {
            p.drawLine(16, 6, 16, 21);
            p.drawLine(10, 15, 16, 21);
            p.drawLine(22, 15, 16, 21);
            p.drawLine(7, 26, 25, 26);
        } else if (type == "menu") {
            p.drawLine(6, 9, 26, 9);
            p.drawLine(6, 16, 26, 16);
            p.drawLine(6, 23, 26, 23);
        } else if (type == "zoom_in") {
            p.drawLine(8, 16, 24, 16);
            p.drawLine(16, 8, 16, 24);
        } else if (type == "zoom_out") {
            p.drawLine(8, 16, 24, 16);
        } else if (type == "translate") {
            p.drawEllipse(6, 6, 20, 20);
            p.drawLine(6, 16, 26, 16);
            p.drawArc(11, 6, 10, 20, 0, 360 * 16);
        }
        p.end();
        return QIcon(px);
    }
}

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
            out << "BraveLite v1.5 Extensive Crash & Diagnostic Report\n";
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

// ---------------- 2. CUSTOM WEB ENGINE PAGE (Seamless Back/Forward/Reload for BraveLite Engine) ----------------
class CustomWebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit CustomWebEnginePage(QWebEngineProfile *profile, AdBlocker *blocker, QObject *parent = nullptr)
    : QWebEnginePage(profile, parent), m_blocker(blocker) {}

    bool isRenderingSearchHtml = false;

signals:
    void loginCaptured(const QString &site, const QString &user, const QString &pass);
    void autofillRequested(const QString &site);
    void linkHoverSummarizeRequested(const QString &url, const QString &linkText);
    void braveliteSearchRequested(const QString &query, const QString &tabType);
    void aiChatRequested(const QString &chatPrompt);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override {
        if (isRenderingSearchHtml && url.host() == "search.bravelite.local") {
            return true;
        }

        if ((url.scheme() == "bravelite" && url.host() == "search") ||
            (url.host() == "search.bravelite.local")) {
            QUrlQuery q(url);
        QString queryVal = q.queryItemValue("q", QUrl::FullyDecoded);
        QString tab = q.queryItemValue("tab", QUrl::FullyDecoded);
        if (tab.isEmpty()) tab = "all";
        if (!queryVal.isEmpty()) {
            QTimer::singleShot(0, this, [this, queryVal, tab]() {
                emit braveliteSearchRequested(queryVal, tab);
            });
            return false;
        }
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
        } else if (message.startsWith("BRAVELITE_SEARCH_TAB::")) {
            QString jsonPart = message.mid(QString("BRAVELITE_SEARCH_TAB::").length());
            QJsonObject obj = QJsonDocument::fromJson(jsonPart.toUtf8()).object();
            QString q = obj["q"].toString().trimmed();
            QString tab = obj["tab"].toString("all");
            if (!q.isEmpty()) {
                emit braveliteSearchRequested(q, tab);
            }
        } else if (message.startsWith("BRAVELITE_SEARCH::")) {
            QString q = message.mid(QString("BRAVELITE_SEARCH::").length()).trimmed();
            if (!q.isEmpty()) {
                emit braveliteSearchRequested(q, "all");
            }
        } else if (message.startsWith("BRAVELITE_AI_CHAT::")) {
            QString prompt = message.mid(QString("BRAVELITE_AI_CHAT::").length()).trimmed();
            if (!prompt.isEmpty()) {
                emit aiChatRequested(prompt);
            }
        }
    }

private:
    AdBlocker *m_blocker;
};

class BrowserWindow;

// ---------------- 3. CUSTOM WEB VIEW (Overhauled Link Right-Click Menu + Working Inspect Element + Search Persistence) ----------------
class CustomWebView : public QWebEngineView {
    Q_OBJECT
public:
    explicit CustomWebView(QWidget *parent = nullptr) : QWebEngineView(parent) {}

    QString lastSearchQuery;
    QString lastSearchTab = "all";
    QString pendingWebResultsJson;
    QString pendingImageResultsJson;
    QString pendingAiOverviewJsArg;
    quint64 activeSearchId = 0;

signals:
    void openLinkInNewTabRequested(const QUrl &url);
    void openLinkInNewWindowRequested(const QUrl &url);
    void openLinkInIncognitoRequested(const QUrl &url);
    void openLinkInSplitViewRequested(const QUrl &url);
    void inspectElementRequested(CustomWebView *targetView);
    void searchBackFallbackRequested(const QString &query, const QString &tab);

protected:
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

    static QUrl stripTrackingParams(const QUrl &rawUrl) {
        if (!rawUrl.hasQuery()) return rawUrl;
        QUrlQuery q(rawUrl);
        QUrlQuery cleanQ;
        const QStringList blockedKeys = {
            "utm_source", "utm_medium", "utm_campaign", "utm_term", "utm_content",
            "utm_id", "fbclid", "gclid", "dclid", "msclkid", "mc_cid", "mc_eid",
            "yclid", "_hsenc", "_hsmi", "igshid", "si", "ref_src"
        };
        const auto items = q.queryItems(QUrl::FullyDecoded);
        for (const auto &pair : items) {
            if (!blockedKeys.contains(pair.first.toLower())) {
                cleanQ.addQueryItem(pair.first, pair.second);
            }
        }
        QUrl cleaned = rawUrl;
        cleaned.setQuery(cleanQ);
        return cleaned;
    }

    void contextMenuEvent(QContextMenuEvent *event) override {
        QPoint clickPos = event->pos();
        QWebEngineContextMenuRequest *ctxReq = lastContextMenuRequest();
        QUrl linkUrl = ctxReq ? ctxReq->linkUrl() : QUrl();

        QMenu *menu = new QMenu(this);
        menu->setStyleSheet(
            "QMenu { background: #202124; color: #e8eaed; border: 1px solid #3c4043; border-radius: 6px; padding: 5px; font-size: 13px; } "
            "QMenu::item { padding: 6px 22px; border-radius: 4px; } "
            "QMenu::item:selected { background: #ff5500; color: #ffffff; } "
            "QMenu::separator { height: 1px; background: #3c4043; margin: 4px 0; }"
        );

        if (linkUrl.isValid() && !linkUrl.isEmpty()) {
            QAction *actNewTab = menu->addAction("Open link in new tab");
            connect(actNewTab, &QAction::triggered, this, [this, linkUrl]() {
                emit openLinkInNewTabRequested(linkUrl);
            });

            QAction *actNewWin = menu->addAction("Open link in new window");
            connect(actNewWin, &QAction::triggered, this, [this, linkUrl]() {
                emit openLinkInNewWindowRequested(linkUrl);
            });

            QAction *actIncog = menu->addAction("Open link in incognito window");
            connect(actIncog, &QAction::triggered, this, [this, linkUrl]() {
                emit openLinkInIncognitoRequested(linkUrl);
            });

            QAction *actSplit = menu->addAction("Open link in split view");
            connect(actSplit, &QAction::triggered, this, [this, linkUrl]() {
                emit openLinkInSplitViewRequested(linkUrl);
            });

            menu->addSeparator();

            QAction *actSaveLink = menu->addAction("Save link as");
            connect(actSaveLink, &QAction::triggered, this, [this]() {
                page()->triggerAction(QWebEnginePage::DownloadLinkToDisk);
            });

            QAction *actCopyLink = menu->addAction("Copy link address");
            connect(actCopyLink, &QAction::triggered, this, [linkUrl]() {
                QApplication::clipboard()->setText(linkUrl.toString());
            });

            QAction *actCopyClean = menu->addAction("Copy clean link");
            connect(actCopyClean, &QAction::triggered, this, [linkUrl]() {
                QApplication::clipboard()->setText(stripTrackingParams(linkUrl).toString());
            });

            menu->addSeparator();
        } else {
            QAction *actBack = menu->addAction("Back");
            actBack->setEnabled(history()->canGoBack() || !lastSearchQuery.isEmpty());
            connect(actBack, &QAction::triggered, this, [this]() {
                if (!lastSearchQuery.isEmpty() && url().host() != "search.bravelite.local" && history()->currentItemIndex() <= 1) {
                    emit searchBackFallbackRequested(lastSearchQuery, lastSearchTab);
                } else if (history()->canGoBack()) {
                    back();
                } else if (!lastSearchQuery.isEmpty()) {
                    emit searchBackFallbackRequested(lastSearchQuery, lastSearchTab);
                }
            });

            QAction *actForward = menu->addAction("Forward");
            actForward->setEnabled(history()->canGoForward());
            connect(actForward, &QAction::triggered, this, &QWebEngineView::forward);

            QAction *actReload = menu->addAction("Reload");
            connect(actReload, &QAction::triggered, this, [this]() {
                if (url().host() == "search.bravelite.local" && !lastSearchQuery.isEmpty()) {
                    emit searchBackFallbackRequested(lastSearchQuery, lastSearchTab);
                } else {
                    reload();
                }
            });
            menu->addSeparator();

            QAction *actSave = menu->addAction("Save page as...");
            connect(actSave, &QAction::triggered, this, [this]() {
                page()->triggerAction(QWebEnginePage::SavePage);
            });

            QAction *actSource = menu->addAction("View page source");
            connect(actSource, &QAction::triggered, this, [this]() {
                page()->triggerAction(QWebEnginePage::ViewSource);
            });
            menu->addSeparator();
        }

        QAction *actBlockEl = menu->addAction("Block elements");
        connect(actBlockEl, &QAction::triggered, this, [this, clickPos]() {
            QString js = QString(
                "(function() {"
                "  const el = document.elementFromPoint(%1, %2);"
                "  if (el && el !== document.body && el !== document.documentElement) {"
                "    el.style.setProperty('display', 'none', 'important');"
                "  }"
                "})();"
            ).arg(clickPos.x()).arg(clickPos.y());
            page()->runJavaScript(js);
        });

        QAction *actInspect = menu->addAction("Inspect elements");
        connect(actInspect, &QAction::triggered, this, [this]() {
            emit inspectElementRequested(this);
        });

        menu->exec(event->globalPos());
        delete menu;
    }
};

// ============================================================================
// BraveLite v1.5 - Part 2 of 4
// ============================================================================

// ---------------- 4. BOOKMARK TOOLBUTTON (Movable Reorder + Drag to Tab Bar & URL Box) ----------------
class BookmarkToolButton : public QToolButton {
    Q_OBJECT
public:
    explicit BookmarkToolButton(int index, const QString &url, QWidget *parent = nullptr)
    : QToolButton(parent), m_index(index), m_url(url) {}

    int bookmarkIndex() const { return m_index; }
    QString bookmarkUrl() const { return m_url; }

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
            int dist = (event->pos() - m_dragStartPos).manhattanLength();
            if (!m_dragging && dist > QApplication::startDragDistance()) {
                m_dragging = true;
            }
            if (m_dragging && parentWidget()) {
                QPoint globalPt = event->globalPosition().toPoint();
                QPoint posInBar = parentWidget()->mapFromGlobal(globalPt);

                // If dragged vertically outside the bookmarks bar (to the Tab Bar or URL Box), start a URL QDrag
                if (posInBar.y() < -4 || posInBar.y() > parentWidget()->height() + 4) {
                    m_dragging = false;
                    setDown(false);
                    QDrag *drag = new QDrag(this);
                    QMimeData *mime = new QMimeData();
                    mime->setUrls({QUrl(m_url)});
                    mime->setText(m_url);
                    drag->setMimeData(mime);
                    drag->setPixmap(icon().pixmap(16, 16));
                    drag->exec(Qt::CopyAction);
                    return;
                }

                // Otherwise reorder horizontally inside the bookmarks bar
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
    QString m_url;
    QPoint m_dragStartPos;
    bool m_dragging = false;
};

// ---------------- 5. DROPPABLE URL BOX (Replaces Existing URL with Dropped Link/Bookmark) ----------------
class DroppableUrlBar : public QLineEdit {
    Q_OBJECT
public:
    explicit DroppableUrlBar(QWidget *parent = nullptr) : QLineEdit(parent) {
        setAcceptDrops(true);
    }

signals:
    void urlDropped(const QUrl &url);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dragMoveEvent(QDragMoveEvent *event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        QUrl u;
        if (event->mimeData()->hasUrls() && !event->mimeData()->urls().isEmpty()) {
            u = event->mimeData()->urls().first();
        } else if (event->mimeData()->hasText()) {
            QString t = event->mimeData()->text().trimmed();
            if (!t.isEmpty()) {
                u = (t.startsWith("http://") || t.startsWith("https://") || t.startsWith("bravelite://"))
                ? QUrl(t) : QUrl("https://" + t);
            }
        }
        if (u.isValid() && !u.isEmpty()) {
            clear();
            setText(u.toString());
            emit urlDropped(u);
            event->acceptProposedAction();
            return;
        }
        QLineEdit::dropEvent(event);
    }
};

// ---------------- 6. ALWAYS-REACHABLE CLOSE BUTTON & SCROLL-ARROW-FREE TAB BAR (Fixes Photo 5) ----------------
class TabCloseButton : public QPushButton {
    Q_OBJECT
public:
    explicit TabCloseButton(QWidget *parent = nullptr) : QPushButton(parent) {
        setFixedSize(16, 16);
        setCursor(Qt::PointingHandCursor);
        setToolTip("Close Tab");
        setText("✕");
        updateState(true);
    }

    void updateState(bool visible) {
        setEnabled(true);
        if (visible) {
            setStyleSheet(
                "QPushButton { background-color: #e81123; border: none; border-radius: 8px; "
                "color: white; font-size: 9px; font-weight: bold; margin: 0; padding: 0; } "
                "QPushButton:hover { background-color: #ff5460; }"
            );
        } else {
            setStyleSheet(
                "QPushButton { background: rgba(255,255,255,0.08); border: none; border-radius: 8px; "
                "color: #9aa0a6; font-size: 9px; font-weight: bold; margin: 0; padding: 0; } "
                "QPushButton:hover { background-color: #e81123; color: white; }"
            );
        }
    }
};

class NewTabDropButton : public QPushButton {
    Q_OBJECT
public:
    explicit NewTabDropButton(const QString &text, QWidget *parent = nullptr)
    : QPushButton(text, parent) {
        setAcceptDrops(true);
    }

signals:
    void urlDropped(const QUrl &url);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }
    void dragMoveEvent(QDragMoveEvent *event) override {
        event->acceptProposedAction();
    }
    void dropEvent(QDropEvent *event) override {
        QUrl u;
        if (event->mimeData()->hasUrls() && !event->mimeData()->urls().isEmpty()) {
            u = event->mimeData()->urls().first();
        } else if (event->mimeData()->hasText()) {
            QString t = event->mimeData()->text().trimmed();
            if (t.startsWith("http://") || t.startsWith("https://")) u = QUrl(t);
        }
        if (u.isValid() && !u.isEmpty()) {
            emit urlDropped(u);
            event->acceptProposedAction();
        }
    }
};

class CustomTabBar : public QTabBar {
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr) : QTabBar(parent) {
        setMouseTracking(true);
        setExpanding(false);
        setUsesScrollButtons(false); // Fixes Photo 5: never spawn tiny scroll arrows over the close button!
        setElideMode(Qt::ElideRight);
        setAcceptDrops(true);
    }

signals:
    void tabDetachRequested(int index, const QPoint &globalPos);
    void urlDroppedOnTabBar(const QUrl &url);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dragMoveEvent(QDragMoveEvent *event) override {
        if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        QUrl u;
        if (event->mimeData()->hasUrls() && !event->mimeData()->urls().isEmpty()) {
            u = event->mimeData()->urls().first();
        } else if (event->mimeData()->hasText()) {
            QString t = event->mimeData()->text().trimmed();
            if (t.startsWith("http://") || t.startsWith("https://")) u = QUrl(t);
        }
        if (u.isValid() && !u.isEmpty()) {
            emit urlDroppedOnTabBar(u);
            event->acceptProposedAction();
            return;
        }
        QTabBar::dropEvent(event);
    }

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
                btn->updateState(i == hovered || i == currentIndex());
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
                btn->updateState(i == currentIndex());
            }
        }
    }

private:
    int m_dragIndex = -1;
    QPoint m_dragStartPos;
    bool m_detachedDuringDrag = false;
};

// ---------------- 7. DATA STRUCTURES & DRAGGABLE DOWNLOAD FILE CARD ----------------
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

class DraggableDownloadCard : public QWidget {
    Q_OBJECT
public:
    explicit DraggableDownloadCard(DownloadItem *item, QWidget *parent = nullptr)
    : QWidget(parent), m_item(item) {}

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_startPos = event->pos();
        }
        QWidget::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if ((event->buttons() & Qt::LeftButton) && m_item && QFileInfo::exists(m_item->path)) {
            if ((event->pos() - m_startPos).manhattanLength() > QApplication::startDragDistance()) {
                QDrag *drag = new QDrag(this);
                QMimeData *mime = new QMimeData();
                QUrl fileUrl = QUrl::fromLocalFile(QFileInfo(m_item->path).absoluteFilePath());
                mime->setUrls({fileUrl});
                mime->setText(fileUrl.toString());
                drag->setMimeData(mime);
                QPixmap dragPx(180, 32);
                dragPx.fill(QColor("#242529"));
                QPainter p(&dragPx);
                p.setPen(QColor("#ff5500"));
                p.drawRect(0, 0, 179, 31);
                p.setPen(Qt::white);
                p.drawText(QRect(8, 0, 164, 32), Qt::AlignVCenter | Qt::AlignLeft, "📄 " + m_item->fileName);
                p.end();
                drag->setPixmap(dragPx);
                drag->exec(Qt::CopyAction);
                return;
            }
        }
        QWidget::mouseMoveEvent(event);
    }

private:
    DownloadItem *m_item;
    QPoint m_startPos;
};

struct SitePermission {
    QString origin;
    QString permission;
    bool granted;
};

// ---------------- 8. MAIN BROWSER WINDOW ----------------
class BrowserWindow : public QMainWindow {
    Q_OBJECT
public:
    static QList<BrowserWindow*> s_openWindows;

    BrowserWindow(bool isIncognito = false, bool openDefaultTab = true, const QList<QUrl> &startupUrls = QList<QUrl>(), QWidget *parent = nullptr)
    : QMainWindow(parent), m_isIncognito(isIncognito), m_profile(nullptr) {

        s_openWindows.append(this);
        setAttribute(Qt::WA_DeleteOnClose, true);

        resize(1280, 840);
        setWindowTitle(m_isIncognito ? "BraveLite v1.5 [Incognito]" : "BraveLite v1.5");

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
            if (aiEnabled && !gamingModeActive) {
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
                if (auto *v = CustomWebViewFromWidget(tabStack->widget(i))) {
                    urls.append(v->url().toString());
                } else {
                    urls.append("[Internal Tab: " + tabBar->tabText(i) + "]");
                }
            }
            return urls;
        };
        CrashLogger::s_activeStateProvider = [this]() {
            return QString("Theme=%1 | Shield=%2 | AI=%3 (Model=%4, GPU=%5) | GamingMode=%6 | SearchEngine=%7 | Incognito=%8")
            .arg(currentThemeProfile,
                 m_adBlocker->isEnabled() ? "ON" : "OFF",
                 aiEnabled ? "ON" : "OFF",
                 ollamaModel,
                 aiUseGpu ? "Enabled" : "CPU-Only",
                 gamingModeActive ? "ACTIVE" : "OFF",
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

    static CustomWebView* CustomWebViewFromWidget(QWidget *w) {
        if (!w) return nullptr;
        if (auto *v = qobject_cast<CustomWebView*>(w)) return v;
        if (auto *sp = qobject_cast<QSplitter*>(w)) {
            for (int i = 0; i < sp->count(); ++i) {
                if (auto *v = qobject_cast<CustomWebView*>(sp->widget(i))) return v;
            }
        }
        return nullptr;
    }

    CustomWebView* activeWebView() const {
        return CustomWebViewFromWidget(tabStack ? tabStack->currentWidget() : nullptr);
    }

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
    void addNewTab(const QUrl &url = QUrl()) {
        createNewTabWithView(url);
    }

    void closeTab(int idx) {
        if (tabBar->count() > 1) {
            QWidget *w = tabStack->widget(idx);
            tabStack->removeWidget(w);
            tabBar->removeTab(idx);
            if (w) w->deleteLater();
        } else {
            close();
        }
        updateSplitButtonState();
    }

    void openNewWindowWithUrl(const QUrl &url) {
        BrowserWindow *w = new BrowserWindow(false, false, {url});
        w->show();
    }

    void openIncognitoWithUrl(const QUrl &url) {
        BrowserWindow *w = new BrowserWindow(true, false, {url});
        w->show();
    }

    void openInspectElementWindow(CustomWebView *targetView) {
        if (!targetView || !targetView->page()) return;

        QDialog *devDlg = new QDialog(this, Qt::Window);
        devDlg->setAttribute(Qt::WA_DeleteOnClose, true);
        devDlg->setWindowTitle("BraveLite Developer Tools — " + (targetView->title().isEmpty() ? targetView->url().toString() : targetView->title()));
        devDlg->resize(1020, 680);

        QVBoxLayout *lay = new QVBoxLayout(devDlg);
        lay->setContentsMargins(0, 0, 0, 0);

        QWebEngineView *devView = new QWebEngineView(devDlg);
        QWebEnginePage *devPage = new QWebEnginePage(m_profile, devView);
        devView->setPage(devPage);
        lay->addWidget(devView);

        targetView->page()->setDevToolsPage(devPage);
        targetView->page()->triggerAction(QWebEnginePage::InspectElement);

        devDlg->show();
        devDlg->raise();
        devDlg->activateWindow();
    }

    // ---------------- SPLIT VIEW CONTROLS & HIGH-SPEED WINDOWS OPTIMIZATION ----------------
    void unsplitCurrentTab() {
        QWidget *current = tabStack->currentWidget();
        auto *splitter = qobject_cast<QSplitter*>(current);
        if (!splitter) return;

        int currentIdx = tabStack->indexOf(splitter);
        CustomWebView *left = qobject_cast<CustomWebView*>(splitter->widget(0));
        CustomWebView *right = splitter->count() > 1 ? qobject_cast<CustomWebView*>(splitter->widget(1)) : nullptr;

        if (left) left->setParent(this);
        if (right) right->setParent(this);

        tabStack->removeWidget(splitter);
        splitter->deleteLater();

        if (left) {
            tabStack->insertWidget(currentIdx, left);
            tabStack->setCurrentIndex(currentIdx);
            left->show();
        }
        if (right) {
            int newIdx = tabStack->addWidget(right);
            int tIdx = tabBar->addTab(right->icon(), right->title().isEmpty() ? "Tab" : right->title().left(18));
            Q_UNUSED(newIdx);
            Q_UNUSED(tIdx);
            right->show();
        }
        updateSplitButtonState();
    }

    void closeLeftSplitView() {
        QWidget *current = tabStack->currentWidget();
        auto *splitter = qobject_cast<QSplitter*>(current);
        if (!splitter || splitter->count() < 2) return;

        int currentIdx = tabStack->indexOf(splitter);
        CustomWebView *left = qobject_cast<CustomWebView*>(splitter->widget(0));
        CustomWebView *right = qobject_cast<CustomWebView*>(splitter->widget(1));

        if (right) right->setParent(this);
        tabStack->removeWidget(splitter);
        splitter->deleteLater();
        if (left) left->deleteLater();

        if (right) {
            tabStack->insertWidget(currentIdx, right);
            tabStack->setCurrentIndex(currentIdx);
            right->show();
            tabBar->setTabText(currentIdx, right->title().isEmpty() ? "Tab" : right->title().left(18));
        }
        updateSplitButtonState();
    }

    void closeRightSplitView() {
        QWidget *current = tabStack->currentWidget();
        auto *splitter = qobject_cast<QSplitter*>(current);
        if (!splitter) return;

        int currentIdx = tabStack->indexOf(splitter);
        CustomWebView *left = qobject_cast<CustomWebView*>(splitter->widget(0));
        CustomWebView *right = splitter->count() > 1 ? qobject_cast<CustomWebView*>(splitter->widget(1)) : nullptr;

        if (left) left->setParent(this);
        tabStack->removeWidget(splitter);
        splitter->deleteLater();
        if (right) right->deleteLater();

        if (left) {
            tabStack->insertWidget(currentIdx, left);
            tabStack->setCurrentIndex(currentIdx);
            left->show();
        }
        updateSplitButtonState();
    }

    void swapSplitTabPositions() {
        QWidget *current = tabStack->currentWidget();
        auto *splitter = qobject_cast<QSplitter*>(current);
        if (!splitter || splitter->count() < 2) return;

        QList<int> curSizes = splitter->sizes();
        QWidget *w1 = splitter->widget(1);
        splitter->insertWidget(0, w1);
        splitter->setSizes(curSizes);
    }

    void openInSplitView(const QUrl &url) {
        QWidget *current = tabStack->currentWidget();
        if (!current) return;
        int currentIdx = tabStack->indexOf(current);

        if (auto *existingSplitter = qobject_cast<QSplitter*>(current)) {
            if (existingSplitter->count() > 1) {
                if (auto *rightV = qobject_cast<CustomWebView*>(existingSplitter->widget(1))) {
                    rightV->load(url);
                    updateSplitButtonState();
                    return;
                }
            }
        }

        CustomWebView *leftView = qobject_cast<CustomWebView*>(current);
        if (!leftView) {
            addNewTab(url);
            return;
        }

        CustomWebView *rightView = new CustomWebView(this);
        CustomWebEnginePage *p = new CustomWebEnginePage(m_profile, m_adBlocker, rightView);
        rightView->setPage(p);
        bindWebViewSignals(rightView);

        QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
        splitter->setChildrenCollapsible(false);
        splitter->setHandleWidth(5);
        #ifdef Q_OS_WIN
        // Eliminates DWM composition resize lag on Windows so Split View resizes at 60+ FPS
        splitter->setOpaqueResize(false);
        #else
        splitter->setOpaqueResize(true);
        #endif
        splitter->setStyleSheet(
            "QSplitter::handle { background-color: #2f3238; border-left: 1px solid #444851; border-right: 1px solid #444851; } "
            "QSplitter::handle:hover { background-color: #ff5500; }"
        );

        tabStack->removeWidget(leftView);
        splitter->addWidget(leftView);
        splitter->addWidget(rightView);
        leftView->show();
        rightView->show();
        splitter->setSizes({width() / 2, width() / 2});

        tabStack->insertWidget(currentIdx, splitter);
        tabStack->setCurrentIndex(currentIdx);
        rightView->load(url);
        updateSplitButtonState();
    }

    // ============================================================================
    // BraveLite v1.5 - Part 3 of 4
    // ============================================================================

    // Split View Top-Left Button Menu (Matches Photo 2: Unsplit, Close Left, Close Right, Show Stacked, Swap)
    void showSplitViewMenu() {
        QMenu menu(this);
        menu.setStyleSheet(
            "QMenu { background: #202124; color: #e8eaed; border: 1px solid #3c4043; border-radius: 8px; padding: 6px; font-size: 13px; } "
            "QMenu::item { padding: 8px 24px 8px 16px; border-radius: 4px; } "
            "QMenu::item:selected { background: #ff5500; color: #ffffff; } "
            "QMenu::separator { height: 1px; background: #3c4043; margin: 4px 0; }"
        );

        QAction *actUnsplit = menu.addAction("⧉   Unsplit tabs");
        connect(actUnsplit, &QAction::triggered, this, &BrowserWindow::unsplitCurrentTab);
        menu.addSeparator();

        QAction *actCloseLeft = menu.addAction("◧   Close left view");
        connect(actCloseLeft, &QAction::triggered, this, &BrowserWindow::closeLeftSplitView);

        QAction *actCloseRight = menu.addAction("◨   Close right view");
        connect(actCloseRight, &QAction::triggered, this, &BrowserWindow::closeRightSplitView);
        menu.addSeparator();

        QAction *actStacked = menu.addAction("⊟   Show stacked");
        connect(actStacked, &QAction::triggered, this, [this]() {
            if (auto *sp = qobject_cast<QSplitter*>(tabStack->currentWidget())) {
                sp->setOrientation(sp->orientation() == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);
            }
        });

        QAction *actSwap = menu.addAction("⇄   Swap tab positions");
        connect(actSwap, &QAction::triggered, this, &BrowserWindow::swapSplitTabPositions);

        menu.exec(btnSplitView->mapToGlobal(QPoint(0, btnSplitView->height() + 4)));
    }

    void updateSplitButtonState() {
        if (!btnSplitView || !tabStack) return;
        QWidget *cur = tabStack->currentWidget();
        bool isSplit = (qobject_cast<QSplitter*>(cur) != nullptr);
        btnSplitView->setVisible(isSplit);
    }

    // Gaming Mode Switch: stops all AI, unloads models from GPU VRAM/RAM, disables hover previews
    void toggleGamingMode() {
        gamingModeActive = !gamingModeActive;
        savePersistentData();

        if (gamingModeActive) {
            if (m_activeHoverReply) {
                m_activeHoverReply->disconnect();
                m_activeHoverReply->abort();
                m_activeHoverReply = nullptr;
            }
            QString endpoint = ollamaEndpoint.trimmed();
            if (endpoint.endsWith("/")) endpoint.chop(1);
            for (const QString &mName : {ollamaModel, translationModel}) {
                QNetworkRequest req(QUrl(endpoint + "/api/generate"));
                req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                QJsonObject body;
                body["model"] = mName.trimmed().toLower();
                body["keep_alive"] = 0;
                QNetworkReply *r = m_netManager->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
                connect(r, &QNetworkReply::finished, r, &QObject::deleteLater);
            }
            setWindowTitle(m_isIncognito ? "BraveLite v1.5 [Incognito | 🎮 Gaming Mode]" : "BraveLite v1.5 [🎮 Gaming Mode]");
            QMessageBox::information(this, "Gaming Mode Enabled",
                                     "🎮 Gaming Mode is now ON!\n\n"
                                     "• BraveLite Engine AI Overview & Hover Link Previews paused.\n"
                                     "• Local Ollama models unloaded from GPU VRAM & RAM.\n"
                                     "• Background timers throttled for maximum gaming FPS.");
        } else {
            if (aiEnabled) ensureOllamaServerRunning();
            setWindowTitle(m_isIncognito ? "BraveLite v1.5 [Incognito]" : "BraveLite v1.5");
            QMessageBox::information(this, "Gaming Mode Disabled",
                                     "🎮 Gaming Mode is now OFF.\n\n"
                                     "• AI Overview, Link Previews, and Full-Site Translation are re-enabled!");
        }
    }

    // Deep Full-Site AI Translation (Translates ALL page contents, headings, paragraphs, articles, and buttons in batches)
    void triggerSiteTranslation() {
        if (gamingModeActive) {
            QMessageBox::information(this, "Gaming Mode Active",
                                     "Gaming Mode is currently ON. Turn off Gaming Mode in the ☰ menu to use AI Site Translation.");
            return;
        }
        CustomWebView *view = activeWebView();
        if (!view || !view->page()) return;

        ensureOllamaServerRunning();
        QString targetLang = langCombo->currentText();
        btnTranslate->setText("⏳ Translating...");
        btnTranslate->setEnabled(false);

        QPointer<CustomWebView> safeView(view);
        QString extractJs = R"(
            (function() {
                const walker = document.createTreeWalker(document.body, NodeFilter.SHOW_TEXT, {
                    acceptNode: function(node) {
                        const p = node.parentElement;
                        if (!p) return NodeFilter.FILTER_REJECT;
                        const tag = p.tagName;
                        if (tag === 'SCRIPT' || tag === 'STYLE' || tag === 'NOSCRIPT' || tag === 'CODE' || tag === 'PRE' || tag === 'SVG') {
                            return NodeFilter.FILTER_REJECT;
                        }
                        if (p.id === '__bl_trans_badge') return NodeFilter.FILTER_REJECT;
                        const t = node.nodeValue.trim();
                        return (t.length >= 2 && /[a-zA-Z\u00C0-\u024F\u0370-\u03FF\u0400-\u04FF]/.test(t))
                            ? NodeFilter.FILTER_ACCEPT : NodeFilter.FILTER_REJECT;
                    }
                }, false);

                window.__blTransNodes = [];
                let texts = [];
                let node;
                while ((node = walker.nextNode()) && window.__blTransNodes.length < 240) {
                    window.__blTransNodes.push(node);
                    texts.push(node.nodeValue.trim().replace(/\s+/g, ' ').slice(0, 320));
                }
                return JSON.stringify(texts);
            })();
        )";

        view->page()->runJavaScript(extractJs, [this, safeView, targetLang](const QVariant &res) {
            QJsonArray srcArr = QJsonDocument::fromJson(res.toString().toUtf8()).array();
            if (srcArr.isEmpty()) {
                btnTranslate->setText("🌐 Translate");
                btnTranslate->setEnabled(true);
                return;
            }

            auto translateBatchPtr = std::make_shared<std::function<void(int)>>();
            *translateBatchPtr = [this, safeView, srcArr, targetLang, translateBatchPtr](int startIndex) {
                if (!safeView || !safeView->page() || startIndex >= srcArr.size()) {
                    btnTranslate->setText("🌐 Translate");
                    btnTranslate->setEnabled(true);
                    return;
                }

                int endIndex = qMin(startIndex + 40, (int)srcArr.size());
                QStringList batchLines;
                for (int i = startIndex; i < endIndex; ++i) {
                    batchLines.append(QString("[%1] %2").arg(i).arg(srcArr[i].toString()));
                }

                btnTranslate->setText(QString("⏳ %1/%2").arg(endIndex).arg(srcArr.size()));

                QString prompt = QString(
                    "Translate every numbered line below into %1. Keep the exact [number] prefix at the start of each line and output ONLY the translated lines:\n\n%2"
                ).arg(targetLang, batchLines.join("\n"));

                QString endpoint = ollamaEndpoint.trimmed();
                if (endpoint.endsWith("/")) endpoint.chop(1);
                QNetworkRequest req(QUrl(endpoint + "/api/generate"));
                req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                QJsonObject options;
                options["num_gpu"] = (aiUseGpu && !m_detectedGpuName.isEmpty()) ? -1 : 0;
                options["temperature"] = 0.1;

                QJsonObject body;
                body["model"] = translationModel.trimmed().toLower();
                body["prompt"] = prompt;
                body["stream"] = false;
                body["options"] = options;

                QNetworkReply *reply = m_netManager->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
                connect(reply, &QNetworkReply::finished, this, [this, reply, safeView, targetLang, endIndex, srcArr, translateBatchPtr]() {
                    QByteArray raw = reply->readAll();
                    if (reply->error() == QNetworkReply::NoError && safeView && safeView->page()) {
                        QJsonObject obj = QJsonDocument::fromJson(raw).object();
                        QString translatedText = obj["response"].toString().trimmed();
                        if (!translatedText.isEmpty()) {
                            QJsonObject payload;
                            payload["raw"] = translatedText;
                            payload["lang"] = targetLang;
                            payload["model"] = translationModel;
                            payload["done"] = (endIndex >= srcArr.size());
                            QString jsonArg = QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));

                            safeView->page()->runJavaScript(QString(R"(
                                (function(data) {
                                    const lines = data.raw.split('\n');
                                    for (const line of lines) {
                                        const m = line.match(/^\s*\[(\d+)\]\s*(.+)$/);
                                        if (m && window.__blTransNodes) {
                                            const idx = parseInt(m[1], 10);
                                            if (window.__blTransNodes[idx]) {
                                                window.__blTransNodes[idx].nodeValue = ' ' + m[2].trim() + ' ';
                                            }
                                        }
                                    }
                                    let badge = document.getElementById('__bl_trans_badge');
                                    if (!badge) {
                                        badge = document.createElement('div');
                                        badge.id = '__bl_trans_badge';
                                        badge.style.cssText = 'position:fixed; bottom:16px; right:16px; z-index:2147483647; background:#1e2229; color:#e8eaed; border:1px solid #de5833; border-radius:10px; padding:10px 16px; font-family:sans-serif; font-size:13px; box-shadow:0 6px 20px rgba(0,0,0,0.65);';
                                        document.body.appendChild(badge);
                                    }
                                    badge.textContent = data.done
                                        ? ('🌐 Full site translated to ' + data.lang + ' (' + data.model + ')')
                                        : ('⏳ Translating site to ' + data.lang + '...');
                                    if (data.done) {
                                        setTimeout(() => { if (badge) badge.remove(); }, 5000);
                                    }
                                })(%1);
                            )").arg(jsonArg));
                        }
                        reply->deleteLater();
                        if (endIndex < srcArr.size()) {
                            (*translateBatchPtr)(endIndex);
                        } else {
                            btnTranslate->setText("🌐 Translate");
                            btnTranslate->setEnabled(true);
                        }
                    } else {
                        btnTranslate->setText("🌐 Translate");
                        btnTranslate->setEnabled(true);
                        QMessageBox::warning(this, "Translation Model Missing",
                                             QString("Could not translate with '%1'. Open Settings -> BraveLite AI and click '⬇ Pull / Download Translation Model'.").arg(translationModel));
                        reply->deleteLater();
                    }
                });
            };

            (*translateBatchPtr)(0);
        });
    }

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
        CustomWebView *view = activeWebView();
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

    // Crash-Proof Hover Link Preview & Summarizer
    void handleLinkHoverSummarize(const QString &targetUrl, const QString &linkText) {
        if (gamingModeActive || !aiEnabled || !aiHoverSummaryEnabled) return;
        CustomWebView *view = activeWebView();
        if (!view || !view->page()) return;

        QUrl url(targetUrl);
        if (!url.isValid() || (!url.scheme().startsWith("http"))) return;

        quint64 reqId = ++m_hoverRequestId;

        if (m_activeHoverReply) {
            m_activeHoverReply->disconnect();
            m_activeHoverReply->abort();
            m_activeHoverReply->deleteLater();
            m_activeHoverReply = nullptr;
        }

        QJsonObject initPayload;
        initPayload["url"] = targetUrl;
        initPayload["domain"] = url.host();
        initPayload["title"] = linkText.isEmpty() ? url.host() : linkText;
        initPayload["summary"] = QString("⏳ BraveLite AI (%1) is generating preview...").arg(ollamaModel);
        QString initJson = QString::fromUtf8(QJsonDocument(initPayload).toJson(QJsonDocument::Compact));
        view->page()->runJavaScript(
            QString("if (window.__updateBraveLiteAiTooltipObj) window.__updateBraveLiteAiTooltipObj(%1);").arg(initJson)
        );

        QNetworkRequest pageReq(url);
        pageReq.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        pageReq.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
        pageReq.setTransferTimeout(2000);

        QNetworkReply *pageReply = m_netManager->get(pageReq);
        m_activeHoverReply = pageReply;
        QPointer<CustomWebView> safeView(view);

        connect(pageReply, &QNetworkReply::finished, this, [this, pageReply, safeView, targetUrl, linkText, url, reqId]() {
            if (m_activeHoverReply == pageReply) {
                m_activeHoverReply = nullptr;
            }
            if (reqId != m_hoverRequestId || gamingModeActive) {
                pageReply->deleteLater();
                return;
            }

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
                extractedText = rawHtml.simplified().left(850);
            }
            pageReply->deleteLater();

            QString prompt = QString(
                "Summarize what this webpage is about in 2 short, factual sentences without inventing details.\n"
                "URL: %1\nLink Text: %2\nPage Title: %3\nPage Excerpt: %4"
            ).arg(targetUrl,
                  linkText.isEmpty() ? "(none)" : linkText,
                  extractedTitle.isEmpty() ? url.host() : extractedTitle,
                  extractedText.isEmpty() ? "(Summarize based on URL and Link Text)" : extractedText);

            QString finalTitle = extractedTitle.isEmpty() ? (linkText.isEmpty() ? url.host() : linkText) : extractedTitle;

            queryOllamaAsync(prompt, [this, safeView, targetUrl, url, finalTitle, reqId](const QString &aiResponse, bool ok) {
                if (reqId != m_hoverRequestId || gamingModeActive || !safeView || !safeView->page()) return;
                QJsonObject payload;
                payload["url"] = targetUrl;
                payload["domain"] = url.host();
                payload["title"] = finalTitle;
                payload["summary"] = ok ? aiResponse.trimmed() : ("⚠ " + aiResponse);
                QString jsonArg = QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));
                safeView->page()->runJavaScript(
                    QString("if (window.__updateBraveLiteAiTooltipObj) window.__updateBraveLiteAiTooltipObj(%1);").arg(jsonArg)
                );
            });
        });
    }

    // ---------------- "BraveLite Engine": PERSISTENT MULTI-SEARCH IN SAME TAB + TAB HIGHLIGHTS + BACK/REFRESH ----------------
    void runBraveLiteSearch(const QString &query, const QString &tabType = "all") {
        if (query.trimmed().isEmpty()) return;
        CustomWebView *view = activeWebView();
        if (!view) {
            view = createNewTabWithView();
        }

        QString cleanTab = tabType.toLower().trimmed();
        if (cleanTab != "images" && cleanTab != "videos" && cleanTab != "news") {
            cleanTab = "all";
        }

        // Increment per-view search ID and clear stale results so searching 2nd/3rd/10th time in the same tab always updates cleanly!
        quint64 currentSearchId = ++view->activeSearchId;
        view->lastSearchQuery = query;
        view->lastSearchTab = cleanTab;
        view->pendingWebResultsJson.clear();
        view->pendingImageResultsJson.clear();
        view->pendingAiOverviewJsArg.clear();

        QString displaySearchUrl = QString("bravelite://search?q=%1&tab=%2")
        .arg(QString::fromUtf8(QUrl::toPercentEncoding(query)), cleanTab);
        QUrl internalBaseUrl(QString("https://search.bravelite.local/?q=%1&tab=%2&sid=%3")
        .arg(QString::fromUtf8(QUrl::toPercentEncoding(query)), cleanTab).arg(currentSearchId));
        urlBar->setText(displaySearchUrl);

        if (!m_isIncognito) {
            QString histEntry = QString("BraveLite Engine: %1 (%2)").arg(query, displaySearchUrl);
            if (historyList.isEmpty() || historyList.last() != histEntry) {
                historyList.append(histEntry);
                savePersistentData();
            }
        }

        int i = tabStack->indexOf(view);
        if (i != -1) tabBar->setTabText(i, (query.left(12) + " at BraveLite Engine"));

        QString escapedQuery = query.toHtmlEscaped();
        bool showAiCard = (aiEnabled && aiSearchOverviewEnabled && !gamingModeActive && cleanTab == "all");

        QString googleAiOverviewHtml = showAiCard ? QString(
            "<div class='g-ai-wrapper' id='aiOverviewCard'>"
            "  <div class='g-ai-header'>"
            "    <div class='g-ai-title-row'>"
            "      <div class='bl-ai-logo'>🦁</div>"
            "      <span class='g-ai-label'>BraveLite Engine • AI Overview</span>"
            "      <span class='g-ai-meta'>%1 (%2)</span>"
            "    </div>"
            "  </div>"
            "  <div class='g-ai-grid'>"
            "    <div class='g-ai-main' id='aiOverviewContainer'>"
            "      <div id='aiOverviewText' class='g-ai-text'>"
            "        <div class='shimmer-line' style='width:92%;'></div>"
            "        <div class='shimmer-line' style='width:84%;'></div>"
            "        <div class='shimmer-line' style='width:68%;'></div>"
            "      </div>"
            "      <div id='aiChatBox' class='g-ai-chat-box' style='display:none;'>"
            "        <div id='aiChatHistory' class='g-ai-chat-history'></div>"
            "        <form class='g-ai-chat-form' onsubmit='window.__submitAiFollowup(event)'>"
            "          <input id='aiChatInput' type='text' placeholder='Ask a follow-up or request code examples...' autocomplete='off' />"
            "          <button type='submit'>Ask AI ➔</button>"
            "        </form>"
            "      </div>"
            "      <button id='aiShowMoreBtn' class='g-ai-more-btn' style='display:none;' onclick='window.__toggleAiExpand()'>Show more & Chat with BraveLite AI ▾</button>"
            "    </div>"
            "    <div class='g-ai-sources' id='aiSourcesRail'></div>"
            "  </div>"
            "  <div class='g-ai-footer'>"
            "    <span>Grounded on verified search results • Local Ollama (%1)</span>"
            "    <span>BraveLite Engine v1.5</span>"
            "  </div>"
            "</div>"
        ).arg(ollamaModel.toHtmlEscaped(), aiUseGpu ? "GPU Accelerated" : "CPU") : QString();

        QString ddgSearchPageHtml = R"HTML(
            <!DOCTYPE html><html><head><meta charset='utf-8'>
            <title>__QUERY_ESC__ at BraveLite Engine</title>
            <style>
            * { box-sizing: border-box; }
            body { margin: 0; background: #1c1c1c; color: #eeeeee; font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, Roboto, sans-serif; }
            a { text-decoration: none; }
            ::-webkit-scrollbar { width: 10px; height: 10px; }
            ::-webkit-scrollbar-track { background: #16171a; }
            ::-webkit-scrollbar-thumb { background: #3c4043; border-radius: 5px; border: 2px solid #16171a; }
            ::-webkit-scrollbar-thumb:hover { background: #ff5500; }
            .ddg-header { background: #1c1c1c; border-bottom: 1px solid #2e2e2e; padding: 14px 24px 0 24px; position: sticky; top: 0; z-index: 100; }
            .ddg-top-row { display: flex; align-items: center; gap: 22px; max-width: 1120px; margin-bottom: 12px; }
            .ddg-logo { display: flex; align-items: center; gap: 10px; font-size: 19px; font-weight: 700; color: #fff; cursor: pointer; white-space: nowrap; }
            .ddg-logo-badge { width: 34px; height: 34px; border-radius: 50%; background: linear-gradient(135deg, #ff5500, #de5833); display: flex; align-items: center; justify-content: center; font-size: 19px; }
            .ddg-search-form { flex: 1; max-width: 630px; display: flex; align-items: center; background: #2b2b2b; border: 1px solid #3d3d3d; border-radius: 8px; height: 42px; padding: 0 6px 0 16px; }
            .ddg-search-form:focus-within { border-color: #de5833; background: #242424; }
            .ddg-search-form input { flex: 1; background: transparent; border: none; outline: none; color: #fff; font-size: 15px; }
            .ddg-clear-btn { background: transparent; border: none; color: #888; font-size: 16px; cursor: pointer; padding: 4px 8px; }
            .ddg-clear-btn:hover { color: #fff; }
            .ddg-submit-btn { background: transparent; border: none; color: #de5833; font-size: 17px; cursor: pointer; padding: 6px 10px; font-weight: bold; }
            .ddg-nav-tabs { display: flex; gap: 24px; padding-left: 192px; font-size: 13.5px; }
            .ddg-tab { color: #aaaaaa; padding-bottom: 10px; border-bottom: 3px solid transparent; cursor: pointer; font-weight: 500; user-select: none; }
            .ddg-tab:hover { color: #ffffff; }
            .ddg-tab.active { color: #ffffff; border-bottom-color: #de5833; font-weight: 600; }
            .ddg-main { padding: 20px 24px 60px 192px; max-width: 1120px; }
            "@media (max-width: 860px) { .ddg-nav-tabs { padding-left: 0; } .ddg-main { padding-left: 24px; } }"
            .g-ai-wrapper { background: linear-gradient(180deg, #1f242d 0%, #181b22 100%); border: 1px solid #333a48; border-radius: 16px; padding: 20px 24px 14px 24px; margin-bottom: 26px; max-width: 820px; box-shadow: 0 4px 20px rgba(0,0,0,0.35); }
            .g-ai-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 14px; }
            .g-ai-title-row { display: flex; align-items: center; gap: 10px; }
            .bl-ai-logo { width: 28px; height: 28px; border-radius: 50%; background: linear-gradient(135deg, #ff5500, #de5833); display: flex; align-items: center; justify-content: center; font-size: 16px; }
            .g-ai-label { font-size: 16px; font-weight: 600; color: #e8eaed; }
            .g-ai-meta { font-size: 11.5px; color: #9aa0a6; background: rgba(255,255,255,0.06); padding: 3px 9px; border-radius: 12px; }
            .g-ai-grid { display: flex; gap: 20px; align-items: flex-start; }
            .g-ai-main { flex: 1; min-width: 0; }
            .g-ai-text { font-size: 14.5px; line-height: 1.65; color: #e8eaed; overflow: hidden; transition: max-height 0.25s ease; }
            .g-ai-text.collapsed { max-height: 170px; mask-image: linear-gradient(to bottom, black 68%, transparent 100%); }
            .bl-code-wrap { background: #111318; border: 1px solid #2e3440; border-radius: 8px; margin: 10px 0; overflow: hidden; }
            .bl-code-bar { display: flex; justify-content: space-between; align-items: center; background: #191c24; padding: 5px 12px; font-size: 11.5px; color: #9aa0a6; border-bottom: 1px solid #2e3440; }
            .bl-copy-btn { background: #2b303c; color: #fff; border: 1px solid #3e4556; border-radius: 4px; padding: 3px 9px; font-size: 11px; cursor: pointer; font-weight: 600; }
            .bl-copy-btn:hover { background: #ff5500; border-color: #ff5500; }
            .bl-code-pre { margin: 0; padding: 12px; overflow-x: auto; font-family: 'Consolas', 'JetBrains Mono', monospace; font-size: 13px; color: #8ab4f8; white-space: pre; }
            .g-ai-chat-box { margin-top: 14px; padding-top: 12px; border-top: 1px solid #2e3442; }
            .g-ai-chat-history { max-height: 220px; overflow-y: auto; margin-bottom: 10px; display: flex; flex-direction: column; gap: 8px; }
            .chat-msg-user { align-self: flex-end; background: #ff5500; color: #fff; padding: 7px 12px; border-radius: 12px 12px 2px 12px; font-size: 13px; max-width: 85%; }
            .chat-msg-ai { align-self: flex-start; background: #262b36; color: #e8eaed; border: 1px solid #363d4c; padding: 8px 12px; border-radius: 12px 12px 12px 2px; font-size: 13px; max-width: 92%; line-height: 1.5; }
            .g-ai-chat-form { display: flex; gap: 8px; }
            .g-ai-chat-form input { flex: 1; background: #14171d; border: 1px solid #383f4f; border-radius: 18px; padding: 7px 14px; color: #fff; font-size: 13px; outline: none; }
            .g-ai-chat-form input:focus { border-color: #ff5500; }
            .g-ai-chat-form button { background: #ff5500; color: #fff; border: none; border-radius: 18px; padding: 7px 16px; font-size: 13px; font-weight: 600; cursor: pointer; }
            .g-ai-more-btn { margin-top: 10px; width: 100%; background: #282e3a; border: 1px solid #3c4454; color: #8ab4f8; border-radius: 20px; padding: 8px 16px; font-size: 13.5px; font-weight: 600; cursor: pointer; }
            .g-ai-more-btn:hover { background: #313846; }
            .g-ai-sources { width: 210px; display: flex; flex-direction: column; gap: 8px; flex-shrink: 0; }
            .g-source-chip { display: block; background: #262b36; border: 1px solid #363d4c; border-radius: 10px; padding: 8px 10px; color: #e8eaed; font-size: 12px; }
            .g-source-chip:hover { background: #303746; border-color: #8ab4f8; }
            .g-source-title { font-weight: 600; color: #8ab4f8; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; margin-bottom: 3px; }
            .g-source-domain { color: #9aa0a6; font-size: 11px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
            .g-ai-footer { display: flex; justify-content: space-between; margin-top: 14px; padding-top: 10px; border-top: 1px solid #2c323f; font-size: 11.5px; color: #8e9297; }
            @keyframes shimmer { 0% { background-position: -400px 0; } 100% { background-position: 400px 0; } }
            .shimmer-line { height: 14px; margin-bottom: 10px; border-radius: 6px; background: linear-gradient(90deg, #282e3b 25%, #3a4356 50%, #282e3b 75%); background-size: 800px 100%; animation: shimmer 1.5s infinite linear; }
            .ddg-results { max-width: 720px; }
            .ddg-result { padding: 14px 16px; margin: 0 -16px 10px -16px; border-radius: 8px; }
            .ddg-result:hover { background: #242424; }
            .ddg-site-row { display: flex; align-items: center; gap: 8px; margin-bottom: 5px; font-size: 13px; color: #b0b0b0; }
            .ddg-fav { width: 16px; height: 16px; border-radius: 3px; }
            .ddg-title { font-size: 19px; line-height: 1.3; color: #7399ff; font-weight: 500; display: block; margin-bottom: 5px; }
            .ddg-title:hover { text-decoration: underline; }
            .ddg-snippet { font-size: 14px; line-height: 1.55; color: #cccccc; }
            .ddg-image-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(190px, 1fr)); gap: 14px; max-width: 960px; }
            .ddg-img-card { background: #242424; border: 1px solid #333; border-radius: 8px; overflow: hidden; display: block; color: #eee; }
            .ddg-img-card:hover { border-color: #de5833; }
            .ddg-img-card img { width: 100%; height: 140px; object-fit: cover; display: block; background: #151515; }
            .ddg-img-caption { padding: 8px 10px; font-size: 12px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
            </style></head><body>
            <div class='ddg-header'>
            <div class='ddg-top-row'>
            <div class='ddg-logo' onclick="console.log('BRAVELITE_SEARCH::' + document.getElementById('qInput').value);">
            <div class='ddg-logo-badge'>🦁</div>
            <span>BraveLite Engine</span>
            </div>
            <form class='ddg-search-form' onsubmit="event.preventDefault(); console.log('BRAVELITE_SEARCH_TAB::' + JSON.stringify({q: document.getElementById('qInput').value, tab: '__TAB_TYPE__'}));">
            <input id='qInput' type='text' value="__QUERY_ESC__" placeholder='Search privately with BraveLite Engine...' autocomplete='off' />
            <button type='button' class='ddg-clear-btn' onclick="var i=document.getElementById('qInput'); i.value=''; i.focus();">✕</button>
            <button type='submit' class='ddg-submit-btn'>🔍</button>
            </form>
            </div>
            <div class='ddg-nav-tabs'>
            <a class='ddg-tab __TAB_ALL_ACTIVE__' onclick="console.log('BRAVELITE_SEARCH_TAB::' + JSON.stringify({q: document.getElementById('qInput').value, tab: 'all'}));">🔍 All</a>
            <a class='ddg-tab __TAB_IMAGES_ACTIVE__' onclick="console.log('BRAVELITE_SEARCH_TAB::' + JSON.stringify({q: document.getElementById('qInput').value, tab: 'images'}));">🖼 Images</a>
            <a class='ddg-tab __TAB_VIDEOS_ACTIVE__' onclick="console.log('BRAVELITE_SEARCH_TAB::' + JSON.stringify({q: document.getElementById('qInput').value, tab: 'videos'}));">🎬 Videos</a>
            <a class='ddg-tab __TAB_NEWS_ACTIVE__' onclick="console.log('BRAVELITE_SEARCH_TAB::' + JSON.stringify({q: document.getElementById('qInput').value, tab: 'news'}));">📰 News</a>
            </div>
            </div>
            <div class='ddg-main'>
            __AI_OVERVIEW_HTML__
            <div class='ddg-results' id='webResults'>
            <div class='ddg-result'><div class='ddg-snippet'>Searching BraveLite Engine (__TAB_TYPE__) for <b>__QUERY_ESC__</b>...</div></div>
            </div>
            </div>
            <script>
            window.__copyCode = function(btn) {
                if (btn && btn.parentElement && btn.parentElement.nextElementSibling) {
                    navigator.clipboard.writeText(btn.parentElement.nextElementSibling.innerText);
                    btn.textContent = "✔ Copied";
                }
            };
            window.__formatAiMarkdown = function(raw) {
                var safe = raw.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
                safe = safe.replace(/```([a-zA-Z0-9_-]*)\n([\s\S]*?)```/g, function(_, lang, code) {
                    var lbl = lang ? lang : 'code';
                return '<div class="bl-code-wrap"><div class="bl-code-bar"><span>' + lbl + '</span>' +
                '<button type="button" class="bl-copy-btn" onclick="window.__copyCode(this)">📋 Copy code</button></div>' +
                '<pre class="bl-code-pre">' + code + '</pre></div>';
                });
                safe = safe.replace(/`([^`\n]+)`/g, '<code style="background:#111318;color:#8ab4f8;padding:2px 6px;border-radius:4px;">$1</code>');
                safe = safe.replace(/\*\*(.+?)\*\*/g, '<b>$1</b>');
                safe = safe.replace(/\n/g, '<br>');
                return safe;
            };
            window.__aiExpanded = false;
            window.__toggleAiExpand = function() {
                var t = document.getElementById('aiOverviewText');
                var b = document.getElementById('aiShowMoreBtn');
                var c = document.getElementById('aiChatBox');
                if (!t || !b) return;
                window.__aiExpanded = !window.__aiExpanded;
                if (window.__aiExpanded) {
                    t.classList.remove('collapsed');
                    if (c) c.style.display = 'block';
                    b.textContent = 'Show less ▴';
                } else {
                    t.classList.add('collapsed');
                    if (c) c.style.display = 'none';
                    b.textContent = 'Show more & Chat with BraveLite AI ▾';
                }
            };
            window.__submitAiFollowup = function(e) {
                e.preventDefault();
                var inp = document.getElementById('aiChatInput');
                var hist = document.getElementById('aiChatHistory');
                if (!inp || !hist || !inp.value.trim()) return;
                var q = inp.value.trim();
                inp.value = '';
                var uDiv = document.createElement('div'); uDiv.className = 'chat-msg-user'; uDiv.textContent = q; hist.appendChild(uDiv);
                var aDiv = document.createElement('div'); aDiv.className = 'chat-msg-ai'; aDiv.id = 'pendingAiReply'; aDiv.textContent = '⏳ Thinking...'; hist.appendChild(aDiv);
                hist.scrollTop = hist.scrollHeight;
                console.log('BRAVELITE_AI_CHAT::Context: __QUERY_ESC__ | Followup: ' + q);
            };
            window.__appendAiChat = function(obj) {
                var pending = document.getElementById('pendingAiReply');
                if (pending) { pending.removeAttribute('id'); pending.innerHTML = window.__formatAiMarkdown(obj.reply); }
            };
            window.__setAiOverview = function(text) {
                var el = document.getElementById('aiOverviewText');
                var btn = document.getElementById('aiShowMoreBtn');
                if (!el) return;
                el.innerHTML = window.__formatAiMarkdown(text);
                if (btn) btn.style.display = 'block';
                if (text && text.length > 340) el.classList.add('collapsed');
            };
                window.__setImageResults = function(items) {
                    var container = document.getElementById('webResults');
                    if (!container) return;
                    container.className = 'ddg-image-grid';
                    container.innerHTML = '';
                    if (!items || items.length === 0) {
                        container.innerHTML = '<div class="ddg-result"><div class="ddg-snippet">No images found for this query.</div></div>';
                        return;
                    }
                    items.forEach(function(it) {
                        var card = document.createElement('a');
                        card.className = 'ddg-img-card';
                    card.href = it.url;
                    card.innerHTML = '<img src="' + it.img + '" loading="lazy" /><div class="ddg-img-caption">' + it.title + '</div>';
                    container.appendChild(card);
                    });
                };
                window.__setWebResults = function(items) {
                    var container = document.getElementById('webResults');
                    var rail = document.getElementById('aiSourcesRail');
                    if (!container) return;
                    container.className = 'ddg-results';
                    container.innerHTML = '';
                    if (rail) rail.innerHTML = '';
                    if (!items || items.length === 0) {
                        container.innerHTML = '<div class="ddg-result"><div class="ddg-snippet">No instant results found.</div></div>';
                        return;
                    }
                    items.forEach(function(it, idx) {
                        var host = '';
                    try { host = (new URL(it.url)).hostname; } catch(e) { host = it.url; }
                    var fav = 'https://www.google.com/s2/favicons?sz=32&domain_url=' + encodeURIComponent(it.url);
                    var card = document.createElement('div');
                    card.setAttribute('class', 'ddg-result');
                    card.innerHTML = '<div class="ddg-site-row"><img class="ddg-fav" src="' + fav + '" /><span>' + host + '</span></div>' +
                    '<a class="ddg-title" href="' + it.url + '">' + it.title + '</a>' +
                    '<div class="ddg-snippet">' + it.snippet + '</div>';
                    container.appendChild(card);
                    if (rail && idx < 3) {
                        var chip = document.createElement('a');
                        chip.setAttribute('class', 'g-source-chip');
                        chip.href = it.url;
                        chip.innerHTML = '<div class="g-source-title">' + it.title + '</div><div class="g-source-domain">🔗 ' + host + '</div>';
                    rail.appendChild(chip);
                    }
                    });
                };
                </script></body></html>
                )HTML";

        ddgSearchPageHtml.replace("__QUERY_ESC__", escapedQuery);
        ddgSearchPageHtml.replace("__AI_OVERVIEW_HTML__", googleAiOverviewHtml);
        ddgSearchPageHtml.replace("__TAB_TYPE__", cleanTab);
        ddgSearchPageHtml.replace("__TAB_ALL_ACTIVE__", cleanTab == "all" ? "active" : "");
        ddgSearchPageHtml.replace("__TAB_IMAGES_ACTIVE__", cleanTab == "images" ? "active" : "");
        ddgSearchPageHtml.replace("__TAB_VIDEOS_ACTIVE__", cleanTab == "videos" ? "active" : "");
        ddgSearchPageHtml.replace("__TAB_NEWS_ACTIVE__", cleanTab == "news" ? "active" : "");

        if (auto *customPage = qobject_cast<CustomWebEnginePage*>(view->page())) {
            customPage->isRenderingSearchHtml = true;
            view->setHtml(ddgSearchPageHtml, internalBaseUrl);
            customPage->isRenderingSearchHtml = false;
        } else {
            view->setHtml(ddgSearchPageHtml, internalBaseUrl);
        }

        QPointer<CustomWebView> safeView(view);

        if (cleanTab == "images") {
            QUrl imgApiUrl("https://commons.wikimedia.org/w/api.php?action=query&generator=search&gsrsearch=" +
            QUrl::toPercentEncoding(query) + "&gsrnamespace=6&prop=imageinfo&iiprop=url&iiurlwidth=420&format=json");
            QNetworkRequest req(imgApiUrl);
            req.setHeader(QNetworkRequest::UserAgentHeader, "BraveLiteEngine/1.5");
            QNetworkReply *rep = m_netManager->get(req);
            connect(rep, &QNetworkReply::finished, this, [rep, safeView, currentSearchId]() {
                if (safeView && safeView->page() && safeView->activeSearchId == currentSearchId && rep->error() == QNetworkReply::NoError) {
                    QJsonObject pages = QJsonDocument::fromJson(rep->readAll()).object()["query"].toObject()["pages"].toObject();
                    QJsonArray imgArr;
                    for (auto it = pages.begin(); it != pages.end(); ++it) {
                        QJsonObject pObj = it.value().toObject();
                        QJsonArray ii = pObj["imageinfo"].toArray();
                        if (!ii.isEmpty()) {
                            QJsonObject info = ii.first().toObject();
                            QJsonObject item;
                            item["title"] = pObj["title"].toString().replace("File:", "");
                            item["img"] = info["thumburl"].toString(info["url"].toString());
                            item["url"] = info["descriptionurl"].toString(info["url"].toString());
                            imgArr.append(item);
                        }
                    }
                    QString jsonStr = QString::fromUtf8(QJsonDocument(imgArr).toJson(QJsonDocument::Compact));
                    safeView->pendingImageResultsJson = jsonStr;
                    for (int delayMs : {80, 280, 600}) {
                        QTimer::singleShot(delayMs, safeView, [safeView, jsonStr, currentSearchId]() {
                            if (safeView && safeView->page() && safeView->activeSearchId == currentSearchId) {
                                safeView->page()->runJavaScript(QString("if (window.__setImageResults) window.__setImageResults(%1);").arg(jsonStr));
                            }
                        });
                    }
                }
                rep->deleteLater();
            });
            return;
        }

        QString searchQueryMod = query;
        if (cleanTab == "videos") searchQueryMod += " site:youtube.com";
        else if (cleanTab == "news") searchQueryMod += " news";

        QUrl ddgUrl("https://html.duckduckgo.com/html/?q=" + QUrl::toPercentEncoding(searchQueryMod));
        QNetworkRequest req(ddgUrl);
        req.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
        QNetworkReply *reply = m_netManager->get(req);

        connect(reply, &QNetworkReply::finished, this, [this, reply, safeView, query, showAiCard, currentSearchId]() {
            if (!safeView || !safeView->page() || safeView->activeSearchId != currentSearchId) {
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
                        if (count < 5) {
                            groundingContext.append(QString("Source [%1] (%2): %3").arg(count + 1).arg(title, snippet));
                        }
                        count++;
                    }
                }
            }
            reply->deleteLater();

            QString jsonStr = QString::fromUtf8(QJsonDocument(resultsArr).toJson(QJsonDocument::Compact));
            safeView->pendingWebResultsJson = jsonStr;
            for (int delayMs : {80, 260, 550}) {
                QTimer::singleShot(delayMs, safeView, [safeView, jsonStr, currentSearchId]() {
                    if (safeView && safeView->page() && safeView->activeSearchId == currentSearchId) {
                        safeView->page()->runJavaScript(QString("if (window.__setWebResults) window.__setWebResults(%1);").arg(jsonStr));
                    }
                });
            }

            if (showAiCard) {
                QString aiPrompt = QString(
                    "User Search Query: \"%1\"\n\n"
                    "Verified Web Search Results:\n%2\n\n"
                    "Instructions:\n"
                    "1. Base your overview STRICTLY on the Verified Web Search Results above. Do NOT invent technical specs not written in the sources.\n"
                    "2. Write a 2-sentence summary followed by 3 bullet points (•).\n"
                    "3. Only use a ```code``` block if the user is asking a programming or command-line question. Do NOT write the literal word ```code``` on non-programming topics."
                ).arg(query, groundingContext.isEmpty() ? "(Answer factually and concisely)" : groundingContext.join("\n"));

                queryOllamaAsync(aiPrompt, [safeView, currentSearchId](const QString &answer, bool ok) {
                    if (!safeView || !safeView->page() || safeView->activeSearchId != currentSearchId) return;
                    QString out = ok ? answer.trimmed() : ("BraveLite AI: " + answer);
                    QJsonArray arr{out};
                    QString escaped = QString::fromUtf8(QJsonDocument(arr).toJson(QJsonDocument::Compact));
                    escaped = escaped.mid(1, escaped.length() - 2);
                    safeView->pendingAiOverviewJsArg = escaped;
                    for (int delayMs : {20, 250, 600}) {
                        QTimer::singleShot(delayMs, safeView, [safeView, escaped, currentSearchId]() {
                            if (safeView && safeView->page() && safeView->activeSearchId == currentSearchId) {
                                safeView->page()->runJavaScript(QString("if (window.__setAiOverview) window.__setAiOverview(%1);").arg(escaped));
                            }
                        });
                    }
                });
            }
        });
    }

    // ============================================================================
    // BraveLite v1.5 - Part 4 of 4 (Final)
    // ============================================================================

private slots:
    void onTrackerBlocked(const QString &originHost, int totalBlocked) {
        Q_UNUSED(totalBlocked);
        if (auto *v = activeWebView()) {
            if (v->url().host().toLower() == originHost) {
                updatePrivacyBadge(v->url());
            }
        }
    }

    void updatePrivacyBadge(const QUrl &url) {
        if (url.isEmpty() || url.scheme() == "about" || url.toString().startsWith("data:") ||
            url.scheme() == "bravelite" || url.host().endsWith(".local")) {
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
        if (auto *v = activeWebView()) {
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

            btnDownload->setStyleSheet("QPushButton { background-color: #2b2d30; border: 1px solid #ff5500; border-radius: 6px; padding: 4px 10px; font-size: 13px; color: #ff5500; font-weight: bold; }");

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
                    btnDownload->setStyleSheet("QPushButton { background-color: #2b2d30; border: 1px solid #52c41a; border-radius: 6px; padding: 4px 10px; font-size: 13px; color: #52c41a; font-weight: bold; }");
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
        DraggableDownloadCard *card = new DraggableDownloadCard(d, parent);
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

        auto syncCardUI = [d, statusLabel, pbar, btnPauseResume, btnCancel, card]() {
            if (d->isFinished) {
                pbar->hide();
                btnPauseResume->hide();
                btnCancel->hide();
                double mbTot = d->totalBytes > 0 ? (d->totalBytes / (1024.0 * 1024.0)) : (d->receivedBytes / (1024.0 * 1024.0));
                statusLabel->setText(QString("✔ Done (%1 MB) — Drag card to copy file to any folder/app").arg(mbTot, 0, 'f', 2));
                statusLabel->setStyleSheet("color: #52c41a; font-weight: bold; font-size: 13px; margin-top: 4px; background: transparent;");
                card->setCursor(Qt::OpenHandCursor);
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

        QLabel *todayLabel = new QLabel("Downloads (Finished files can be dragged directly into folders/apps)", page);
        todayLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;");
        layout->addWidget(todayLabel);

        QScrollArea *scroll = new QScrollArea(page);
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet(
            "QScrollArea { border: none; background: transparent; } "
            "QScrollBar:vertical { background: #1c1c1c; width: 10px; } "
            "QScrollBar::handle:vertical { background: #3c4043; border-radius: 5px; } "
            "QScrollBar::handle:vertical:hover { background: #ff5500; }"
        );

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
        searchBox->setPlaceholderText("🔍 Search browsing & BraveLite Engine history");
        searchBox->setStyleSheet("background: #2b2d30; border: 1px solid #3c4043; border-radius: 20px; padding: 10px 18px; color: #fff; max-width: 650px; font-size: 14px;");
        layout->addWidget(searchBox);

        QHBoxLayout *topBar = new QHBoxLayout();
        QLabel *title = new QLabel("Browsing & Search History", page);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffffff;");
        QPushButton *btnClear = new QPushButton("Clear All History", page);
        btnClear->setStyleSheet("background-color: #35373c; border-radius: 6px; padding: 6px 14px; color: #ff5500; font-weight: bold;");
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(btnClear);
        layout->addLayout(topBar);

        QListWidget *list = new QListWidget(page);
        list->setStyleSheet("background-color: #242529; border: 1px solid #32353b; border-radius: 8px; padding: 12px; color: #fff; font-size: 14px;");

        auto populateHistory = [this, list](const QString &filter = QString()) {
            list->clear();
            QString f = filter.trimmed().toLower();
            for (int i = historyList.size() - 1; i >= 0; --i) {
                if (f.isEmpty() || historyList[i].toLower().contains(f)) {
                    list->addItem(historyList[i]);
                }
            }
        };
        populateHistory();
        layout->addWidget(list);

        connect(searchBox, &QLineEdit::textChanged, page, populateHistory);

        connect(list, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *it) {
            QString text = it->text();
            if (text.startsWith("BraveLite Engine: ")) {
                int paren = text.lastIndexOf("(bravelite://search");
                if (paren != -1) {
                    QString qPart = text.mid(18, paren - 19).trimmed();
                    runBraveLiteSearch(qPart, "all");
                    return;
                }
            }
            addNewTab(QUrl(text));
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
        QLabel *sSub = new QLabel("Choose between BraveLite Engine (DuckDuckGo UI + BraveLite AI Overview) or standard DuckDuckGo.", searchCard);
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

        searchLayout->addLayout(searchTextLayout, 1); // Fixed layout compiler error!
        searchLayout->addWidget(engineCombo);
        layout->addWidget(searchCard);

        layout->addStretch(1);
        return container;
    }

    void detectHardwareGpu() {
        m_detectedGpuName.clear();
        #ifdef Q_OS_WIN
        QProcess p;
        p.start("powershell", {"-Command", "Get-CimInstance Win32_VideoController | Select-Object -ExpandProperty Name"});
        if (p.waitForFinished(1500) && p.exitCode() == 0) {
            QStringList lines = QString::fromUtf8(p.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
            for (const QString &l : lines) {
                QString clean = l.trimmed();
                if (!clean.isEmpty()) {
                    m_detectedGpuName = clean;
                    break;
                }
            }
        }
        if (m_detectedGpuName.isEmpty()) {
            QSettings regGPU("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000", QSettings::NativeFormat);
            QString drvDesc = regGPU.value("DriverDesc").toString();
            if (!drvDesc.isEmpty()) m_detectedGpuName = drvDesc;
        }
        if (m_detectedGpuName.isEmpty()) {
            if (QFileInfo::exists("C:/Program Files/Ollama/ollama.exe") ||
                QFileInfo::exists(QDir::homePath() + "/AppData/Local/Programs/Ollama/ollama.exe")) {
                m_detectedGpuName = "Windows DirectX / Vulkan GPU";
                }
        }
        #else
        QString nvSmi = QStandardPaths::findExecutable("nvidia-smi");
        if (!nvSmi.isEmpty()) {
            QProcess p;
            p.start(nvSmi, {"--query-gpu=name", "--format=csv,noheader"});
            if (p.waitForFinished(1000) && p.exitCode() == 0) {
                m_detectedGpuName = QString::fromUtf8(p.readAllStandardOutput()).trimmed().split('\n').first();
            }
        }
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
        if (m_detectedGpuName.isEmpty() && QFileInfo::exists("/dev/dri/renderD128")) {
            m_detectedGpuName = "Linux DRI Hardware GPU (/dev/dri/renderD128)";
        }
        #endif
    }

    void ensureOllamaServerRunning() {
        QString ollamaBin = QStandardPaths::findExecutable("ollama");
        if (ollamaBin.isEmpty()) {
            #ifdef Q_OS_WIN
            QString localPath = QDir::homePath() + "/AppData/Local/Programs/Ollama/ollama.exe";
            if (QFileInfo::exists(localPath)) ollamaBin = localPath;
            else if (QFileInfo::exists("C:/Program Files/Ollama/ollama.exe")) ollamaBin = "C:/Program Files/Ollama/ollama.exe";
            #endif
        }
        if (!ollamaBin.isEmpty()) {
            #ifdef Q_OS_WIN
            QProcess::startDetached(ollamaBin, {"serve"}, QDir::currentPath(), nullptr);
            #else
            QProcess::startDetached(ollamaBin, {"serve"});
            #endif
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

        QCheckBox *chkOverview = new QCheckBox("Enable BraveLite Engine AI Overview & Interactive Chat on Search Results", card);
        chkOverview->setChecked(aiSearchOverviewEnabled);
        chkOverview->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none;");

        QCheckBox *chkHover = new QCheckBox("Enable Rich Link Previews & AI Summaries when hovering over links", card);
        chkHover->setChecked(aiHoverSummaryEnabled);
        chkHover->setStyleSheet("font-size: 14px; color: #e0e0e0; border: none;");

        QString gpuStatusText = m_detectedGpuName.isEmpty()
        ? "Use GPU Acceleration if available (will fallback to CPU if needed)"
        : QString("Use GPU Acceleration — Detected: %1").arg(m_detectedGpuName);
        QCheckBox *chkGpu = new QCheckBox(gpuStatusText, card);
        chkGpu->setChecked(aiUseGpu);
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
        QStringList presetModels = {"qwen2.5:0.5b", "smollm2:360m", "smollm2:135m", "llama3.2:1b", "phi3:mini", "gemma2:2b", "tinyllama"};
        if (!presetModels.contains(ollamaModel.toLower())) {
            modelCombo->addItem(ollamaModel.toLower());
        }
        modelCombo->addItems(presetModels);
        modelCombo->setCurrentText(ollamaModel.toLower());
        modelCombo->setStyleSheet("QComboBox { background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 10px; color: #fff; }");

        QHBoxLayout *transModelRow = new QHBoxLayout();
        QComboBox *transCombo = new QComboBox(card);
        transCombo->setEditable(true);
        QStringList presetTransModels = {
            "translategemma:4b",
            "translategemma:latest",
            "gemma2:2b",
            "qwen2.5:1.5b",
            "qwen2.5:0.5b",
            "llama3.2:1b",
            "aya-expanse:8b"
        };
        if (!presetTransModels.contains(translationModel.toLower())) {
            transCombo->addItem(translationModel.toLower());
        }
        transCombo->addItems(presetTransModels);
        transCombo->setCurrentText(translationModel.toLower());
        transCombo->setStyleSheet("QComboBox { background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 10px; color: #fff; }");

        QPushButton *btnPullTransModel = new QPushButton("⬇ Pull Translation Model", card);
        btnPullTransModel->setStyleSheet("QPushButton { background: #2d6a4f; color: #fff; font-weight: bold; padding: 6px 14px; border-radius: 6px; border: none; } QPushButton:hover { background: #40916c; }");
        transModelRow->addWidget(transCombo, 1);
        transModelRow->addWidget(btnPullTransModel);

        QLineEdit *sysPromptEdit = new QLineEdit(aiSystemPrompt, card);
        sysPromptEdit->setStyleSheet("background: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 6px 10px; color: #fff;");

        form->addRow("Ollama Endpoint URL:", endpointEdit);
        form->addRow("Overview / Summary Model:", modelCombo);
        form->addRow("Site Translation Model:", transModelRow);
        form->addRow("System Prompt:", sysPromptEdit);
        cLayout->addLayout(form);

        // 1-Click Install Ollama Button (Linux & Windows)
        QHBoxLayout *installOllamaRow = new QHBoxLayout();
        QPushButton *btnInstallOllama = new QPushButton("📥 Install Ollama (Auto-Setup)", card);
        btnInstallOllama->setStyleSheet("QPushButton { background: #1d3557; color: #fff; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: none; } QPushButton:hover { background: #457b9d; }");
        installOllamaRow->addWidget(btnInstallOllama);
        installOllamaRow->addStretch();
        cLayout->addLayout(installOllamaRow);

        QLabel *statusLbl = new QLabel("Status: Ready. Configure Overview and Translation models above.", card);
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
        QPushButton *btnPullModel = new QPushButton("⬇ Pull Overview Model", card);

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

        connect(btnInstallOllama, &QPushButton::clicked, card, [statusLbl]() {
            #ifdef Q_OS_WIN
            bool launched = QProcess::startDetached("winget", {"install", "-e", "--id", "Ollama.Ollama"});
            if (!launched) {
                QDesktopServices::openUrl(QUrl("https://ollama.com/download/OllamaSetup.exe"));
            }
            statusLbl->setText("✔ Launched Ollama installer for Windows!");
            #else
            QString term = QStandardPaths::findExecutable("konsole");
            if (term.isEmpty()) term = QStandardPaths::findExecutable("gnome-terminal");
            if (term.isEmpty()) term = QStandardPaths::findExecutable("xfce4-terminal");
            if (term.isEmpty()) term = QStandardPaths::findExecutable("xterm");

            if (!term.isEmpty()) {
                QProcess::startDetached(term, {"-e", "sh -c 'curl -fsSL https://ollama.com/install.sh | sh; echo; read -p \"Press Enter to close...\"'"});
            } else {
                QProcess::startDetached("sh", {"-c", "curl -fsSL https://ollama.com/install.sh | sh"});
            }
            statusLbl->setText("✔ Launched Ollama installer in terminal!");
            #endif
            statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
        });

        auto pullSpecificModel = [this, card, endpointEdit, statusLbl, pullProgressBar](const QString &modelToPull) {
            ensureOllamaServerRunning();
            QString base = endpointEdit->text().trimmed();
            if (base.endsWith("/")) base.chop(1);

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
                    statusLbl->setText("✔ Model '" + modelToPull + "' is downloaded and ready!");
                    statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
                } else {
                    statusLbl->setText("✖ Failed to pull '" + modelToPull + "': " + rep->errorString());
                    statusLbl->setStyleSheet("font-size: 13px; color: #f5222d; border: none;");
                }
                rep->deleteLater();
            });
        };

        connect(btnPullModel, &QPushButton::clicked, card, [this, modelCombo, pullSpecificModel]() {
            ollamaModel = modelCombo->currentText().trimmed().toLower();
            savePersistentData();
            pullSpecificModel(ollamaModel);
        });

        connect(btnPullTransModel, &QPushButton::clicked, card, [this, transCombo, pullSpecificModel]() {
            translationModel = transCombo->currentText().trimmed().toLower();
            savePersistentData();
            pullSpecificModel(translationModel);
        });

        connect(btnRedetectGpu, &QPushButton::clicked, card, [this, chkGpu, statusLbl]() {
            detectHardwareGpu();
            if (!m_detectedGpuName.isEmpty()) {
                chkGpu->setText("Use GPU Acceleration — Detected: " + m_detectedGpuName);
                chkGpu->setChecked(true);
                aiUseGpu = true;
                savePersistentData();
                statusLbl->setText("✔ GPU Detected: " + m_detectedGpuName);
                statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
            } else {
                chkGpu->setText("Use GPU Acceleration if available");
                statusLbl->setText("⚠ No dedicated GPU detected; will run via CPU.");
                statusLbl->setStyleSheet("font-size: 13px; color: #faad14; border: none;");
            }
        });

        connect(btnSaveAi, &QPushButton::clicked, card, [this, endpointEdit, modelCombo, transCombo, sysPromptEdit, statusLbl]() {
            ollamaEndpoint = endpointEdit->text().trimmed();
            if (ollamaEndpoint.endsWith("/")) ollamaEndpoint.chop(1);
            ollamaModel = modelCombo->currentText().trimmed().toLower();
            translationModel = transCombo->currentText().trimmed().toLower();
            aiSystemPrompt = sysPromptEdit->text().trimmed();
            savePersistentData();
            statusLbl->setText(QString("✔ Saved! Overview Model: %1 | Translation Model: %2").arg(ollamaModel, translationModel));
            statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
        });

        connect(btnStartServer, &QPushButton::clicked, card, [statusLbl]() {
            QString ollamaBin = QStandardPaths::findExecutable("ollama");
            if (ollamaBin.isEmpty()) {
                #ifdef Q_OS_WIN
                QString localPath = QDir::homePath() + "/AppData/Local/Programs/Ollama/ollama.exe";
                if (QFileInfo::exists(localPath)) ollamaBin = localPath;
                else if (QFileInfo::exists("C:/Program Files/Ollama/ollama.exe")) ollamaBin = "C:/Program Files/Ollama/ollama.exe";
                #endif
            }
            if (ollamaBin.isEmpty()) {
                statusLbl->setText("✖ 'ollama' binary not found. Click '📥 Install Ollama' above.");
                statusLbl->setStyleSheet("font-size: 13px; color: #f5222d; border: none;");
                return;
            }
            #ifdef Q_OS_WIN
            QProcess::startDetached(ollamaBin, {"serve"}, QDir::currentPath(), nullptr);
            #else
            QProcess::startDetached(ollamaBin, {"serve"});
            #endif
            statusLbl->setText("✔ Launched 'ollama serve' in the background!");
            statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
        });

        // Fixes bug where 'Detect Installed Models' overwrote overview model with translategemma:4b
        connect(btnFetchModels, &QPushButton::clicked, card, [this, card, endpointEdit, modelCombo, transCombo, statusLbl]() {
            ensureOllamaServerRunning();
            QString base = endpointEdit->text().trimmed();
            if (base.endsWith("/")) base.chop(1);
            QString currentOverview = modelCombo->currentText();
            QString currentTrans = transCombo->currentText();

            QNetworkReply *rep = m_netManager->get(QNetworkRequest(QUrl(base + "/api/tags")));
            statusLbl->setText("⏳ Querying Ollama /api/tags...");
            connect(rep, &QNetworkReply::finished, card, [rep, modelCombo, transCombo, currentOverview, currentTrans, statusLbl]() {
                if (rep->error() == QNetworkReply::NoError) {
                    QJsonArray models = QJsonDocument::fromJson(rep->readAll()).object()["models"].toArray();
                    if (!models.isEmpty()) {
                        modelCombo->clear();
                        transCombo->clear();
                        for (auto m : models) {
                            QString name = m.toObject()["name"].toString();
                            modelCombo->addItem(name);
                            transCombo->addItem(name);
                        }
                        modelCombo->setCurrentText(currentOverview);
                        transCombo->setCurrentText(currentTrans);
                        statusLbl->setText(QString("✔ Found %1 installed local Ollama model(s)!").arg(models.size()));
                        statusLbl->setStyleSheet("font-size: 13px; color: #52c41a; border: none;");
                    } else {
                        statusLbl->setText("⚠ Ollama is running, but no models are downloaded yet.");
                    }
                } else {
                    statusLbl->setText("✖ Could not connect to Ollama: " + rep->errorString());
                }
                rep->deleteLater();
            });
        });

        connect(btnTestAi, &QPushButton::clicked, card, [this, endpointEdit, modelCombo, transCombo, sysPromptEdit, statusLbl]() {
            ensureOllamaServerRunning();
            ollamaEndpoint = endpointEdit->text().trimmed();
            if (ollamaEndpoint.endsWith("/")) ollamaEndpoint.chop(1);
            ollamaModel = modelCombo->currentText().trimmed().toLower();
            translationModel = transCombo->currentText().trimmed().toLower();
            aiSystemPrompt = sysPromptEdit->text().trimmed();
            savePersistentData();
            statusLbl->setText(QString("⏳ Asking BraveLite AI (%1)...").arg(ollamaModel));
            queryOllamaAsync("Say hello in one short sentence as BraveLite AI.", [statusLbl](const QString &resp, bool ok) {
                statusLbl->setText(ok ? ("✨ BraveLite AI: " + resp.trimmed()) : ("✖ " + resp));
                statusLbl->setStyleSheet(QString("font-size: 13px; color: %1; border: none;").arg(ok ? "#52c41a" : "#f5222d"));
            });
        });

        layout->addWidget(card);
        layout->addStretch(1);
        return container;
    }

    // ---------------- EXTENSIONS & USERSCRIPTS DASHBOARD ----------------
    QWidget* createExtensionsWidget(QWidget *parent = nullptr) {
        QWidget *container = new QWidget(parent);
        QVBoxLayout *layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(14);

        QWidget *card = new QWidget(container);
        card->setStyleSheet("background: #242529; border: 1px solid #32353b; border-radius: 8px;");
        QVBoxLayout *cLayout = new QVBoxLayout(card);
        cLayout->setContentsMargins(20, 18, 20, 18);
        cLayout->setSpacing(12);

        QLabel *lbl = new QLabel("Userscript & Web Extensions Engine", card);
        lbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff; border: none;");
        QLabel *desc = new QLabel("Install and run custom userscripts (.user.js) and unpacked extensions directly in BraveLite. Drop files into your Extensions folder:", card);
        desc->setWordWrap(true);
        desc->setStyleSheet("font-size: 13px; color: #9aa0a6; border: none;");

        cLayout->addWidget(lbl);
        cLayout->addWidget(desc);

        QString extDir = getConfigDir() + "/Extensions";
        QDir().mkpath(extDir);

        QListWidget *extList = new QListWidget(card);
        extList->setStyleSheet("background-color: #1e1f22; border: 1px solid #3c4043; border-radius: 6px; padding: 8px; color: #fff; font-size: 13px;");

        QDir dir(extDir);
        QStringList scripts = dir.entryList({"*.user.js", "*.js"}, QDir::Files);
        if (scripts.isEmpty()) {
            extList->addItem("No custom extensions or .user.js scripts installed yet.");
        } else {
            for (const QString &s : scripts) {
                extList->addItem("🧩 " + s + " (Active)");
            }
        }
        cLayout->addWidget(extList);

        QHBoxLayout *btnRow = new QHBoxLayout();
        QPushButton *btnOpenDir = new QPushButton("📂 Open Extensions Folder", card);
        QPushButton *btnInstallScript = new QPushButton("➕ Install Script File (.user.js)", card);
        btnOpenDir->setStyleSheet("QPushButton { background: #35373c; color: #fff; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: 1px solid #484b52; }");
        btnInstallScript->setStyleSheet("QPushButton { background: #ff5500; color: #fff; font-weight: bold; padding: 8px 16px; border-radius: 6px; border: none; }");

        btnRow->addWidget(btnInstallScript);
        btnRow->addWidget(btnOpenDir);
        btnRow->addStretch();
        cLayout->addLayout(btnRow);

        connect(btnOpenDir, &QPushButton::clicked, [extDir]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(extDir));
        });

        connect(btnInstallScript, &QPushButton::clicked, card, [this, extDir, extList]() {
            QString src = QFileDialog::getOpenFileName(this, "Select Userscript", QDir::homePath(), "Userscripts (*.user.js *.js)");
            if (!src.isEmpty()) {
                QFileInfo fi(src);
                QString dest = extDir + "/" + fi.fileName();
                QFile::copy(src, dest);
                extList->addItem("🧩 " + fi.fileName() + " (Active)");
                QMessageBox::information(this, "Extension Installed", "Script installed and registered for all pages.");
            }
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
            if (auto *v = activeWebView()) {
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

    void openSettingsTab(int initialCategoryIndex = 0) {
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
        sidebar->addItem("Extensions (Userscripts)");
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
        detailStack->addWidget(createExtensionsWidget(detailStack));
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
                detailTitle->setText("BraveLite AI (Ollama & GPU Configuration)");
                detailStack->setCurrentIndex(1);
                return;
            }
            if (row == 2) {
                detailTitle->setText("Extensions (Userscripts)");
                detailStack->setCurrentIndex(2);
                return;
            }
            if (row == 3) {
                detailTitle->setText("Theme Profiles");
                detailStack->setCurrentIndex(3);
                return;
            }
            if (row == 4) {
                detailTitle->setText("Keybinds & Shortcuts");
                detailStack->setCurrentIndex(4);
                return;
            }
            if (row == 5) {
                detailTitle->setText("Autofill and passwords");
                detailStack->setCurrentIndex(5);
                return;
            }

            detailStack->setCurrentIndex(6);

            if (row == 6) {
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
            } else if (row == 7) {
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
            } else if (row == 8) {
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
            } else if (row == 9) {
                detailTitle->setText("Permissions Dashboard");
                itemsList->addItem("Inspect and revoke camera, mic, and location permissions.");
                actionBtn1->setText("Open Dashboard Tab");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, this, &BrowserWindow::openPermissionsTab);
            } else if (row == 10) {
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
            } else if (row == 11) {
                detailTitle->setText("Delete browsing data");
                itemsList->addItem("Clear Cookies and Web Cache");
                actionBtn1->setText("Delete All Cookies");
                actionBtn1->show();
                connect(actionBtn1, &QPushButton::clicked, [this]() {
                    if (m_profile) m_profile->cookieStore()->deleteAllCookies();
                    QMessageBox::information(this, "Cleared", "Cookies and storage cleared.");
                });
            } else if (row == 12) {
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
        sidebar->setCurrentRow(initialCategoryIndex);

        hLayout->addWidget(sidebar);
        hLayout->addWidget(content, 1);

        int idx = tabStack->addWidget(page);
        tabBar->addTab("Settings");
        tabBar->setCurrentIndex(idx);
    }

    void openAppMenu() {
        QMenu menu(this);
        menu.setStyleSheet(R"(
            QMenu { background-color: #202124; border: 1px solid #3c4043; color: #e8eaed; padding: 6px 0px; font-size: 13px; border-radius: 8px; }
            QMenu::item { padding: 6px 28px 6px 20px; }
            QMenu::item:selected { background-color: #ff5500; color: #fff; }
            QMenu::separator { height: 1px; background-color: #3c4043; margin: 4px 0px; }
        )");

        menu.addAction("New tab\t" + keybindsMap.value("NewTab"), this, [this]() { addNewTab(); });
        menu.addAction("New window\t" + keybindsMap.value("NewWindow"), this, [this]() { (new BrowserWindow(false))->show(); });
        menu.addAction("New private window\t" + keybindsMap.value("Incognito"), this, &BrowserWindow::openIncognito);
        menu.addSeparator();

        QAction *actGaming = menu.addAction(gamingModeActive
        ? "🎮 Gaming Mode: ON (Click to Re-enable AI)"
        : "🎮 Gaming Mode: OFF (Click to Optimize for Gaming)");
        connect(actGaming, &QAction::triggered, this, &BrowserWindow::toggleGamingMode);

        menu.addSeparator();

        QMenu *zoomMenu = menu.addMenu("Zoom");
        zoomMenu->addAction("Zoom In\t" + keybindsMap.value("ZoomIn"), this, &BrowserWindow::zoomIn);
        zoomMenu->addAction("Zoom Out\t" + keybindsMap.value("ZoomOut"), this, &BrowserWindow::zoomOut);
        zoomMenu->addAction("Reset Zoom (100%)\t" + keybindsMap.value("ZoomReset"), this, &BrowserWindow::zoomReset);

        menu.addAction("✨ BraveLite AI Settings", this, [this]() { openSettingsTab(1); });
        menu.addAction("Passwords && Autofill", this, &BrowserWindow::openPasswordManagerDialog);
        menu.addAction("History\t" + keybindsMap.value("History"), this, &BrowserWindow::openHistoryTab);
        menu.addAction("Bookmarks Manager", this, [this]() { openSettingsTab(6); });
        menu.addAction("Extensions (Userscripts)", this, [this]() { openSettingsTab(2); });
        menu.addAction("Downloads\t" + keybindsMap.value("Downloads"), this, &BrowserWindow::openDownloadsTab);
        menu.addAction("Site Permissions", this, &BrowserWindow::openPermissionsTab);

        menu.addAction("Inspect Element", this, [this]() {
            openInspectElementWindow(activeWebView());
        });

        menu.addAction("Delete browsing data...\tCtrl+Shift+Del", this, [this]() {
            if (QMessageBox::question(this, "Clear Data", "Delete all cookies and history?") == QMessageBox::Yes) {
                if (m_profile) m_profile->cookieStore()->deleteAllCookies();
                historyList.clear();
                savePersistentData();
                QMessageBox::information(this, "Cleared", "Cookies and history cleared.");
            }
        });

        menu.addSeparator();
        menu.addAction("Settings", this, [this]() { openSettingsTab(0); });
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
                QMessageBox::information(this, "Firefox Import", "Found Firefox profile.");
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
                QMessageBox::information(this, "Firefox Import", "Found Firefox profile.");
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
        if (auto *v = activeWebView()) {
            v->setZoomFactor(v->zoomFactor() + 0.1);
        }
    }

    void zoomOut() {
        if (auto *v = activeWebView()) {
            if (v->zoomFactor() > 0.3) v->setZoomFactor(v->zoomFactor() - 0.1);
        }
    }

    void zoomReset() {
        if (auto *v = activeWebView()) {
            v->setZoomFactor(1.0);
        }
    }

private:
    bool m_isIncognito;
    bool m_isDefaultBrowserSaved = false;
    bool m_wasMaximizedBeforeFullScreen = false;
    bool m_inFullScreenTransition = false;
    bool gamingModeActive = false;
    quint64 m_hoverRequestId = 0;
    QWebEngineProfile *m_profile;
    QNetworkAccessManager *m_netManager;
    QPointer<QNetworkReply> m_activeHoverReply;
    QSet<QString> m_pendingFaviconHosts;
    AdBlocker *m_adBlocker;
    QWidget *headerContainer;
    CustomTabBar *tabBar;
    QStackedWidget *tabStack;
    DroppableUrlBar *urlBar;
    QPushButton *btnSplitView;
    QPushButton *btnTranslate;
    QComboBox *langCombo;
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
    QString ollamaModel = "qwen2.5:0.5b";
    QString translationModel = "translategemma:4b";
    QString aiSystemPrompt = "You are BraveLite AI. State only factual, grounded specifications present in the context. If specs (engine, battery, dimensions) are not verifiable, explicitly state 'Not listed' rather than guessing. Never fabricate engine configurations.";

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
            if (auto *v = activeWebView()) {
                if (v->url().host() == "search.bravelite.local" && !v->lastSearchQuery.isEmpty()) {
                    runBraveLiteSearch(v->lastSearchQuery, v->lastSearchTab);
                } else {
                    v->reload();
                }
            }
        });
        bindKey(keybindsMap.value("ZoomIn"), [this]() { zoomIn(); });
        bindKey("Ctrl+=", [this]() { zoomIn(); });
        bindKey(keybindsMap.value("ZoomOut"), [this]() { zoomOut(); });
        bindKey(keybindsMap.value("ZoomReset"), [this]() { zoomReset(); });
        bindKey("Esc", [this]() {
            if (isFullScreen()) {
                if (auto *v = activeWebView()) {
                    v->page()->triggerAction(QWebEnginePage::ExitFullScreen);
                }
                exitBrowserFullScreen();
            }
        });
    }

    void queryOllamaAsync(const QString &prompt, std::function<void(const QString&, bool)> callback) {
        if (gamingModeActive) {
            callback("Gaming Mode is currently active.", false);
            return;
        }

        QString endpoint = ollamaEndpoint.trimmed();
        if (endpoint.endsWith("/")) endpoint.chop(1);
        QUrl apiUrl(endpoint + "/api/generate");

        QNetworkRequest req(apiUrl);
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject options;
        options["num_gpu"] = (aiUseGpu && !m_detectedGpuName.isEmpty()) ? -1 : 0;
        options["temperature"] = 0.1;
        options["top_p"] = 0.85;

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
                    callback("Ollama error: " + obj["error"].toString() + " — Click '⬇ Pull Overview Model' in Settings.", false);
                } else {
                    QString resp = obj["response"].toString();
                    callback(resp, !resp.isEmpty());
                }
            } else {
                QJsonObject errObj = QJsonDocument::fromJson(raw).object();
                if (errObj.contains("error")) {
                    callback("Model '" + ollamaModel + "' is not downloaded (" + errObj["error"].toString() + "). Click '⬇ Pull Overview Model' in Settings!", false);
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

            QString jsCode =
            "(function() {\n"
            "  try {\n"
            "    const root = document.head || document.documentElement;\n"
            "    if (root && !document.getElementById('__bravelite_cosmetic')) {\n"
            "      const s = document.createElement('style');\n"
            "      s.id = '__bravelite_cosmetic';\n"
            "      s.textContent = '::-webkit-scrollbar { width: 10px; height: 10px; } ::-webkit-scrollbar-track { background: #16171a; } ::-webkit-scrollbar-thumb { background: #3c4043; border-radius: 5px; border: 2px solid #16171a; } ::-webkit-scrollbar-thumb:hover { background: #ff5500; } .adbox, .banner_ads, .adsbox, .textads, div[id*=\"google_ads\"], div[id*=\"ad-slot\"], #ad-container, .adsbygoogle, ytd-ad-slot-renderer, ytd-in-feed-ad-layout-renderer, ytd-banner-promo-renderer, ytd-statement-banner-renderer, ytd-masthead-ad-v3-renderer, ytd-primetime-promo-renderer, ytd-engagement-panel-section-list-renderer[target-id=\"engagement-panel-ads\"], ytd-merch-shelf-renderer, ytd-action-companion-ad-renderer, .ytd-mealbar-promo-renderer, .ytp-ad-overlay-container, .ytp-ad-message-container, .ytp-ad-image-overlay, .ytp-ad-text-overlay, .ytp-ad-player-overlay-layout, #masthead-ad, #player-ads, .video-ads { display: none !important; visibility: hidden !important; height: 0 !important; width: 0 !important; pointer-events: none !important; }';\n"
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
            QWidget { background-color: %1; color: %7; font-family: 'Segoe UI', 'Segoe UI Emoji', 'Segoe UI Symbol', -apple-system, BlinkMacSystemFont, Roboto, Ubuntu, sans-serif; font-size: 13px; }
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
                padding: 5px 11px;
                color: %7;
                font-size: 13px;
                font-weight: 500;
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
        connect(tabBar, &CustomTabBar::urlDroppedOnTabBar, this, [this](const QUrl &u) { addNewTab(u); });

        NewTabDropButton *newTabBtn = new NewTabDropButton("+", this);
        newTabBtn->setFixedSize(28, 28);
        newTabBtn->setCursor(Qt::PointingHandCursor);
        newTabBtn->setToolTip("New tab (Ctrl+T) — Drag & drop any link or bookmark here to open!");
        newTabBtn->setStyleSheet("QPushButton { font-size: 16px; font-weight: bold; color: #c8ccd0; background: #242629; border: 1px solid #34373d; border-radius: 6px; padding: 0; } QPushButton:hover { background-color: #35373c; color: #fff; }");
        connect(newTabBtn, &QPushButton::clicked, this, [this]() { addNewTab(); });
        connect(newTabBtn, &NewTabDropButton::urlDropped, this, [this](const QUrl &u) { addNewTab(u); });

        tabRowLayout->addWidget(tabBar);
        tabRowLayout->addWidget(newTabBtn);
        tabRowLayout->addStretch();

        QWidget *navBar = new QWidget(this);
        QHBoxLayout *navLayout = new QHBoxLayout(navBar);
        navLayout->setContentsMargins(8, 5, 8, 5);
        navLayout->setSpacing(6);

        QString navBtnStyle = "QPushButton { background-color: #25272b; border: 1px solid #34373d; border-radius: 6px; padding: 4px 10px; color: #e4e6eb; font-size: 14px; font-weight: bold; } QPushButton:hover { background-color: #35373c; border-color: #4b4f57; color: #fff; }";

        QPushButton *btnBack = new QPushButton(this);
        btnBack->setIcon(BrowserIcons::drawIcon("back"));
        btnBack->setIconSize(QSize(20, 20));
        btnBack->setToolTip("Back (Alt+Left)");
        btnBack->setCursor(Qt::PointingHandCursor);
        btnBack->setStyleSheet(navBtnStyle);
        connect(btnBack, &QPushButton::clicked, this, [this]() {
            if (auto *v = activeWebView()) {
                if (!v->lastSearchQuery.isEmpty() && (v->url().host() != "search.bravelite.local" || v->history()->currentItemIndex() <= 1)) {
                    runBraveLiteSearch(v->lastSearchQuery, v->lastSearchTab);
                } else if (v->history()->canGoBack()) {
                    v->back();
                } else if (!v->lastSearchQuery.isEmpty()) {
                    runBraveLiteSearch(v->lastSearchQuery, v->lastSearchTab);
                }
            }
        });

        QPushButton *btnForward = new QPushButton(this);
        btnForward->setIcon(BrowserIcons::drawIcon("forward"));
        btnForward->setIconSize(QSize(20, 20));
        btnForward->setToolTip("Forward (Alt+Right)");
        btnForward->setCursor(Qt::PointingHandCursor);
        btnForward->setStyleSheet(navBtnStyle);
        connect(btnForward, &QPushButton::clicked, this, [this]() {
            if (auto *v = activeWebView()) v->forward();
        });

            QPushButton *btnReload = new QPushButton(this);
            btnReload->setIcon(BrowserIcons::drawIcon("reload"));
            btnReload->setIconSize(QSize(20, 20));
            btnReload->setToolTip("Reload (Ctrl+R)");
            btnReload->setCursor(Qt::PointingHandCursor);
            btnReload->setStyleSheet(navBtnStyle);
            connect(btnReload, &QPushButton::clicked, this, [this]() {
                if (auto *v = activeWebView()) {
                    if (v->url().host() == "search.bravelite.local" && !v->lastSearchQuery.isEmpty()) {
                        runBraveLiteSearch(v->lastSearchQuery, v->lastSearchTab);
                    } else {
                        v->reload();
                    }
                }
            });

            btnSplitView = new QPushButton(this);
            btnSplitView->setIcon(BrowserIcons::drawIcon("split", QColor("#ff5500")));
            btnSplitView->setIconSize(QSize(20, 20));
            btnSplitView->setToolTip("Split View Options");
            btnSplitView->setCursor(Qt::PointingHandCursor);
            btnSplitView->setStyleSheet(
                "QPushButton { background-color: #25272b; border: 1px solid #ff5500; border-radius: 6px; padding: 4px 8px; } "
                "QPushButton:hover { background-color: #ff5500; }"
            );
            btnSplitView->hide();
            connect(btnSplitView, &QPushButton::clicked, this, &BrowserWindow::showSplitViewMenu);

            urlBar = new DroppableUrlBar(this);
            urlBar->setPlaceholderText("Search with BraveLite Engine or enter address");
            connect(urlBar, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlSubmit);
            connect(urlBar, &DroppableUrlBar::urlDropped, this, [this](const QUrl &u) {
                if (auto *v = activeWebView()) v->load(u);
                else addNewTab(u);
            });

                langCombo = new QComboBox(this);
                langCombo->addItems({"English", "Greek", "Spanish", "French", "German", "Italian", "Japanese", "Chinese", "Russian", "Portuguese"});
                langCombo->setToolTip("Target language for AI Site Translation (translategemma:4b)");
                langCombo->setStyleSheet("QComboBox { background: #25272b; border: 1px solid #34373d; border-radius: 6px; color: #fff; padding: 4px 8px; font-size: 12px; }");

                btnTranslate = new QPushButton("🌐 Translate", this);
                btnTranslate->setToolTip("Translate entire page with local AI (translategemma:4b)");
                btnTranslate->setCursor(Qt::PointingHandCursor);
                btnTranslate->setStyleSheet("QPushButton { background-color: #25272b; border: 1px solid #34373d; border-radius: 6px; padding: 4px 10px; color: #8ab4f8; font-size: 12px; font-weight: bold; } QPushButton:hover { background-color: #35373c; border-color: #8ab4f8; }");
                connect(btnTranslate, &QPushButton::clicked, this, &BrowserWindow::triggerSiteTranslation);

                QPushButton *btnBookmark = new QPushButton(this);
                btnBookmark->setIcon(BrowserIcons::drawIcon("star", QColor("#fadb14")));
                btnBookmark->setIconSize(QSize(20, 20));
                btnBookmark->setToolTip("Bookmark this page");
                btnBookmark->setCursor(Qt::PointingHandCursor);
                btnBookmark->setStyleSheet(navBtnStyle);
                connect(btnBookmark, &QPushButton::clicked, this, &BrowserWindow::promptAddBookmark);

                btnPasswords = new QPushButton(this);
                btnPasswords->setIcon(BrowserIcons::drawIcon("key", QColor("#ff9c6e")));
                btnPasswords->setIconSize(QSize(20, 20));
                btnPasswords->setToolTip("Passwords & Autofill Vault");
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

                QPushButton *btnZoomOut = new QPushButton(this);
                btnZoomOut->setIcon(BrowserIcons::drawIcon("zoom_out"));
                btnZoomOut->setIconSize(QSize(18, 18));
                btnZoomOut->setToolTip("Zoom Out");
                btnZoomOut->setCursor(Qt::PointingHandCursor);
                btnZoomOut->setStyleSheet(navBtnStyle);
                connect(btnZoomOut, &QPushButton::clicked, this, &BrowserWindow::zoomOut);

                QPushButton *btnZoomIn = new QPushButton(this);
                btnZoomIn->setIcon(BrowserIcons::drawIcon("zoom_in"));
                btnZoomIn->setIconSize(QSize(18, 18));
                btnZoomIn->setToolTip("Zoom In");
                btnZoomIn->setCursor(Qt::PointingHandCursor);
                btnZoomIn->setStyleSheet(navBtnStyle);
                connect(btnZoomIn, &QPushButton::clicked, this, &BrowserWindow::zoomIn);

                btnDownload = new QPushButton(this);
                btnDownload->setIcon(BrowserIcons::drawIcon("download", QColor("#52c41a")));
                btnDownload->setIconSize(QSize(20, 20));
                btnDownload->setToolTip("Downloads (Ctrl+J)");
                btnDownload->setCursor(Qt::PointingHandCursor);
                btnDownload->setStyleSheet(navBtnStyle);
                connect(btnDownload, &QPushButton::clicked, this, &BrowserWindow::openDownloadsPopup);

                btnMenu = new QPushButton(this);
                btnMenu->setIcon(BrowserIcons::drawIcon("menu"));
                btnMenu->setIconSize(QSize(20, 20));
                btnMenu->setToolTip("BraveLite Menu");
                btnMenu->setCursor(Qt::PointingHandCursor);
                btnMenu->setStyleSheet(navBtnStyle);
                connect(btnMenu, &QPushButton::clicked, this, &BrowserWindow::openAppMenu);

                navLayout->addWidget(btnBack);
                navLayout->addWidget(btnForward);
                navLayout->addWidget(btnReload);
                navLayout->addWidget(btnSplitView);
                navLayout->addWidget(btnPrivacyGrade);
                navLayout->addWidget(urlBar, 1);
                navLayout->addWidget(langCombo);
                navLayout->addWidget(btnTranslate);
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

    void tabSwitched(int idx) {
        if (idx >= 0 && idx < tabStack->count()) {
            tabStack->setCurrentIndex(idx);
            if (auto *v = activeWebView()) {
                QString urlStr = v->url().toString();
                if (urlStr.startsWith("data:") || v->url().scheme() == "about" || v->url().host() == "search.bravelite.local") {
                    if (v->url().host() != "search.bravelite.local" && v->url().scheme() != "bravelite") urlBar->clear();
                    btnPrivacyGrade->hide();
                } else {
                    urlBar->setText(urlStr);
                    updatePrivacyBadge(v->url());
                }
            }
        }
        updateSplitButtonState();
    }

    void handleUrlSubmit() {
        QString text = urlBar->text().trimmed();
        if (text.isEmpty()) return;

        if (text.startsWith("http://") || text.startsWith("https://")) {
            if (auto *v = activeWebView()) v->load(QUrl(text));
            else addNewTab(QUrl(text));
        } else if (text.contains(".") && !text.contains(" ")) {
            QUrl target("https://" + text);
            if (auto *v = activeWebView()) v->load(target);
            else addNewTab(target);
        } else {
            if (defaultSearchEngine == "BraveLite Engine") {
                runBraveLiteSearch(text, "all");
            } else {
                QUrl target("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(text));
                if (auto *v = activeWebView()) v->load(target);
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
        auto *v = activeWebView();
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

            BookmarkToolButton *btn = new BookmarkToolButton(i, bm.url, bookmarksBar);
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setIcon(displayIcon);
            btn->setIconSize(QSize(16, 16));
            btn->setText(shortTitle);
            btn->setToolTip(bm.title + "\n" + bm.url + "\n(Drag to '+' to open in new tab • Drag to URL bar • Middle-click for new tab)");
            btn->setCursor(Qt::PointingHandCursor);

            connect(btn, &QToolButton::clicked, this, [this, bm]() {
                if (auto *v = activeWebView()) {
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
        gamingModeActive = root.value("gamingModeActive").toBool(false);

        aiEnabled = root.value("aiEnabled").toBool(true);
        aiSearchOverviewEnabled = root.value("aiSearchOverviewEnabled").toBool(true);
        aiHoverSummaryEnabled = root.value("aiHoverSummaryEnabled").toBool(true);
        aiUseGpu = root.value("aiUseGpu").toBool(true);
        ollamaEndpoint = root.value("ollamaEndpoint").toString("http://127.0.0.1:11434");
        ollamaModel = root.value("ollamaModel").toString("qwen2.5:0.5b").toLower();
        translationModel = root.value("translationModel").toString("translategemma:4b").toLower();
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
        root["gamingModeActive"] = gamingModeActive;

        root["aiEnabled"] = aiEnabled;
        root["aiSearchOverviewEnabled"] = aiSearchOverviewEnabled;
        root["aiHoverSummaryEnabled"] = aiHoverSummaryEnabled;
        root["aiUseGpu"] = aiUseGpu;
        root["ollamaEndpoint"] = ollamaEndpoint;
        root["ollamaModel"] = ollamaModel.toLower();
        root["translationModel"] = translationModel.toLower();
        root["aiSystemPrompt"] = aiSystemPrompt;

        QJsonObject kbObj;
        for (auto it = keybindsMap.begin(); it != keybindsMap.end(); ++it) {
            kbObj[it.key()] = it.value();
        }
        root["keybinds"] = kbObj;

        QJsonArray sessionArr;
        if (tabStack) {
            for (int i = 0; i < tabStack->count(); ++i) {
                if (auto *v = CustomWebViewFromWidget(tabStack->widget(i))) {
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
        connect(view, &CustomWebView::openLinkInNewTabRequested, this, [this](const QUrl &u) { addNewTab(u); });
        connect(view, &CustomWebView::openLinkInNewWindowRequested, this, &BrowserWindow::openNewWindowWithUrl);
        connect(view, &CustomWebView::openLinkInIncognitoRequested, this, &BrowserWindow::openIncognitoWithUrl);
        connect(view, &CustomWebView::openLinkInSplitViewRequested, this, &BrowserWindow::openInSplitView);
        connect(view, &CustomWebView::inspectElementRequested, this, [this](CustomWebView *targetView) {
            openInspectElementWindow(targetView);
        });
        connect(view, &CustomWebView::searchBackFallbackRequested, this, [this](const QString &q, const QString &tab) {
            runBraveLiteSearch(q, tab);
        });

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
            connect(p, &CustomWebEnginePage::braveliteSearchRequested, this, [this](const QString &q, const QString &tabType) {
                if (defaultSearchEngine == "BraveLite Engine") {
                    runBraveLiteSearch(q, tabType);
                } else {
                    if (auto *v = activeWebView()) {
                        v->load(QUrl("https://duckduckgo.com/?q=" + QUrl::toPercentEncoding(q)));
                    }
                }
            });

            connect(p, &CustomWebEnginePage::aiChatRequested, this, [this, view](const QString &chatPrompt) {
                if (gamingModeActive) return;
                QPointer<CustomWebView> safeView(view);
                QString fullPrompt = chatPrompt + "\nIf you provide code or terminal commands, wrap them in ```code``` blocks.";
                queryOllamaAsync(fullPrompt, [safeView](const QString &reply, bool ok) {
                    if (!safeView || !safeView->page()) return;
                    QJsonObject o;
                    o["reply"] = ok ? reply.trimmed() : ("⚠ " + reply);
                    QString arg = QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
                    safeView->page()->runJavaScript(QString("if (window.__appendAiChat) window.__appendAiChat(%1);").arg(arg));
                });
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
            if (activeWebView() == view) {
                progressBar->setValue(progress == 100 ? 0 : progress);
            }
        });

        connect(view, &QWebEngineView::urlChanged, this, [this, view](const QUrl &u) {
            QString urlStr = u.toString();
            if (activeWebView() == view) {
                if (urlStr.startsWith("data:") || u.scheme() == "about" || u.host() == "search.bravelite.local") {
                    if (u.host() != "search.bravelite.local" && u.scheme() != "bravelite") urlBar->clear();
                    btnPrivacyGrade->hide();
                } else {
                    urlBar->setText(urlStr);
                    updatePrivacyBadge(u);
                }
            }

            if (!m_isIncognito && !urlStr.isEmpty() && !urlStr.startsWith("data:") && u.scheme() != "about") {
                if (u.scheme() == "bravelite" && u.host() == "search") {
                    QString q = QUrlQuery(u).queryItemValue("q");
                    if (!q.isEmpty()) {
                        historyList.append(QString("BraveLite Engine: %1 (%2)").arg(q, urlStr));
                        savePersistentData();
                    }
                } else {
                    historyList.append(urlStr);
                    savePersistentData();
                }
            }
        });

        connect(view, &QWebEngineView::loadFinished, this, [this, view](bool ok) {
            if (ok && !m_isIncognito) {
                if (view->url().host() == "search.bravelite.local") {
                    if (!view->pendingWebResultsJson.isEmpty()) {
                        QString js = QString("if (window.__setWebResults) window.__setWebResults(%1);").arg(view->pendingWebResultsJson);
                        view->page()->runJavaScript(js);
                    }
                    if (!view->pendingImageResultsJson.isEmpty()) {
                        QString js = QString("if (window.__setImageResults) window.__setImageResults(%1);").arg(view->pendingImageResultsJson);
                        view->page()->runJavaScript(js);
                    }
                    if (!view->pendingAiOverviewJsArg.isEmpty()) {
                        QString js = QString("if (window.__setAiOverview) window.__setAiOverview(%1);").arg(view->pendingAiOverviewJsArg);
                        view->page()->runJavaScript(js);
                    }
                }

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
                    tt.style.cssText = 'position:fixed; z-index:2147483647; max-width:390px; background:linear-gradient(180deg,#1f242d,#161920); color:#f0f2f5; border:1px solid #ff5500; border-radius:12px; padding:12px 15px; font-family:sans-serif; font-size:12.5px; line-height:1.5; box-shadow:0 12px 32px rgba(0,0,0,0.8); pointer-events:none; display:none;';
                    document.body.appendChild(tt);
                        }
                        return tt;
                    }

                    window.__updateBraveLiteAiTooltipObj = function(obj) {
                        if (!obj || obj.url !== currentHoverUrl) return;
                        const tt = ensureTooltip();
                        tt.innerHTML = '<div style=\"display:flex;align-items:center;gap:6px;margin-bottom:5px;font-weight:bold;color:#ff6a22;font-size:12px;\">🦁 BraveLite Link Preview • ' + (obj.domain || '') + '</div>' +
                    '<div style=\"font-weight:600;color:#8ab4f8;margin-bottom:4px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;\">' + (obj.title || '') + '</div>' +
                    '<div style=\"color:#e0e2e8;\">' + (obj.summary || '') + '</div>';
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
                        const x = Math.min(window.innerWidth - 405, Math.max(12, rect.left));
                        const y = (rect.bottom + 125 < window.innerHeight) ? (rect.bottom + 8) : Math.max(12, rect.top - 105);

                        clearTimeout(hoverTimer);
                        hoverTimer = setTimeout(function() {
                            if (activeAnchor !== a) return;
                            const tt = ensureTooltip();
                            tt.style.left = x + 'px';
                        tt.style.top = y + 'px';
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
                                if (v && v.__sbSegments && Array.isArray(v.__sbSegments)) {
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

                QString extDir = getConfigDir() + "/Extensions";
                QDir dir(extDir);
                for (const QString &scriptFile : dir.entryList({"*.user.js", "*.js"}, QDir::Files)) {
                    QFile f(extDir + "/" + scriptFile);
                    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        view->page()->runJavaScript(QString::fromUtf8(f.readAll()));
                    }
                }
            }
        });
    }
};

QList<BrowserWindow*> BrowserWindow::s_openWindows;

// ---------------- 10. POPUP BLOCKER & NEW TABS ----------------
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

    updateSplitButtonState();
    return view;
}

int main(int argc, char *argv[]) {
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--enable-smooth-scrolling --disable-background-networking --enable-LowPriorityIframes "
            "--disable-renderer-backgrounding=false --autoplay-policy=no-user-gesture-required");

    CrashLogger::installCrashHandlers();

    QApplication app(argc, argv);
    app.setApplicationName("BraveLite");
    app.setApplicationVersion("1.5");
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

    const QString ipcServerName = "BraveLiteSingleInstanceServer_v150";
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
