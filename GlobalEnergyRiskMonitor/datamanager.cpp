#include "datamanager.h"
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_refreshTimer(new QTimer(this))
{
    connect(m_refreshTimer, &QTimer::timeout, this, &DataManager::onRefreshTimer);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &DataManager::onNetworkReply);

    initializeSimulatedData();
}

DataManager::~DataManager()
{
    stopAutoRefresh();
}

void DataManager::startAutoRefresh(int intervalMs)
{
    m_refreshTimer->start(intervalMs);
    qDebug() << "[DataManager] Auto-refresh started, interval:" << intervalMs << "ms";
}

void DataManager::stopAutoRefresh()
{
    m_refreshTimer->stop();
}

void DataManager::refreshAllData()
{
    m_loading = true;
    emit loadingStateChanged(true);

    simulateDataUpdate();

    m_loading = false;
    emit loadingStateChanged(false);
    emit dataUpdated();
    m_updateCount++;
}

GlobalEnergyMetrics DataManager::getGlobalMetrics() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_globalMetrics;
}

QVector<CountryEnergyData> DataManager::getCountryData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_countryData;
}

QVector<WeatherImpact> DataManager::getWeatherImpacts() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_weatherImpacts;
}

QVector<EnergyAlert> DataManager::getAlerts() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_alerts;
}

QVector<EnergyNewsItem> DataManager::getNews() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_news;
}

QVector<TimeSeriesPoint> DataManager::getHistoricalPrices(const QString &commodity, int days) const
{
    QMutexLocker locker(&m_dataMutex);
    auto data = m_historicalPrices.value(commodity);
    if (data.size() > days) {
        return data.mid(data.size() - days);
    }
    return data;
}

void DataManager::onRefreshTimer()
{
    refreshAllData();
}

void DataManager::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        m_lastError = reply->errorString();
        emit errorOccurred(m_lastError);
        qDebug() << "[DataManager] Network error:" << m_lastError;
    }
    reply->deleteLater();
}

void DataManager::initializeSimulatedData()
{
    generateSimulatedGlobalMetrics();
    generateSimulatedCountryData();
    generateSimulatedWeatherData();
    generateSimulatedAlerts();
    generateSimulatedNews();
    generateHistoricalData();

    qDebug() << "[DataManager] Simulated data initialized";
}

void DataManager::simulateDataUpdate()
{
    QMutexLocker locker(&m_dataMutex);

    // Vary global metrics slightly
    m_globalMetrics.oilPriceUSD = applyVariation(m_globalMetrics.oilPriceUSD, 2.0);
    m_globalMetrics.naturalGasPriceUSD = applyVariation(m_globalMetrics.naturalGasPriceUSD, 3.0);
    m_globalMetrics.coalPriceUSD = applyVariation(m_globalMetrics.coalPriceUSD, 1.5);
    m_globalMetrics.globalDemandTWh = applyVariation(m_globalMetrics.globalDemandTWh, 1.0);
    m_globalMetrics.globalSupplyTWh = applyVariation(m_globalMetrics.globalSupplyTWh, 1.0);
    m_globalMetrics.globalRenewablePercent = qBound(0.0,
        applyVariation(m_globalMetrics.globalRenewablePercent, 0.5), 100.0);
    m_globalMetrics.globalCarbonEmissionsMT = applyVariation(
        m_globalMetrics.globalCarbonEmissionsMT, 0.8);
    m_globalMetrics.oilVolatilityIndex = qBound(0.0,
        applyVariation(m_globalMetrics.oilVolatilityIndex, 5.0), 100.0);
    m_globalMetrics.lastUpdated = QDateTime::currentDateTime();

    emit globalMetricsUpdated(m_globalMetrics);

    // Update country data
    for (auto &country : m_countryData) {
        country.totalDemandGW = applyVariation(country.totalDemandGW, 2.0);
        country.totalSupplyGW = applyVariation(country.totalSupplyGW, 2.0);
        country.renewablePercentage = qBound(0.0,
            applyVariation(country.renewablePercentage, 1.0), 100.0);
        country.fossilPercentage = 100.0 - country.renewablePercentage;
        country.carbonIntensityGCO2 = applyVariation(country.carbonIntensityGCO2, 1.5);
        country.electricityPricePerkWh = applyVariation(country.electricityPricePerkWh, 2.0);

        double supplyDeficit = (country.totalDemandGW - country.totalSupplyGW)
                               / country.totalDemandGW * 100.0;
        country.riskScore = qBound(0.0,
            country.oilDependencyPercent * 0.3
            + country.carbonIntensityGCO2 / 10.0
            + qMax(0.0, supplyDeficit) * 2.0
            + (100.0 - country.renewablePercentage) * 0.2, 100.0);
        country.riskLevel = calculateRiskFromScore(country.riskScore);
        country.lastUpdated = QDateTime::currentDateTime();
    }

    emit countryDataUpdated(m_countryData);

    // Update weather
    for (auto &w : m_weatherImpacts) {
        w.temperature = applyVariation(w.temperature, 3.0);
        w.windSpeed = qMax(0.0, applyVariation(w.windSpeed, 10.0));
        w.solarIrradiance = qMax(0.0, applyVariation(w.solarIrradiance, 5.0));
        w.timestamp = QDateTime::currentDateTime();
    }
    emit weatherDataUpdated(m_weatherImpacts);

    // Append to historical prices
    QDateTime now = QDateTime::currentDateTime();
    auto appendPrice = [&](const QString &key, double value) {
        TimeSeriesPoint pt;
        pt.timestamp = now;
        pt.value = value;
        pt.label = key;
        m_historicalPrices[key].append(pt);
        if (m_historicalPrices[key].size() > 500) {
            m_historicalPrices[key].removeFirst();
        }
    };
    appendPrice("Oil", m_globalMetrics.oilPriceUSD);
    appendPrice("NaturalGas", m_globalMetrics.naturalGasPriceUSD);
    appendPrice("Coal", m_globalMetrics.coalPriceUSD);
    appendPrice("RenewablePercent", m_globalMetrics.globalRenewablePercent);
    appendPrice("CarbonEmissions", m_globalMetrics.globalCarbonEmissionsMT);

    // Occasionally add a new alert
    if (QRandomGenerator::global()->bounded(100) < 15) {
        EnergyAlert alert;
        QStringList titles = {
            "Oil Price Surge Detected",
            "Renewable Output Drop",
            "Grid Instability Warning",
            "Carbon Emission Spike",
            "Fuel Supply Disruption",
            "Weather Impact on Solar Output",
            "Wind Farm Output Below Threshold",
            "Nuclear Plant Maintenance Scheduled",
            "Natural Gas Supply Constraint",
            "Electricity Demand Peak Alert"
        };
        QStringList regions = {"Europe", "Asia", "North America", "Middle East",
                               "Africa", "South America", "Oceania"};
        alert.id = QString::number(QRandomGenerator::global()->generate());
        alert.title = titles[QRandomGenerator::global()->bounded(titles.size())];
        alert.description = "Automated alert: " + alert.title + " in " +
                            regions[QRandomGenerator::global()->bounded(regions.size())];
        alert.severity = calculateRiskFromScore(randomInRange(20, 90));
        alert.region = regions[QRandomGenerator::global()->bounded(regions.size())];
        alert.category = "System";
        alert.timestamp = QDateTime::currentDateTime();
        m_alerts.prepend(alert);
        if (m_alerts.size() > 50) m_alerts.removeLast();
        emit alertsUpdated(m_alerts);
    }
}

void DataManager::generateSimulatedGlobalMetrics()
{
    m_globalMetrics.oilPriceUSD = randomInRange(65.0, 95.0);
    m_globalMetrics.naturalGasPriceUSD = randomInRange(2.5, 6.5);
    m_globalMetrics.coalPriceUSD = randomInRange(80.0, 180.0);
    m_globalMetrics.globalDemandTWh = randomInRange(25000, 30000);
    m_globalMetrics.globalSupplyTWh = randomInRange(24500, 30500);
    m_globalMetrics.globalRenewablePercent = randomInRange(28.0, 38.0);
    m_globalMetrics.globalCarbonEmissionsMT = randomInRange(33000, 37000);
    m_globalMetrics.avgCarbonIntensity = randomInRange(400, 550);
    m_globalMetrics.oilVolatilityIndex = randomInRange(15.0, 45.0);
    m_globalMetrics.renewableGrowthRate = randomInRange(3.0, 12.0);
    m_globalMetrics.lastUpdated = QDateTime::currentDateTime();
}

void DataManager::generateSimulatedCountryData()
{
    struct CountryInfo {
        QString code, name, region;
        double demandBase, renewBase, oilDep, carbonBase, priceBase;
    };

    QVector<CountryInfo> countries = {
        {"US",  "United States", "North America", 450, 22, 35, 380, 0.12},
        {"CN",  "China",         "Asia",          980, 30, 18, 580, 0.08},
        {"IN",  "India",         "Asia",          280, 18, 28, 650, 0.06},
        {"DE",  "Germany",       "Europe",        75,  45, 30, 340, 0.30},
        {"GB",  "United Kingdom","Europe",        42,  42, 38, 250, 0.28},
        {"JP",  "Japan",         "Asia",          130, 20, 40, 450, 0.22},
        {"BR",  "Brazil",        "South America", 85,  80, 45, 70,  0.10},
        {"FR",  "France",        "Europe",        68,  25, 30, 55,  0.18},
        {"AU",  "Australia",     "Oceania",       35,  28, 30, 530, 0.25},
        {"SA",  "Saudi Arabia",  "Middle East",   45,  2,  95, 560, 0.05},
        {"CA",  "Canada",        "North America", 60,  67, 20, 130, 0.10},
        {"RU",  "Russia",        "Europe",        120, 18, 20, 420, 0.05},
        {"ZA",  "South Africa",  "Africa",        30,  12, 15, 750, 0.14},
        {"KR",  "South Korea",   "Asia",          70,  8,  42, 450, 0.11},
        {"NO",  "Norway",        "Europe",        18,  98, 5,  15,  0.15},
        {"AE",  "UAE",           "Middle East",   18,  5,  90, 480, 0.08},
        {"MX",  "Mexico",        "North America", 40,  25, 50, 420, 0.09},
        {"NG",  "Nigeria",       "Africa",        8,   15, 60, 380, 0.07},
        {"SE",  "Sweden",        "Europe",        20,  56, 25, 25,  0.20},
        {"EG",  "Egypt",         "Africa",        22,  12, 55, 450, 0.06},
    };

    m_countryData.clear();
    for (const auto &ci : countries) {
        CountryEnergyData d;
        d.countryCode = ci.code;
        d.countryName = ci.name;
        d.region = ci.region;
        d.totalDemandGW = applyVariation(ci.demandBase, 5);
        d.totalSupplyGW = d.totalDemandGW * randomInRange(0.92, 1.08);
        d.renewablePercentage = qBound(0.0, applyVariation(ci.renewBase, 5), 100.0);
        d.fossilPercentage = 100.0 - d.renewablePercentage;
        d.carbonIntensityGCO2 = applyVariation(ci.carbonBase, 5);
        d.oilDependencyPercent = qBound(0.0, applyVariation(ci.oilDep, 5), 100.0);
        d.electricityPricePerkWh = applyVariation(ci.priceBase, 5);

        // Energy mix
        double renewable = d.renewablePercentage;
        double fossil = d.fossilPercentage;
        d.energyMix[EnergySource::Solar] = renewable * randomInRange(0.2, 0.4);
        d.energyMix[EnergySource::Wind] = renewable * randomInRange(0.2, 0.4);
        d.energyMix[EnergySource::Hydro] = renewable * randomInRange(0.1, 0.3);
        d.energyMix[EnergySource::Biomass] = renewable * randomInRange(0.05, 0.1);
        d.energyMix[EnergySource::Coal] = fossil * randomInRange(0.2, 0.4);
        d.energyMix[EnergySource::NaturalGas] = fossil * randomInRange(0.2, 0.4);
        d.energyMix[EnergySource::Oil] = fossil * randomInRange(0.1, 0.3);
        d.energyMix[EnergySource::Nuclear] = fossil * randomInRange(0.05, 0.15);

        double supplyDeficit = (d.totalDemandGW - d.totalSupplyGW) / d.totalDemandGW * 100.0;
        d.riskScore = qBound(0.0,
            d.oilDependencyPercent * 0.3
            + d.carbonIntensityGCO2 / 10.0
            + qMax(0.0, supplyDeficit) * 2.0
            + (100.0 - d.renewablePercentage) * 0.2, 100.0);
        d.riskLevel = calculateRiskFromScore(d.riskScore);
        d.lastUpdated = QDateTime::currentDateTime();

        m_countryData.append(d);
    }
}

void DataManager::generateSimulatedWeatherData()
{
    struct RegionWeather {
        QString region;
        double tempBase, windBase, solarBase;
        QString condition;
    };

    QVector<RegionWeather> regions = {
        {"North America", 18, 12, 650, "Partly Cloudy"},
        {"Europe",        14, 15, 500, "Overcast"},
        {"Asia",          25, 8,  750, "Clear"},
        {"Middle East",   38, 6,  900, "Clear"},
        {"Africa",        30, 10, 850, "Sunny"},
        {"South America", 22, 9,  700, "Humid"},
        {"Oceania",       20, 14, 720, "Windy"},
    };

    m_weatherImpacts.clear();
    for (const auto &rw : regions) {
        WeatherImpact w;
        w.region = rw.region;
        w.temperature = applyVariation(rw.tempBase, 10);
        w.windSpeed = qMax(0.0, applyVariation(rw.windBase, 15));
        w.solarIrradiance = qMax(0.0, applyVariation(rw.solarBase, 10));
        w.humidity = randomInRange(30, 85);
        w.weatherCondition = rw.condition;
        w.energyImpactPercent = randomInRange(-15, 15);
        w.impactDescription = w.energyImpactPercent > 0
            ? "Favorable conditions boosting output"
            : "Adverse conditions reducing output";
        w.timestamp = QDateTime::currentDateTime();
        m_weatherImpacts.append(w);
    }
}

void DataManager::generateSimulatedAlerts()
{
    m_alerts.clear();
    QVector<QPair<QString, RiskLevel>> alertData = {
        {"Oil prices approaching $90/barrel threshold", RiskLevel::High},
        {"European wind output exceeding forecasts by 12%", RiskLevel::Low},
        {"India coal reserves at 60-day supply level", RiskLevel::Moderate},
        {"Solar panel efficiency gains detected in Q2 data", RiskLevel::Low},
        {"Middle East geopolitical tensions affecting supply routes", RiskLevel::Critical},
        {"China grid modernization ahead of schedule", RiskLevel::Low},
        {"Australia bushfire season may impact solar farms", RiskLevel::Moderate},
        {"Natural gas spot price volatility increasing", RiskLevel::High},
        {"Brazil hydroelectric output at seasonal peak", RiskLevel::Low},
        {"Nigeria pipeline disruption reported", RiskLevel::Critical},
    };

    for (int i = 0; i < alertData.size(); ++i) {
        EnergyAlert a;
        a.id = QString::number(i + 1);
        a.title = alertData[i].first;
        a.description = "Automated monitoring alert: " + a.title;
        a.severity = alertData[i].second;
        a.region = "Global";
        a.category = "Monitoring";
        a.timestamp = QDateTime::currentDateTime().addSecs(-i * 600);
        m_alerts.append(a);
    }
}

void DataManager::generateSimulatedNews()
{
    m_news.clear();
    QVector<QPair<QString, QString>> newsData = {
        {"Global Renewable Energy Investment Reaches Record $500B",
         "Investment in clean energy technologies surged to an all-time high, driven by solar and wind expansion."},
        {"OPEC+ Agrees to Gradual Production Increase",
         "The oil cartel has agreed to increase output by 400,000 barrels per day over the next quarter."},
        {"European Grid Achieves 65% Renewable Day",
         "For the first time, the European grid ran on 65% renewable energy for an entire day."},
        {"New Battery Technology Promises 5x Energy Density",
         "Researchers announced a breakthrough in solid-state batteries with significantly higher capacity."},
        {"India Launches World's Largest Solar Park",
         "A 30GW solar installation begins operations in Rajasthan, powering 20 million homes."},
        {"Carbon Capture Plant Opens in Iceland",
         "The world's largest direct air capture facility begins removing CO2 from the atmosphere."},
        {"US Natural Gas Prices Drop on Mild Weather",
         "Warmer-than-expected forecasts push natural gas futures lower across North American markets."},
        {"China Approves 6 New Nuclear Reactors",
         "China continues its nuclear expansion program with approval of six new Generation III+ reactors."},
    };

    for (int i = 0; i < newsData.size(); ++i) {
        EnergyNewsItem n;
        n.title = newsData[i].first;
        n.summary = newsData[i].second;
        n.source = "Energy Monitor";
        n.url = "#";
        n.publishedAt = QDateTime::currentDateTime().addSecs(-i * 3600);
        n.impactLevel = (i % 3 == 0) ? RiskLevel::High :
                        (i % 3 == 1) ? RiskLevel::Moderate : RiskLevel::Low;
        m_news.append(n);
    }
}

void DataManager::generateHistoricalData()
{
    auto generateSeries = [this](double baseValue, double volatility, int points) {
        QVector<TimeSeriesPoint> series;
        QDateTime now = QDateTime::currentDateTime();
        double val = baseValue;
        for (int i = points; i >= 0; --i) {
            TimeSeriesPoint pt;
            pt.timestamp = now.addSecs(-i * 3600);
            val = applyVariation(val, volatility);
            pt.value = val;
            series.append(pt);
        }
        return series;
    };

    m_historicalPrices["Oil"] = generateSeries(78.0, 1.5, 168);
    m_historicalPrices["NaturalGas"] = generateSeries(4.2, 2.0, 168);
    m_historicalPrices["Coal"] = generateSeries(130.0, 1.0, 168);
    m_historicalPrices["RenewablePercent"] = generateSeries(32.0, 0.3, 168);
    m_historicalPrices["CarbonEmissions"] = generateSeries(35000, 0.2, 168);
}

double DataManager::randomInRange(double min, double max) const
{
    return min + QRandomGenerator::global()->generateDouble() * (max - min);
}

double DataManager::applyVariation(double base, double variationPercent) const
{
    double variation = base * variationPercent / 100.0;
    return base + (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * variation;
}

RiskLevel DataManager::calculateRiskFromScore(double score) const
{
    if (score < 25) return RiskLevel::Low;
    if (score < 50) return RiskLevel::Moderate;
    if (score < 75) return RiskLevel::High;
    return RiskLevel::Critical;
}
