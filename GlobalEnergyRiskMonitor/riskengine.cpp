#include "riskengine.h"
#include <QtMath>
#include <QRandomGenerator>
#include <algorithm>
#include <numeric>

RiskEngine::RiskEngine(QObject *parent)
    : QObject(parent)
{
}

double RiskEngine::calculateCountryRisk(const CountryEnergyData &data) const
{
    double supplyDeficit = 0.0;
    if (data.totalDemandGW > 0) {
        supplyDeficit = (data.totalDemandGW - data.totalSupplyGW) / data.totalDemandGW * 100.0;
    }
    double renewableGap = 100.0 - data.renewablePercentage;
    return weightedRiskScore(data.oilDependencyPercent, data.carbonIntensityGCO2,
                             supplyDeficit, renewableGap);
}

double RiskEngine::calculateGlobalRisk(const GlobalEnergyMetrics &metrics,
                                       const QVector<CountryEnergyData> &countries) const
{
    double avgCountryRisk = 0.0;
    if (!countries.isEmpty()) {
        for (const auto &c : countries) {
            avgCountryRisk += c.riskScore;
        }
        avgCountryRisk /= countries.size();
    }

    double supplyRisk = 0.0;
    if (metrics.globalDemandTWh > 0) {
        double deficit = (metrics.globalDemandTWh - metrics.globalSupplyTWh) /
                         metrics.globalDemandTWh * 100.0;
        supplyRisk = qMax(0.0, deficit) * 3.0;
    }

    double oilPriceRisk = qBound(0.0, (metrics.oilPriceUSD - 60.0) / 40.0 * 30.0, 30.0);
    double volatilityRisk = metrics.oilVolatilityIndex * 0.3;
    double carbonRisk = qBound(0.0, (metrics.globalCarbonEmissionsMT - 30000) / 10000.0 * 20.0, 20.0);

    double global = avgCountryRisk * 0.3 + supplyRisk * 0.2 + oilPriceRisk * 0.2
                    + volatilityRisk * 0.15 + carbonRisk * 0.15;
    return qBound(0.0, global, 100.0);
}

RiskLevel RiskEngine::classifyRisk(double score) const
{
    if (score < 25) return RiskLevel::Low;
    if (score < 50) return RiskLevel::Moderate;
    if (score < 75) return RiskLevel::High;
    return RiskLevel::Critical;
}

QString RiskEngine::analyzeTrend(const QVector<TimeSeriesPoint> &data) const
{
    double slope = calculateTrendSlope(data);
    if (qAbs(slope) < 0.01) return "Stable";
    if (slope > 0.05) return "Rising Sharply";
    if (slope > 0) return "Rising";
    if (slope < -0.05) return "Falling Sharply";
    return "Falling";
}

double RiskEngine::calculateTrendSlope(const QVector<TimeSeriesPoint> &data) const
{
    if (data.size() < 2) return 0.0;

    int n = data.size();
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (int i = 0; i < n; ++i) {
        sumX += i;
        sumY += data[i].value;
        sumXY += i * data[i].value;
        sumX2 += i * i;
    }

    double denom = n * sumX2 - sumX * sumX;
    if (qAbs(denom) < 1e-10) return 0.0;

    double slope = (n * sumXY - sumX * sumY) / denom;
    double avgY = sumY / n;
    if (qAbs(avgY) < 1e-10) return 0.0;

    return slope / avgY;
}

double RiskEngine::calculateVolatility(const QVector<TimeSeriesPoint> &data) const
{
    if (data.size() < 2) return 0.0;

    double mean = 0;
    for (const auto &pt : data) mean += pt.value;
    mean /= data.size();

    double variance = 0;
    for (const auto &pt : data) {
        double diff = pt.value - mean;
        variance += diff * diff;
    }
    variance /= data.size();

    return qSqrt(variance) / qAbs(mean) * 100.0;
}

ForecastData RiskEngine::generateForecast(const QString &metric,
                                           const QVector<TimeSeriesPoint> &historical,
                                           int forecastHours) const
{
    ForecastData forecast;
    forecast.metric = metric;
    forecast.region = "Global";
    forecast.historicalData = historical;
    forecast.generatedAt = QDateTime::currentDateTime();

    if (historical.size() < 2) {
        forecast.confidenceLevel = 0.0;
        forecast.trendDirection = "Insufficient Data";
        return forecast;
    }

    double slope = calculateTrendSlope(historical);
    double volatility = calculateVolatility(historical);
    double lastValue = historical.last().value;
    QDateTime lastTime = historical.last().timestamp;

    forecast.trendDirection = analyzeTrend(historical);
    forecast.confidenceLevel = qBound(0.0, 100.0 - volatility * 2.0, 95.0);

    for (int h = 1; h <= forecastHours; ++h) {
        TimeSeriesPoint pt;
        pt.timestamp = lastTime.addSecs(h * 3600);

        double trend = lastValue * slope * h;
        double noise = lastValue * volatility / 100.0 *
                       (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * 0.3;
        pt.value = lastValue + trend + noise;
        pt.label = metric + " (forecast)";

        forecast.predictedData.append(pt);
    }

    return forecast;
}

QVector<ForecastData> RiskEngine::generateAllForecasts(
    const QMap<QString, QVector<TimeSeriesPoint>> &historicalData) const
{
    QVector<ForecastData> forecasts;
    for (auto it = historicalData.constBegin(); it != historicalData.constEnd(); ++it) {
        forecasts.append(generateForecast(it.key(), it.value(), 24));
    }
    return forecasts;
}

QVector<AIInsight> RiskEngine::generateInsights(
    const GlobalEnergyMetrics &metrics,
    const QVector<CountryEnergyData> &countries,
    const QVector<WeatherImpact> &weather) const
{
    QVector<AIInsight> insights;

    // Oil price analysis
    if (metrics.oilPriceUSD > 85.0) {
        insights.append(createInsight(
            "High Oil Price Alert",
            QString("Oil price at $%1/barrel exceeds the $85 threshold. "
                    "This may indicate supply constraints or increased demand.")
                .arg(metrics.oilPriceUSD, 0, 'f', 2),
            "Consider reviewing oil-dependent regions and diversifying energy sources.",
            0.88, "Market", RiskLevel::High));
    }

    // Oil volatility
    if (metrics.oilVolatilityIndex > 30.0) {
        insights.append(createInsight(
            "Oil Market Volatility Elevated",
            QString("Volatility index at %1 suggests market instability. "
                    "This could lead to unpredictable price swings.")
                .arg(metrics.oilVolatilityIndex, 0, 'f', 1),
            "Monitor geopolitical developments and hedging strategies.",
            0.82, "Market", RiskLevel::High));
    }

    // Supply-demand imbalance
    if (metrics.globalDemandTWh > metrics.globalSupplyTWh) {
        double deficit = (metrics.globalDemandTWh - metrics.globalSupplyTWh) /
                         metrics.globalDemandTWh * 100.0;
        insights.append(createInsight(
            "Global Supply Deficit Detected",
            QString("Global energy demand exceeds supply by %1%. "
                    "This deficit may lead to price increases and rationing.")
                .arg(deficit, 0, 'f', 1),
            "Increase renewable capacity and optimize grid distribution.",
            0.91, "Supply", RiskLevel::Critical));
    }

    // Renewable growth
    if (metrics.renewableGrowthRate > 8.0) {
        insights.append(createInsight(
            "Strong Renewable Growth Detected",
            QString("Renewable energy growing at %1% annually, outpacing projections. "
                    "This trend supports long-term energy security.")
                .arg(metrics.renewableGrowthRate, 0, 'f', 1),
            "Maintain investment momentum and address grid integration challenges.",
            0.85, "Renewable", RiskLevel::Low));
    } else if (metrics.globalRenewablePercent < 30.0) {
        insights.append(createInsight(
            "Renewable Adoption Below Target",
            QString("Global renewable share at %1% remains below the 30% target. "
                    "Accelerated deployment needed to meet climate goals.")
                .arg(metrics.globalRenewablePercent, 0, 'f', 1),
            "Scale up solar and wind installations, improve grid storage.",
            0.90, "Renewable", RiskLevel::Moderate));
    }

    // Carbon emissions
    if (metrics.globalCarbonEmissionsMT > 35000) {
        insights.append(createInsight(
            "Carbon Emissions Above Sustainable Levels",
            QString("Global emissions at %1 MT CO₂ exceed the 35,000 MT sustainability "
                    "threshold. Immediate action required.")
                .arg(metrics.globalCarbonEmissionsMT, 0, 'f', 0),
            "Implement carbon pricing and accelerate coal phase-out.",
            0.93, "Environment", RiskLevel::High));
    }

    // Country-specific insights
    for (const auto &c : countries) {
        if (c.oilDependencyPercent > 70) {
            insights.append(createInsight(
                QString("High Oil Dependency: %1").arg(c.countryName),
                QString("%1 has %2% oil dependency, creating significant vulnerability "
                        "to oil price shocks and supply disruptions.")
                    .arg(c.countryName).arg(c.oilDependencyPercent, 0, 'f', 1),
                "Diversify energy mix with renewable investments.",
                0.87, "Country", RiskLevel::High));
        }

        if (c.riskScore > 60) {
            insights.append(createInsight(
                QString("Elevated Risk: %1").arg(c.countryName),
                QString("%1 risk score at %2/100. Key factors: %3% fossil dependency, "
                        "%4 gCO₂/kWh carbon intensity.")
                    .arg(c.countryName)
                    .arg(c.riskScore, 0, 'f', 1)
                    .arg(c.fossilPercentage, 0, 'f', 1)
                    .arg(c.carbonIntensityGCO2, 0, 'f', 0),
                "Focus on reducing carbon intensity and increasing supply margins.",
                0.84, "Country", RiskLevel::High));
        }
    }

    // Weather impacts
    for (const auto &w : weather) {
        if (w.energyImpactPercent < -10) {
            insights.append(createInsight(
                QString("Weather Impact: %1").arg(w.region),
                QString("Adverse weather in %1 reducing energy output by %2%. "
                        "Condition: %3, Wind: %4 m/s, Solar: %5 W/m²")
                    .arg(w.region)
                    .arg(qAbs(w.energyImpactPercent), 0, 'f', 1)
                    .arg(w.weatherCondition)
                    .arg(w.windSpeed, 0, 'f', 1)
                    .arg(w.solarIrradiance, 0, 'f', 0),
                "Activate backup generation and review demand-response programs.",
                0.79, "Weather", RiskLevel::Moderate));
        }
    }

    return insights;
}

QVector<RiskEngine::EmergencyScenario> RiskEngine::getEmergencyScenarios() const
{
    QVector<EmergencyScenario> scenarios;

    scenarios.append({
        "Oil Supply Shock",
        "Major oil producing region experiences 30% supply disruption",
        {{"oilPriceUSD", 1.4}, {"oilVolatilityIndex", 1.8},
         {"globalSupplyTWh", 0.92}},
        RiskLevel::Critical
    });

    scenarios.append({
        "Renewable Grid Failure",
        "Widespread renewable grid instability across Europe",
        {{"globalRenewablePercent", 0.7}, {"globalSupplyTWh", 0.95},
         {"coalPriceUSD", 1.2}},
        RiskLevel::High
    });

    scenarios.append({
        "Extreme Weather Event",
        "Category 5 hurricane affecting Gulf of Mexico energy infrastructure",
        {{"oilPriceUSD", 1.25}, {"naturalGasPriceUSD", 1.5},
         {"globalSupplyTWh", 0.9}},
        RiskLevel::Critical
    });

    scenarios.append({
        "Nuclear Incident",
        "Safety incident at major nuclear facility requiring shutdown",
        {{"globalSupplyTWh", 0.97}, {"coalPriceUSD", 1.15},
         {"naturalGasPriceUSD", 1.3}},
        RiskLevel::High
    });

    scenarios.append({
        "Carbon Tax Implementation",
        "Global carbon tax of $50/ton CO₂ implemented",
        {{"coalPriceUSD", 1.6}, {"naturalGasPriceUSD", 1.2},
         {"globalCarbonEmissionsMT", 0.85}},
        RiskLevel::Moderate
    });

    return scenarios;
}

GlobalEnergyMetrics RiskEngine::simulateEmergency(const GlobalEnergyMetrics &baseline,
                                                    const EmergencyScenario &scenario) const
{
    GlobalEnergyMetrics result = baseline;

    for (auto it = scenario.adjustments.constBegin();
         it != scenario.adjustments.constEnd(); ++it) {
        const QString &key = it.key();
        double multiplier = it.value();

        if (key == "oilPriceUSD") result.oilPriceUSD *= multiplier;
        else if (key == "naturalGasPriceUSD") result.naturalGasPriceUSD *= multiplier;
        else if (key == "coalPriceUSD") result.coalPriceUSD *= multiplier;
        else if (key == "globalDemandTWh") result.globalDemandTWh *= multiplier;
        else if (key == "globalSupplyTWh") result.globalSupplyTWh *= multiplier;
        else if (key == "globalRenewablePercent") result.globalRenewablePercent *= multiplier;
        else if (key == "globalCarbonEmissionsMT") result.globalCarbonEmissionsMT *= multiplier;
        else if (key == "oilVolatilityIndex") result.oilVolatilityIndex *= multiplier;
    }

    result.lastUpdated = QDateTime::currentDateTime();
    return result;
}

double RiskEngine::weightedRiskScore(double oilDependency, double carbonIntensity,
                                      double supplyDeficit, double renewableGap) const
{
    return qBound(0.0,
        oilDependency * 0.30
        + carbonIntensity / 10.0 * 0.25
        + qMax(0.0, supplyDeficit) * 2.0 * 0.25
        + renewableGap * 0.20, 100.0);
}

AIInsight RiskEngine::createInsight(const QString &title, const QString &desc,
                                     const QString &recommendation, double confidence,
                                     const QString &category, RiskLevel severity) const
{
    AIInsight insight;
    insight.title = title;
    insight.description = desc;
    insight.recommendation = recommendation;
    insight.confidence = confidence;
    insight.category = category;
    insight.severity = severity;
    insight.generatedAt = QDateTime::currentDateTime();
    return insight;
}
