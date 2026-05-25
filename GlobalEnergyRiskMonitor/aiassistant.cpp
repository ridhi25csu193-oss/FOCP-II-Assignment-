#include "aiassistant.h"
#include "datamanager.h"
#include "riskengine.h"
#include <QDateTime>

AIAssistant::AIAssistant(DataManager *dataManager, RiskEngine *riskEngine,
                         QObject *parent)
    : QObject(parent)
    , m_dataManager(dataManager)
    , m_riskEngine(riskEngine)
{
}

QString AIAssistant::processQuery(const QString &query)
{
    ChatMessage userMsg;
    userMsg.content = query;
    userMsg.isUser = true;
    userMsg.timestamp = QDateTime::currentDateTime();
    m_chatHistory.append(userMsg);

    QString lower = query.toLower().trimmed();
    QString response;

    // Country-specific queries
    QString country = findMatchingCountry(lower);
    if (!country.isEmpty()) {
        response = handleCountryQuery(country);
    }
    // Renewable / clean energy queries
    else if (lower.contains("renewable") || lower.contains("solar") ||
             lower.contains("wind") || lower.contains("clean energy") ||
             lower.contains("green energy")) {
        QString region = findMatchingRegion(lower);
        response = handleRenewableQuery(region);
    }
    // Oil queries
    else if (lower.contains("oil") || lower.contains("petroleum") ||
             lower.contains("crude") || lower.contains("opec")) {
        response = handleOilQuery();
    }
    // Carbon / emission queries
    else if (lower.contains("carbon") || lower.contains("emission") ||
             lower.contains("co2") || lower.contains("pollution")) {
        response = handleCarbonQuery();
    }
    // Forecast / prediction queries
    else if (lower.contains("forecast") || lower.contains("predict") ||
             lower.contains("future") || lower.contains("trend")) {
        QString metric = "Oil";
        if (lower.contains("gas")) metric = "NaturalGas";
        else if (lower.contains("coal")) metric = "Coal";
        else if (lower.contains("renewable")) metric = "RenewablePercent";
        else if (lower.contains("carbon") || lower.contains("emission"))
            metric = "CarbonEmissions";
        response = handleForecastQuery(metric);
    }
    // Comparison queries
    else if (lower.contains("compare") || lower.contains("vs") ||
             lower.contains("versus") || lower.contains("difference")) {
        response = handleComparisonQuery(findMatchingRegion(lower));
    }
    else {
        response = handleGeneralQuery(query);
    }

    ChatMessage aiMsg;
    aiMsg.content = response;
    aiMsg.isUser = false;
    aiMsg.timestamp = QDateTime::currentDateTime();
    m_chatHistory.append(aiMsg);

    emit responseReady(response);
    return response;
}

QVector<AIInsight> AIAssistant::generateDashboardInsights()
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto countries = m_dataManager->getCountryData();
    auto weather = m_dataManager->getWeatherImpacts();

    auto insights = m_riskEngine->generateInsights(metrics, countries, weather);
    emit insightsUpdated(insights);
    return insights;
}

QString AIAssistant::generateRiskSummary()
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto countries = m_dataManager->getCountryData();

    double globalRisk = m_riskEngine->calculateGlobalRisk(metrics, countries);
    RiskLevel level = m_riskEngine->classifyRisk(globalRisk);

    QString summary;
    summary += QString("📊 Global Energy Risk Summary\n");
    summary += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    summary += QString("Overall Risk Score: %1/100 (%2)\n\n")
                   .arg(globalRisk, 0, 'f', 1)
                   .arg(riskLevelToString(level));

    summary += QString("Key Metrics:\n");
    summary += QString("  • Oil Price: $%1/barrel\n").arg(metrics.oilPriceUSD, 0, 'f', 2);
    summary += QString("  • Natural Gas: $%1/MMBtu\n").arg(metrics.naturalGasPriceUSD, 0, 'f', 2);
    summary += QString("  • Global Renewable Share: %1%\n").arg(metrics.globalRenewablePercent, 0, 'f', 1);
    summary += QString("  • Carbon Emissions: %1 MT\n").arg(metrics.globalCarbonEmissionsMT, 0, 'f', 0);
    summary += QString("  • Supply/Demand Balance: %1/%2 TWh\n\n")
                   .arg(metrics.globalSupplyTWh, 0, 'f', 0)
                   .arg(metrics.globalDemandTWh, 0, 'f', 0);

    // Top risk countries
    auto sorted = countries;
    std::sort(sorted.begin(), sorted.end(),
              [](const CountryEnergyData &a, const CountryEnergyData &b) {
                  return a.riskScore > b.riskScore;
              });

    summary += "Highest Risk Countries:\n";
    int count = qMin(5, sorted.size());
    for (int i = 0; i < count; ++i) {
        summary += QString("  %1. %2 — Risk: %3 (%4)\n")
                       .arg(i + 1)
                       .arg(sorted[i].countryName)
                       .arg(sorted[i].riskScore, 0, 'f', 1)
                       .arg(riskLevelToString(sorted[i].riskLevel));
    }

    return summary;
}

QVector<QString> AIAssistant::getSuggestedQueries() const
{
    return {
        "What is India's current energy risk?",
        "Show renewable trends in Europe",
        "Why is oil volatility increasing?",
        "Current carbon emission status?",
        "Compare renewable vs fossil usage",
        "What's the oil price forecast?",
        "Which country has the highest risk?",
        "How does weather impact energy output?",
        "Show global energy supply-demand balance",
        "What are the top energy risks right now?"
    };
}

QString AIAssistant::handleCountryQuery(const QString &country)
{
    auto countries = m_dataManager->getCountryData();

    for (const auto &c : countries) {
        if (c.countryName.toLower() == country.toLower() ||
            c.countryCode.toLower() == country.toLower()) {
            QString res;
            res += QString("🏳 %1 Energy Risk Analysis\n").arg(c.countryName);
            res += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
            res += QString("Risk Score: %1/100 (%2)\n")
                       .arg(c.riskScore, 0, 'f', 1)
                       .arg(riskLevelToString(c.riskLevel));
            res += QString("Region: %1\n\n").arg(c.region);

            res += "Energy Profile:\n";
            res += QString("  • Demand: %1 GW\n").arg(c.totalDemandGW, 0, 'f', 1);
            res += QString("  • Supply: %1 GW\n").arg(c.totalSupplyGW, 0, 'f', 1);
            res += QString("  • Renewable: %1%\n").arg(c.renewablePercentage, 0, 'f', 1);
            res += QString("  • Fossil: %1%\n").arg(c.fossilPercentage, 0, 'f', 1);
            res += QString("  • Oil Dependency: %1%\n").arg(c.oilDependencyPercent, 0, 'f', 1);
            res += QString("  • Carbon Intensity: %1 gCO₂/kWh\n").arg(c.carbonIntensityGCO2, 0, 'f', 0);
            res += QString("  • Electricity Price: $%1/kWh\n\n").arg(c.electricityPricePerkWh, 0, 'f', 3);

            // Analysis
            if (c.riskScore > 60) {
                res += "⚠ Analysis: This country shows elevated risk due to ";
                QStringList factors;
                if (c.oilDependencyPercent > 40) factors << "high oil dependency";
                if (c.carbonIntensityGCO2 > 400) factors << "high carbon intensity";
                if (c.totalDemandGW > c.totalSupplyGW) factors << "supply deficit";
                if (c.renewablePercentage < 20) factors << "low renewable adoption";
                res += factors.join(", ") + ".\n";
                res += "Recommendation: Accelerate renewable deployment and diversify supply.\n";
            } else if (c.riskScore < 30) {
                res += "✓ Analysis: This country has a healthy energy profile with balanced supply "
                       "and growing renewable capacity.\n";
            }

            return res;
        }
    }
    return QString("I don't have data for \"%1\". Available countries include: "
                   "US, China, India, Germany, UK, Japan, Brazil, France, Australia, "
                   "Saudi Arabia, Canada, Russia, South Africa, South Korea, Norway, "
                   "UAE, Mexico, Nigeria, Sweden, Egypt.").arg(country);
}

QString AIAssistant::handleRenewableQuery(const QString &region)
{
    auto countries = m_dataManager->getCountryData();
    auto metrics = m_dataManager->getGlobalMetrics();

    QString res;
    res += QString("🌿 Renewable Energy Analysis");
    if (!region.isEmpty()) res += QString(" — %1").arg(region);
    res += "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    res += QString("Global Renewable Share: %1%\n")
               .arg(metrics.globalRenewablePercent, 0, 'f', 1);
    res += QString("Growth Rate: %1% annually\n\n")
               .arg(metrics.renewableGrowthRate, 0, 'f', 1);

    QVector<const CountryEnergyData*> filtered;
    for (const auto &c : countries) {
        if (region.isEmpty() || c.region.toLower().contains(region.toLower())) {
            filtered.append(&c);
        }
    }

    std::sort(filtered.begin(), filtered.end(),
              [](const CountryEnergyData *a, const CountryEnergyData *b) {
                  return a->renewablePercentage > b->renewablePercentage;
              });

    res += "Renewable Adoption by Country:\n";
    for (const auto *c : filtered) {
        res += QString("  %1: %2% renewable | %3% fossil\n")
                   .arg(c->countryName, -20)
                   .arg(c->renewablePercentage, 5, 'f', 1)
                   .arg(c->fossilPercentage, 5, 'f', 1);
    }

    if (metrics.globalRenewablePercent > 33) {
        res += "\n✓ Renewable energy adoption is trending positively above the global average.\n";
    } else {
        res += "\n⚠ Renewable adoption remains below the 33% target for sustainable energy transition.\n";
    }

    return res;
}

QString AIAssistant::handleOilQuery()
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto prices = m_dataManager->getHistoricalPrices("Oil", 24);

    QString res;
    res += "🛢 Oil Market Analysis\n";
    res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    res += QString("Current Price: $%1/barrel\n").arg(metrics.oilPriceUSD, 0, 'f', 2);
    res += QString("Volatility Index: %1\n").arg(metrics.oilVolatilityIndex, 0, 'f', 1);
    res += QString("Natural Gas: $%1/MMBtu\n").arg(metrics.naturalGasPriceUSD, 0, 'f', 2);
    res += QString("Coal: $%1/ton\n\n").arg(metrics.coalPriceUSD, 0, 'f', 2);

    if (!prices.isEmpty()) {
        QString trend = m_riskEngine->analyzeTrend(prices);
        double volatility = m_riskEngine->calculateVolatility(prices);
        res += QString("24h Trend: %1\n").arg(trend);
        res += QString("24h Volatility: %1%\n\n").arg(volatility, 0, 'f', 2);
    }

    if (metrics.oilPriceUSD > 85) {
        res += "⚠ Oil prices are elevated. Key factors:\n";
        res += "  • Supply constraints from production cuts\n";
        res += "  • Increasing global demand\n";
        res += "  • Geopolitical uncertainties\n";
    } else if (metrics.oilPriceUSD < 70) {
        res += "✓ Oil prices are in a comfortable range.\n";
        res += "  • Adequate global supply\n";
        res += "  • Stable demand patterns\n";
    }

    if (metrics.oilVolatilityIndex > 30) {
        res += "\n⚠ High volatility indicates market uncertainty. Oil-dependent economies "
               "should prepare contingency plans.\n";
    }

    return res;
}

QString AIAssistant::handleCarbonQuery()
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto countries = m_dataManager->getCountryData();

    QString res;
    res += "🌍 Carbon Emission Status\n";
    res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    res += QString("Global CO₂ Emissions: %1 MT/year\n")
               .arg(metrics.globalCarbonEmissionsMT, 0, 'f', 0);
    res += QString("Average Carbon Intensity: %1 gCO₂/kWh\n\n")
               .arg(metrics.avgCarbonIntensity, 0, 'f', 0);

    // Sort by carbon intensity
    auto sorted = countries;
    std::sort(sorted.begin(), sorted.end(),
              [](const CountryEnergyData &a, const CountryEnergyData &b) {
                  return a.carbonIntensityGCO2 > b.carbonIntensityGCO2;
              });

    res += "Carbon Intensity by Country (gCO₂/kWh):\n";
    for (const auto &c : sorted) {
        QString bar;
        int barLen = static_cast<int>(c.carbonIntensityGCO2 / 50.0);
        for (int i = 0; i < qMin(barLen, 15); ++i) bar += "█";
        res += QString("  %1: %2 %3\n")
                   .arg(c.countryName, -20)
                   .arg(c.carbonIntensityGCO2, 6, 'f', 0)
                   .arg(bar);
    }

    if (metrics.globalCarbonEmissionsMT > 35000) {
        res += "\n⚠ Emissions exceed sustainable levels. Urgent action needed:\n";
        res += "  • Accelerate coal phase-out\n";
        res += "  • Implement carbon pricing mechanisms\n";
        res += "  • Increase investment in carbon capture\n";
    }

    return res;
}

QString AIAssistant::handleForecastQuery(const QString &metric)
{
    auto historical = m_dataManager->getHistoricalPrices(metric, 168);
    auto forecast = m_riskEngine->generateForecast(metric, historical, 24);

    QString res;
    res += QString("📈 %1 Forecast\n").arg(metric);
    res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    res += QString("Trend: %1\n").arg(forecast.trendDirection);
    res += QString("Confidence: %1%\n\n").arg(forecast.confidenceLevel, 0, 'f', 1);

    if (!forecast.predictedData.isEmpty()) {
        res += "Predicted Values (next 24h):\n";
        for (int i = 0; i < forecast.predictedData.size(); i += 6) {
            const auto &pt = forecast.predictedData[i];
            res += QString("  %1: %2\n")
                       .arg(pt.timestamp.toString("hh:mm"))
                       .arg(pt.value, 0, 'f', 2);
        }
    }

    res += QString("\nNote: Forecasts are generated using trend analysis with %1% confidence. "
                   "Actual values may vary based on market conditions.\n")
               .arg(forecast.confidenceLevel, 0, 'f', 0);

    return res;
}

QString AIAssistant::handleComparisonQuery(const QString &region)
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto countries = m_dataManager->getCountryData();

    QString res;
    res += "⚡ Renewable vs Fossil Comparison\n";
    res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    res += QString("Global Split: %1% Renewable | %2% Fossil\n\n")
               .arg(metrics.globalRenewablePercent, 0, 'f', 1)
               .arg(100.0 - metrics.globalRenewablePercent, 0, 'f', 1);

    for (const auto &c : countries) {
        if (!region.isEmpty() && !c.region.toLower().contains(region.toLower())) continue;

        int renBar = static_cast<int>(c.renewablePercentage / 5.0);
        int fosBar = static_cast<int>(c.fossilPercentage / 5.0);
        QString renBlocks, fosBlocks;
        for (int i = 0; i < renBar; ++i) renBlocks += "🟩";
        for (int i = 0; i < fosBar; ++i) fosBlocks += "🟥";

        res += QString("  %1:\n    Renewable (%2%): %3\n    Fossil    (%4%): %5\n\n")
                   .arg(c.countryName)
                   .arg(c.renewablePercentage, 0, 'f', 1)
                   .arg(renBlocks)
                   .arg(c.fossilPercentage, 0, 'f', 1)
                   .arg(fosBlocks);
    }

    return res;
}

QString AIAssistant::handleGeneralQuery(const QString &query)
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto countries = m_dataManager->getCountryData();

    QString lower = query.toLower();

    if (lower.contains("risk") || lower.contains("danger") || lower.contains("threat")) {
        return generateRiskSummary();
    }

    if (lower.contains("supply") || lower.contains("demand") || lower.contains("balance")) {
        QString res;
        res += "⚡ Global Supply-Demand Balance\n";
        res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        res += QString("Global Demand: %1 TWh\n").arg(metrics.globalDemandTWh, 0, 'f', 0);
        res += QString("Global Supply: %1 TWh\n").arg(metrics.globalSupplyTWh, 0, 'f', 0);
        double balance = metrics.globalSupplyTWh - metrics.globalDemandTWh;
        res += QString("Balance: %1%2 TWh\n\n")
                   .arg(balance >= 0 ? "+" : "")
                   .arg(balance, 0, 'f', 0);

        if (balance < 0) {
            res += "⚠ Supply deficit detected. This may lead to:\n";
            res += "  • Price increases\n  • Grid instability\n  • Energy rationing\n";
        } else {
            res += "✓ Supply currently meets demand with adequate margins.\n";
        }
        return res;
    }

    if (lower.contains("weather") || lower.contains("climate")) {
        auto weather = m_dataManager->getWeatherImpacts();
        QString res;
        res += "🌤 Weather Impact on Energy\n";
        res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        for (const auto &w : weather) {
            res += QString("%1:\n").arg(w.region);
            res += QString("  Temp: %1°C | Wind: %2 m/s | Solar: %3 W/m²\n")
                       .arg(w.temperature, 0, 'f', 1)
                       .arg(w.windSpeed, 0, 'f', 1)
                       .arg(w.solarIrradiance, 0, 'f', 0);
            res += QString("  Impact: %1% — %2\n\n")
                       .arg(w.energyImpactPercent, 0, 'f', 1)
                       .arg(w.impactDescription);
        }
        return res;
    }

    // Default response with overview
    QString res;
    res += "🌐 Global Energy Overview\n";
    res += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    res += QString("Oil: $%1/barrel | Gas: $%2/MMBtu | Coal: $%3/ton\n")
               .arg(metrics.oilPriceUSD, 0, 'f', 2)
               .arg(metrics.naturalGasPriceUSD, 0, 'f', 2)
               .arg(metrics.coalPriceUSD, 0, 'f', 2);
    res += QString("Renewable Share: %1% (Growth: %2%/yr)\n")
               .arg(metrics.globalRenewablePercent, 0, 'f', 1)
               .arg(metrics.renewableGrowthRate, 0, 'f', 1);
    res += QString("Carbon Emissions: %1 MT CO₂\n\n")
               .arg(metrics.globalCarbonEmissionsMT, 0, 'f', 0);
    res += "Try asking about:\n";
    res += "  • A specific country (e.g., \"India's energy risk\")\n";
    res += "  • Renewable trends\n";
    res += "  • Oil market analysis\n";
    res += "  • Carbon emissions\n";
    res += "  • Price forecasts\n";
    res += "  • Supply-demand balance\n";
    return res;
}

QString AIAssistant::findMatchingCountry(const QString &input) const
{
    QMap<QString, QStringList> countryAliases = {
        {"United States", {"us", "usa", "united states", "america", "u.s."}},
        {"China", {"china", "cn", "chinese"}},
        {"India", {"india", "in", "indian"}},
        {"Germany", {"germany", "de", "german", "deutschland"}},
        {"United Kingdom", {"uk", "gb", "united kingdom", "britain", "british", "england"}},
        {"Japan", {"japan", "jp", "japanese"}},
        {"Brazil", {"brazil", "br", "brazilian"}},
        {"France", {"france", "fr", "french"}},
        {"Australia", {"australia", "au", "australian", "aussie"}},
        {"Saudi Arabia", {"saudi", "sa", "saudi arabia"}},
        {"Canada", {"canada", "ca", "canadian"}},
        {"Russia", {"russia", "ru", "russian"}},
        {"South Africa", {"south africa", "za"}},
        {"South Korea", {"south korea", "korea", "kr", "korean"}},
        {"Norway", {"norway", "no", "norwegian"}},
        {"UAE", {"uae", "ae", "emirates", "dubai", "abu dhabi"}},
        {"Mexico", {"mexico", "mx", "mexican"}},
        {"Nigeria", {"nigeria", "ng", "nigerian"}},
        {"Sweden", {"sweden", "se", "swedish"}},
        {"Egypt", {"egypt", "eg", "egyptian"}},
    };

    for (auto it = countryAliases.constBegin(); it != countryAliases.constEnd(); ++it) {
        const auto aliases = it.value();
        for (const auto &alias : aliases) {
            if (input.contains(alias)) {
                return it.key();
            }
        }
    }
    return {};
}

QString AIAssistant::findMatchingRegion(const QString &input) const
{
    QMap<QString, QStringList> regionAliases = {
        {"Europe", {"europe", "european", "eu"}},
        {"Asia", {"asia", "asian"}},
        {"North America", {"north america", "na"}},
        {"South America", {"south america", "latin america", "latam"}},
        {"Middle East", {"middle east", "mideast"}},
        {"Africa", {"africa", "african"}},
        {"Oceania", {"oceania", "pacific", "australasia"}},
    };

    for (auto it = regionAliases.constBegin(); it != regionAliases.constEnd(); ++it) {
        const auto aliases = it.value();
        for (const auto &alias : aliases) {
            if (input.contains(alias)) {
                return it.key();
            }
        }
    }
    return {};
}
