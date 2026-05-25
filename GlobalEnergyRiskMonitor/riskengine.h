#ifndef RISKENGINE_H
#define RISKENGINE_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "datamodels.h"

class RiskEngine : public QObject
{
    Q_OBJECT

public:
    explicit RiskEngine(QObject *parent = nullptr);

    // Risk scoring
    double calculateCountryRisk(const CountryEnergyData &data) const;
    double calculateGlobalRisk(const GlobalEnergyMetrics &metrics,
                               const QVector<CountryEnergyData> &countries) const;
    RiskLevel classifyRisk(double score) const;

    // Trend analysis
    QString analyzeTrend(const QVector<TimeSeriesPoint> &data) const;
    double calculateTrendSlope(const QVector<TimeSeriesPoint> &data) const;
    double calculateVolatility(const QVector<TimeSeriesPoint> &data) const;

    // Predictive analysis
    ForecastData generateForecast(const QString &metric,
                                  const QVector<TimeSeriesPoint> &historical,
                                  int forecastHours = 24) const;
    QVector<ForecastData> generateAllForecasts(
        const QMap<QString, QVector<TimeSeriesPoint>> &historicalData) const;

    // AI insights
    QVector<AIInsight> generateInsights(const GlobalEnergyMetrics &metrics,
                                        const QVector<CountryEnergyData> &countries,
                                        const QVector<WeatherImpact> &weather) const;

    // Emergency simulation
    struct EmergencyScenario {
        QString name;
        QString description;
        QMap<QString, double> adjustments;
        RiskLevel expectedImpact;
    };

    QVector<EmergencyScenario> getEmergencyScenarios() const;
    GlobalEnergyMetrics simulateEmergency(const GlobalEnergyMetrics &baseline,
                                           const EmergencyScenario &scenario) const;

signals:
    void riskCalculated(double globalRisk, RiskLevel level);
    void insightsGenerated(const QVector<AIInsight> &insights);
    void forecastGenerated(const QVector<ForecastData> &forecasts);

private:
    double weightedRiskScore(double oilDependency, double carbonIntensity,
                             double supplyDeficit, double renewableGap) const;
    AIInsight createInsight(const QString &title, const QString &desc,
                            const QString &recommendation, double confidence,
                            const QString &category, RiskLevel severity) const;
};

#endif // RISKENGINE_H
