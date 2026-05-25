#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>
#include <QCache>
#include "datamodels.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();

    void startAutoRefresh(int intervalMs = 30000);
    void stopAutoRefresh();
    void refreshAllData();

    GlobalEnergyMetrics getGlobalMetrics() const;
    QVector<CountryEnergyData> getCountryData() const;
    QVector<WeatherImpact> getWeatherImpacts() const;
    QVector<EnergyAlert> getAlerts() const;
    QVector<EnergyNewsItem> getNews() const;
    QVector<TimeSeriesPoint> getHistoricalPrices(const QString &commodity, int days = 30) const;

    bool isLoading() const { return m_loading; }
    QString lastError() const { return m_lastError; }

signals:
    void dataUpdated();
    void globalMetricsUpdated(const GlobalEnergyMetrics &metrics);
    void countryDataUpdated(const QVector<CountryEnergyData> &data);
    void weatherDataUpdated(const QVector<WeatherImpact> &impacts);
    void alertsUpdated(const QVector<EnergyAlert> &alerts);
    void newsUpdated(const QVector<EnergyNewsItem> &news);
    void loadingStateChanged(bool loading);
    void errorOccurred(const QString &error);

private slots:
    void onRefreshTimer();
    void onNetworkReply(QNetworkReply *reply);

private:
    void initializeSimulatedData();
    void simulateDataUpdate();
    void generateSimulatedGlobalMetrics();
    void generateSimulatedCountryData();
    void generateSimulatedWeatherData();
    void generateSimulatedAlerts();
    void generateSimulatedNews();
    void generateHistoricalData();

    double randomInRange(double min, double max) const;
    double applyVariation(double base, double variationPercent) const;
    RiskLevel calculateRiskFromScore(double score) const;

    QNetworkAccessManager *m_networkManager;
    QTimer *m_refreshTimer;
    mutable QMutex m_dataMutex;

    GlobalEnergyMetrics m_globalMetrics;
    QVector<CountryEnergyData> m_countryData;
    QVector<WeatherImpact> m_weatherImpacts;
    QVector<EnergyAlert> m_alerts;
    QVector<EnergyNewsItem> m_news;
    QMap<QString, QVector<TimeSeriesPoint>> m_historicalPrices;

    bool m_loading = false;
    QString m_lastError;
    int m_updateCount = 0;
};

#endif // DATAMANAGER_H
