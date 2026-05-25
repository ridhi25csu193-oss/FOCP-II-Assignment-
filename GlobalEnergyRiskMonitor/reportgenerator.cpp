#include "reportgenerator.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ReportGenerator::ReportGenerator(QObject *parent)
    : QObject(parent)
{
}

bool ReportGenerator::exportToHtml(const ReportData &data, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Cannot open file: " + filePath);
        return false;
    }

    QTextStream out(&file);
    out << generateHtmlContent(data);
    file.close();

    emit exportCompleted(filePath);
    return true;
}

bool ReportGenerator::exportToCsv(const QVector<CountryEnergyData> &countries,
                                   const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Cannot open file: " + filePath);
        return false;
    }

    QTextStream out(&file);
    out << "Country,Code,Region,Demand (GW),Supply (GW),Renewable %,"
           "Fossil %,Carbon Intensity,Oil Dependency %,Risk Score,Risk Level\n";

    for (const auto &c : countries) {
        out << c.countryName << "," << c.countryCode << "," << c.region << ","
            << c.totalDemandGW << "," << c.totalSupplyGW << ","
            << c.renewablePercentage << "," << c.fossilPercentage << ","
            << c.carbonIntensityGCO2 << "," << c.oilDependencyPercent << ","
            << c.riskScore << "," << riskLevelToString(c.riskLevel) << "\n";
    }

    file.close();
    emit exportCompleted(filePath);
    return true;
}

bool ReportGenerator::exportToJson(const ReportData &data, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit exportFailed("Cannot open file: " + filePath);
        return false;
    }

    QJsonObject root;
    root["title"] = data.title;
    root["generatedAt"] = data.generatedAt.toString(Qt::ISODate);

    // Global metrics
    QJsonObject metricsObj;
    metricsObj["oilPriceUSD"] = data.globalMetrics.oilPriceUSD;
    metricsObj["naturalGasPriceUSD"] = data.globalMetrics.naturalGasPriceUSD;
    metricsObj["coalPriceUSD"] = data.globalMetrics.coalPriceUSD;
    metricsObj["globalDemandTWh"] = data.globalMetrics.globalDemandTWh;
    metricsObj["globalSupplyTWh"] = data.globalMetrics.globalSupplyTWh;
    metricsObj["globalRenewablePercent"] = data.globalMetrics.globalRenewablePercent;
    metricsObj["globalCarbonEmissionsMT"] = data.globalMetrics.globalCarbonEmissionsMT;
    root["globalMetrics"] = metricsObj;

    QJsonArray countriesArr;
    for (const auto &c : data.countryData) {
        countriesArr.append(c.toJson());
    }
    root["countries"] = countriesArr;

    QJsonArray insightsArr;
    for (const auto &ins : data.insights) {
        QJsonObject obj;
        obj["title"] = ins.title;
        obj["description"] = ins.description;
        obj["recommendation"] = ins.recommendation;
        obj["confidence"] = ins.confidence;
        obj["category"] = ins.category;
        obj["severity"] = riskLevelToString(ins.severity);
        insightsArr.append(obj);
    }
    root["insights"] = insightsArr;

    QJsonDocument doc(root);
    QTextStream out(&file);
    out << doc.toJson(QJsonDocument::Indented);
    file.close();

    emit exportCompleted(filePath);
    return true;
}

QString ReportGenerator::generateHtmlContent(const ReportData &data) const
{
    QString html;
    html += "<!DOCTYPE html><html><head><meta charset='utf-8'>\n";
    html += "<title>" + data.title + "</title>\n";
    html += "<style>\n";
    html += R"(
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', -apple-system, sans-serif;
            background: #F0F8F2; color: #212B26;
            padding: 40px; line-height: 1.6;
        }
        .header {
            background: linear-gradient(135deg, #38804C, #2E6A40);
            color: white; padding: 30px 40px; border-radius: 16px;
            margin-bottom: 30px;
        }
        .header h1 { font-size: 28px; margin-bottom: 8px; }
        .header p { opacity: 0.85; font-size: 14px; }
        .section { margin-bottom: 30px; }
        .section h2 {
            color: #2E6A40; font-size: 20px;
            margin-bottom: 16px; padding-bottom: 8px;
            border-bottom: 2px solid #BAE0C8;
        }
        .metrics-grid {
            display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 16px; margin-bottom: 20px;
        }
        .metric-card {
            background: white; border: 1px solid #E0EEDF;
            border-radius: 12px; padding: 20px; text-align: center;
        }
        .metric-value { font-size: 28px; font-weight: bold; color: #38804C; }
        .metric-label { font-size: 12px; color: #586E5E; margin-top: 4px; }
        table {
            width: 100%; border-collapse: separate; border-spacing: 0;
            background: white; border-radius: 12px; overflow: hidden;
            border: 1px solid #E0EEDF;
        }
        th {
            background: #F0F8F2; color: #38804C; padding: 12px 16px;
            text-align: left; font-weight: 600; font-size: 12px;
            border-bottom: 2px solid #BAE0C8;
        }
        td { padding: 10px 16px; border-bottom: 1px solid #E8F5EC; font-size: 12px; }
        tr:last-child td { border-bottom: none; }
        tr:hover td { background: #F8FCF9; }
        .badge {
            display: inline-block; padding: 3px 10px; border-radius: 12px;
            font-size: 11px; font-weight: 600;
        }
        .badge-low { background: #E8F5EC; color: #38804C; }
        .badge-moderate { background: #FEF9E7; color: #B8860B; }
        .badge-high { background: #FDEBD0; color: #D35400; }
        .badge-critical { background: #FDF2F2; color: #DC4640; }
        .insight-card {
            background: white; border: 1px solid #E0EEDF;
            border-left: 4px solid #4C9460; border-radius: 8px;
            padding: 16px; margin-bottom: 12px;
        }
        .insight-card h3 { color: #2E6A40; font-size: 14px; margin-bottom: 6px; }
        .insight-card p { font-size: 12px; color: #586E5E; }
        .footer {
            text-align: center; padding: 20px; color: #8A9A8E;
            font-size: 11px; margin-top: 40px;
        }
    )";
    html += "</style></head><body>\n";

    // Header
    html += "<div class='header'>\n";
    html += "<h1>" + data.title + "</h1>\n";
    html += "<p>Generated: " + data.generatedAt.toString("yyyy-MM-dd hh:mm:ss UTC") + "</p>\n";
    html += "</div>\n";

    // Global metrics
    html += generateMetricsSection(data.globalMetrics);

    // Country table
    html += "<div class='section'>\n";
    html += "<h2>Country Energy Risk Data</h2>\n";
    html += generateCountryTable(data.countryData);
    html += "</div>\n";

    // AI Insights
    if (!data.insights.isEmpty()) {
        html += generateInsightsSection(data.insights);
    }

    // Alerts
    if (!data.alerts.isEmpty()) {
        html += generateAlertsSection(data.alerts);
    }

    html += "<div class='footer'>Global Energy Risk Monitor — AI-Powered Analysis Report</div>\n";
    html += "</body></html>";

    return html;
}

QString ReportGenerator::generateMetricsSection(const GlobalEnergyMetrics &metrics) const
{
    QString html;
    html += "<div class='section'>\n<h2>Global Energy Metrics</h2>\n";
    html += "<div class='metrics-grid'>\n";

    auto card = [](const QString &value, const QString &label) {
        return "<div class='metric-card'><div class='metric-value'>" + value +
               "</div><div class='metric-label'>" + label + "</div></div>\n";
    };

    html += card(QString("$%1").arg(metrics.oilPriceUSD, 0, 'f', 2), "Oil Price (USD/barrel)");
    html += card(QString("$%1").arg(metrics.naturalGasPriceUSD, 0, 'f', 2), "Natural Gas (USD/MMBtu)");
    html += card(QString("%1%").arg(metrics.globalRenewablePercent, 0, 'f', 1), "Global Renewable Share");
    html += card(QString("%1 MT").arg(metrics.globalCarbonEmissionsMT, 0, 'f', 0), "Carbon Emissions (MT CO₂)");
    html += card(QString("%1 TWh").arg(metrics.globalDemandTWh, 0, 'f', 0), "Global Energy Demand");
    html += card(QString("%1 TWh").arg(metrics.globalSupplyTWh, 0, 'f', 0), "Global Energy Supply");

    html += "</div>\n</div>\n";
    return html;
}

QString ReportGenerator::generateCountryTable(const QVector<CountryEnergyData> &countries) const
{
    QString html;
    html += "<table><thead><tr>";
    html += "<th>Country</th><th>Region</th><th>Demand (GW)</th><th>Supply (GW)</th>";
    html += "<th>Renewable %</th><th>Carbon Intensity</th><th>Risk Score</th><th>Risk Level</th>";
    html += "</tr></thead><tbody>\n";

    for (const auto &c : countries) {
        html += "<tr>";
        html += "<td><strong>" + c.countryName + "</strong></td>";
        html += "<td>" + c.region + "</td>";
        html += QString("<td>%1</td>").arg(c.totalDemandGW, 0, 'f', 1);
        html += QString("<td>%1</td>").arg(c.totalSupplyGW, 0, 'f', 1);
        html += QString("<td>%1%</td>").arg(c.renewablePercentage, 0, 'f', 1);
        html += QString("<td>%1 gCO₂</td>").arg(c.carbonIntensityGCO2, 0, 'f', 0);
        html += QString("<td>%1</td>").arg(c.riskScore, 0, 'f', 1);
        html += "<td>" + riskBadge(c.riskLevel) + "</td>";
        html += "</tr>\n";
    }

    html += "</tbody></table>\n";
    return html;
}

QString ReportGenerator::generateInsightsSection(const QVector<AIInsight> &insights) const
{
    QString html;
    html += "<div class='section'>\n<h2>AI-Generated Insights</h2>\n";
    for (const auto &ins : insights) {
        html += "<div class='insight-card'>\n";
        html += "<h3>" + ins.title + " " + riskBadge(ins.severity) + "</h3>\n";
        html += "<p>" + ins.description + "</p>\n";
        html += "<p><strong>Recommendation:</strong> " + ins.recommendation + "</p>\n";
        html += QString("<p><em>Confidence: %1%</em></p>\n").arg(ins.confidence * 100.0, 0, 'f', 0);
        html += "</div>\n";
    }
    html += "</div>\n";
    return html;
}

QString ReportGenerator::generateAlertsSection(const QVector<EnergyAlert> &alerts) const
{
    QString html;
    html += "<div class='section'>\n<h2>Active Alerts</h2>\n";
    int count = qMin(10, alerts.size());
    for (int i = 0; i < count; ++i) {
        const auto &a = alerts[i];
        html += "<div class='insight-card'>\n";
        html += "<h3>" + riskBadge(a.severity) + " " + a.title + "</h3>\n";
        html += "<p>" + a.description + "</p>\n";
        html += "<p><em>" + a.timestamp.toString("yyyy-MM-dd hh:mm") + "</em></p>\n";
        html += "</div>\n";
    }
    html += "</div>\n";
    return html;
}

QString ReportGenerator::riskBadge(RiskLevel level) const
{
    QString cls, text;
    text = riskLevelToString(level);
    switch (level) {
        case RiskLevel::Low:      cls = "badge-low"; break;
        case RiskLevel::Moderate:  cls = "badge-moderate"; break;
        case RiskLevel::High:     cls = "badge-high"; break;
        case RiskLevel::Critical: cls = "badge-critical"; break;
    }
    return QString("<span class='badge %1'>%2</span>").arg(cls, text);
}
