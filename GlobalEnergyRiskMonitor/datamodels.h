#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QMap>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>

// ── Risk severity levels ──
enum class RiskLevel {
    Low,
    Moderate,
    High,
    Critical
};

inline QString riskLevelToString(RiskLevel level) {
    switch (level) {
        case RiskLevel::Low:      return "Low";
        case RiskLevel::Moderate:  return "Moderate";
        case RiskLevel::High:     return "High";
        case RiskLevel::Critical: return "Critical";
    }
    return "Unknown";
}

inline QColor riskLevelColor(RiskLevel level) {
    switch (level) {
        case RiskLevel::Low:      return QColor(76, 175, 80);
        case RiskLevel::Moderate:  return QColor(255, 193, 7);
        case RiskLevel::High:     return QColor(255, 152, 0);
        case RiskLevel::Critical: return QColor(244, 67, 54);
    }
    return QColor(158, 158, 158);
}

// ── Energy source types ──
enum class EnergySource {
    Solar,
    Wind,
    Hydro,
    Nuclear,
    Coal,
    NaturalGas,
    Oil,
    Biomass,
    Geothermal
};

inline QString energySourceToString(EnergySource src) {
    switch (src) {
        case EnergySource::Solar:       return "Solar";
        case EnergySource::Wind:        return "Wind";
        case EnergySource::Hydro:       return "Hydro";
        case EnergySource::Nuclear:     return "Nuclear";
        case EnergySource::Coal:        return "Coal";
        case EnergySource::NaturalGas:  return "Natural Gas";
        case EnergySource::Oil:         return "Oil";
        case EnergySource::Biomass:     return "Biomass";
        case EnergySource::Geothermal:  return "Geothermal";
    }
    return "Unknown";
}

inline bool isRenewable(EnergySource src) {
    return src == EnergySource::Solar || src == EnergySource::Wind ||
           src == EnergySource::Hydro || src == EnergySource::Biomass ||
           src == EnergySource::Geothermal;
}

// ── Country energy data ──
struct CountryEnergyData {
    QString countryCode;
    QString countryName;
    QString region;
    double totalDemandGW = 0.0;
    double totalSupplyGW = 0.0;
    double renewablePercentage = 0.0;
    double fossilPercentage = 0.0;
    double carbonIntensityGCO2 = 0.0;
    double oilDependencyPercent = 0.0;
    double electricityPricePerkWh = 0.0;
    double riskScore = 0.0;
    RiskLevel riskLevel = RiskLevel::Low;
    QMap<EnergySource, double> energyMix;
    QDateTime lastUpdated;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["countryCode"] = countryCode;
        obj["countryName"] = countryName;
        obj["region"] = region;
        obj["totalDemandGW"] = totalDemandGW;
        obj["totalSupplyGW"] = totalSupplyGW;
        obj["renewablePercentage"] = renewablePercentage;
        obj["fossilPercentage"] = fossilPercentage;
        obj["carbonIntensityGCO2"] = carbonIntensityGCO2;
        obj["riskScore"] = riskScore;
        return obj;
    }
};

// ── Global energy metrics ──
struct GlobalEnergyMetrics {
    double oilPriceUSD = 0.0;
    double naturalGasPriceUSD = 0.0;
    double coalPriceUSD = 0.0;
    double globalDemandTWh = 0.0;
    double globalSupplyTWh = 0.0;
    double globalRenewablePercent = 0.0;
    double globalCarbonEmissionsMT = 0.0;
    double avgCarbonIntensity = 0.0;
    double oilVolatilityIndex = 0.0;
    double renewableGrowthRate = 0.0;
    QDateTime lastUpdated;
};

// ── Weather impact data ──
struct WeatherImpact {
    QString region;
    double temperature = 0.0;
    double windSpeed = 0.0;
    double solarIrradiance = 0.0;
    double humidity = 0.0;
    QString weatherCondition;
    double energyImpactPercent = 0.0;
    QString impactDescription;
    QDateTime timestamp;
};

// ── Energy alert ──
struct EnergyAlert {
    QString id;
    QString title;
    QString description;
    RiskLevel severity = RiskLevel::Low;
    QString region;
    QString category;
    QDateTime timestamp;
    bool isRead = false;
};

// ── AI insight ──
struct AIInsight {
    QString title;
    QString description;
    QString recommendation;
    double confidence = 0.0;
    QString category;
    RiskLevel severity = RiskLevel::Low;
    QDateTime generatedAt;
};

// ── Time series data point ──
struct TimeSeriesPoint {
    QDateTime timestamp;
    double value = 0.0;
    QString label;
};

// ── Forecast data ──
struct ForecastData {
    QString metric;
    QString region;
    QVector<TimeSeriesPoint> historicalData;
    QVector<TimeSeriesPoint> predictedData;
    double confidenceLevel = 0.0;
    QString trendDirection;
    QDateTime generatedAt;
};

// ── Chat message for AI assistant ──
struct ChatMessage {
    QString content;
    bool isUser = true;
    QDateTime timestamp;
};

// ── Energy news item ──
struct EnergyNewsItem {
    QString title;
    QString summary;
    QString source;
    QString url;
    QDateTime publishedAt;
    RiskLevel impactLevel = RiskLevel::Low;
};

// ── Report data for export ──
struct ReportData {
    QString title;
    QDateTime generatedAt;
    GlobalEnergyMetrics globalMetrics;
    QVector<CountryEnergyData> countryData;
    QVector<AIInsight> insights;
    QVector<EnergyAlert> alerts;
    QVector<ForecastData> forecasts;
};

#endif // DATAMODELS_H
