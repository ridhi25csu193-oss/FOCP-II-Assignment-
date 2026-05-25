#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <QObject>
#include <QString>
#include "datamodels.h"

class ReportGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ReportGenerator(QObject *parent = nullptr);

    bool exportToHtml(const ReportData &data, const QString &filePath);
    bool exportToCsv(const QVector<CountryEnergyData> &countries, const QString &filePath);
    bool exportToJson(const ReportData &data, const QString &filePath);

    QString generateHtmlContent(const ReportData &data) const;

signals:
    void exportCompleted(const QString &filePath);
    void exportFailed(const QString &error);

private:
    QString generateMetricsSection(const GlobalEnergyMetrics &metrics) const;
    QString generateCountryTable(const QVector<CountryEnergyData> &countries) const;
    QString generateInsightsSection(const QVector<AIInsight> &insights) const;
    QString generateAlertsSection(const QVector<EnergyAlert> &alerts) const;
    QString riskBadge(RiskLevel level) const;
};

#endif // REPORTGENERATOR_H
