#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QScrollArea>
#include <QTableWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTabWidget>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

#include "datamodels.h"

QT_CHARTS_USE_NAMESPACE

class DataManager;
class RiskEngine;
class AIAssistant;
class ReportGenerator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onDataUpdated();
    void onNavigationClicked(int index);
    void onChatSend();
    void onExportReport();
    void onEmergencySimulation();
    void onRefreshClicked();
    void toggleFullscreen();

private:
    // Setup methods
    void setupUI();
    void setupSidebar();
    void setupDashboardPage();
    void setupCountriesPage();
    void setupChartsPage();
    void setupAIInsightsPage();
    void setupChatPage();
    void setupAlertsPage();
    void setupSettingsPage();
    void setupStatusBar();
    void setupConnections();
    void setupAnimations();

    // Update methods
    void updateDashboard();
    void updateMetricCards();
    void updateRiskGauge();
    void updateCountryTable();
    void updateCharts();
    void updatePriceChart();
    void updateRenewableChart();
    void updateEnergyMixChart();
    void updateCarbonChart();
    void updateAlertsList();
    void updateAIInsights();
    void updateStatusBar();
    void updateWeatherPanel();

    // UI creation helpers
    QFrame *createMetricCard(const QString &title, const QString &value,
                             const QString &subtitle, const QColor &accentColor);
    QFrame *createRiskCard(const CountryEnergyData &country);
    QFrame *createAlertCard(const EnergyAlert &alert);
    QFrame *createInsightCard(const AIInsight &insight);
    QFrame *createChatBubble(const QString &text, bool isUser);
    QWidget *createSettingsSection(const QString &title, QLayout *content);
    void animateWidget(QWidget *widget, int delay = 0);

    // Core modules
    DataManager *m_dataManager;
    RiskEngine *m_riskEngine;
    AIAssistant *m_aiAssistant;
    ReportGenerator *m_reportGenerator;

    // Navigation
    QFrame *m_sidebar;
    QVector<QPushButton*> m_navButtons;
    QStackedWidget *m_stackedWidget;
    int m_currentPage = 0;

    // Dashboard widgets
    QVector<QFrame*> m_metricCards;
    QLabel *m_globalRiskLabel;
    QLabel *m_globalRiskValue;
    QFrame *m_riskGaugeFrame;
    QGridLayout *m_countryCardsGrid;
    QScrollArea *m_countryCardsScroll;

    // Charts
    QChartView *m_priceChartView;
    QChartView *m_renewableChartView;
    QChartView *m_energyMixChartView;
    QChartView *m_carbonChartView;
    QChart *m_priceChart;
    QChart *m_renewableChart;
    QChart *m_energyMixChart;
    QChart *m_carbonChart;

    // Country table
    QTableWidget *m_countryTable;
    QComboBox *m_regionFilter;

    // AI Chat
    QVBoxLayout *m_chatLayout;
    QScrollArea *m_chatScroll;
    QLineEdit *m_chatInput;

    // Alerts
    QVBoxLayout *m_alertsLayout;
    QScrollArea *m_alertsScroll;

    // AI Insights
    QVBoxLayout *m_insightsLayout;
    QScrollArea *m_insightsScroll;
    QLabel *m_riskSummaryLabel;

    // Weather
    QGridLayout *m_weatherGrid;

    // Status bar
    QLabel *m_statusLabel;
    QLabel *m_lastUpdateLabel;
    QLabel *m_dataPointsLabel;

    // Emergency sim
    QComboBox *m_emergencyCombo;
    QLabel *m_simResultLabel;
};

#endif // MAINWINDOW_H
