#pragma once
#include <QWebEngineView>
#include <QWebEngineUrlRequestInterceptor>
#include <QStringList>

class AdBlockerInterceptor : public QWebEngineUrlRequestInterceptor {
    Q_OBJECT
public:
    explicit AdBlockerInterceptor(QObject *parent = nullptr);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

private:
    bool m_enabled = true;
    QStringList blockedPatterns;
};

class WebView : public QWebEngineView {
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = nullptr);
};
