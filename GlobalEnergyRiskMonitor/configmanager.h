#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager &instance();

    // Refresh settings
    int refreshInterval() const;
    void setRefreshInterval(int ms);

    // API keys
    QString apiKey(const QString &service) const;
    void setApiKey(const QString &service, const QString &key);

    // Display settings
    bool fullscreenMode() const;
    void setFullscreenMode(bool enabled);

    bool darkMode() const;
    void setDarkMode(bool enabled);

    int chartAnimationDuration() const;
    void setChartAnimationDuration(int ms);

    // Data settings
    int maxHistoricalDataPoints() const;
    void setMaxHistoricalDataPoints(int points);

    int maxAlerts() const;
    void setMaxAlerts(int count);

    // Window state
    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray &geometry);

    QByteArray windowState() const;
    void setWindowState(const QByteArray &state);

    // Logging
    bool loggingEnabled() const;
    void setLoggingEnabled(bool enabled);

    QString logFilePath() const;

signals:
    void settingsChanged();

private:
    ConfigManager();
    QSettings m_settings;
};

#endif // CONFIGMANAGER_H
