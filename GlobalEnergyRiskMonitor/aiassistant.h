#ifndef AIASSISTANT_H
#define AIASSISTANT_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QRegularExpression>
#include "datamodels.h"

class DataManager;
class RiskEngine;

class AIAssistant : public QObject
{
    Q_OBJECT

public:
    explicit AIAssistant(DataManager *dataManager, RiskEngine *riskEngine,
                         QObject *parent = nullptr);

    QString processQuery(const QString &query);
    QVector<AIInsight> generateDashboardInsights();
    QString generateRiskSummary();
    QVector<QString> getSuggestedQueries() const;

signals:
    void responseReady(const QString &response);
    void insightsUpdated(const QVector<AIInsight> &insights);

private:
    QString handleCountryQuery(const QString &country);
    QString handleRenewableQuery(const QString &region);
    QString handleOilQuery();
    QString handleCarbonQuery();
    QString handleForecastQuery(const QString &metric);
    QString handleComparisonQuery(const QString &region);
    QString handleGeneralQuery(const QString &query);

    QString findMatchingCountry(const QString &input) const;
    QString findMatchingRegion(const QString &input) const;

    DataManager *m_dataManager;
    RiskEngine *m_riskEngine;
    QVector<ChatMessage> m_chatHistory;
};

#endif // AIASSISTANT_H
