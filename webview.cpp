#include "webview.h"
#include <QWebEngineUrlRequestInfo>

AdBlockerInterceptor::AdBlockerInterceptor(QObject *parent)
: QWebEngineUrlRequestInterceptor(parent), m_enabled(true) {

    // Comprehensive tracker, ad server, and analytics domain filter
    blockedPatterns = {
        "doubleclick.net", "googleadservices.com", "googlesyndication.com",
        "adservice.google", "adnxs.com", "scorecardresearch.com",
        "zedo.com", "outbrain.com", "taboola.com", "amazon-adsystem.com",
        "adroll.com", "rubiconproject.com", "openx.net", "pubmatic.com",
        "criteo.com", "casalemedia.com", "moatads.com", "serving-sys.com",
        "smartadserver.com", "adcolony.com", "chartbeat.com", "quantserve.com",
        "adsafeprotected.com", "hotjar.com", "clarity.ms", "advertising.com",
        "revcontent.com", "popads.net", "propellerads.com", "ad-delivery.net",
        "/pagead/", "/ads.js", "analytics.js", "adsense", "/adsserver/"
    };
}

void AdBlockerInterceptor::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool AdBlockerInterceptor::isEnabled() const {
    return m_enabled;
}

void AdBlockerInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    if (!m_enabled) {
        return; // Shield is OFF -> allow
    }

    QString urlString = info.requestUrl().toString().toLower();
    QString host = info.requestUrl().host().toLower();

    for (const QString &pattern : blockedPatterns) {
        if (host.contains(pattern) || urlString.contains(pattern)) {
            info.block(true); // Shield is ON -> block match
            return;
        }
    }
}

WebView::WebView(QWidget *parent) : QWebEngineView(parent) {}
