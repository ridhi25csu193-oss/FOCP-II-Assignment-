#include "configmanager.h"
#include <QStandardPaths>
#include <QDir>

ConfigManager &ConfigManager::instance()
{
    static ConfigManager mgr;
    return mgr;
}

ConfigManager::ConfigManager()
    : m_settings("GlobalEnergyRiskMonitor", "GERM")
{
}

int ConfigManager::refreshInterval() const
{
    return m_settings.value("data/refreshInterval", 30000).toInt();
}

void ConfigManager::setRefreshInterval(int ms)
{
    m_settings.setValue("data/refreshInterval", ms);
    emit settingsChanged();
}

QString ConfigManager::apiKey(const QString &service) const
{
    return m_settings.value("api/" + service).toString();
}

void ConfigManager::setApiKey(const QString &service, const QString &key)
{
    m_settings.setValue("api/" + service, key);
}

bool ConfigManager::fullscreenMode() const
{
    return m_settings.value("display/fullscreen", false).toBool();
}

void ConfigManager::setFullscreenMode(bool enabled)
{
    m_settings.setValue("display/fullscreen", enabled);
    emit settingsChanged();
}

bool ConfigManager::darkMode() const
{
    return m_settings.value("display/darkMode", false).toBool();
}

void ConfigManager::setDarkMode(bool enabled)
{
    m_settings.setValue("display/darkMode", enabled);
    emit settingsChanged();
}

int ConfigManager::chartAnimationDuration() const
{
    return m_settings.value("display/chartAnimation", 800).toInt();
}

void ConfigManager::setChartAnimationDuration(int ms)
{
    m_settings.setValue("display/chartAnimation", ms);
    emit settingsChanged();
}

int ConfigManager::maxHistoricalDataPoints() const
{
    return m_settings.value("data/maxHistorical", 500).toInt();
}

void ConfigManager::setMaxHistoricalDataPoints(int points)
{
    m_settings.setValue("data/maxHistorical", points);
}

int ConfigManager::maxAlerts() const
{
    return m_settings.value("data/maxAlerts", 50).toInt();
}

void ConfigManager::setMaxAlerts(int count)
{
    m_settings.setValue("data/maxAlerts", count);
}

QByteArray ConfigManager::windowGeometry() const
{
    return m_settings.value("window/geometry").toByteArray();
}

void ConfigManager::setWindowGeometry(const QByteArray &geometry)
{
    m_settings.setValue("window/geometry", geometry);
}

QByteArray ConfigManager::windowState() const
{
    return m_settings.value("window/state").toByteArray();
}

void ConfigManager::setWindowState(const QByteArray &state)
{
    m_settings.setValue("window/state", state);
}

bool ConfigManager::loggingEnabled() const
{
    return m_settings.value("system/logging", true).toBool();
}

void ConfigManager::setLoggingEnabled(bool enabled)
{
    m_settings.setValue("system/logging", enabled);
}

QString ConfigManager::logFilePath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/germ.log";
}
