#include "mainwindow.h"
#include "datamanager.h"
#include "riskengine.h"
#include "aiassistant.h"
#include "reportgenerator.h"
#include "stylemanager.h"
#include "configmanager.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QSpacerItem>
#include <QSplitter>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_dataManager(new DataManager(this))
    , m_riskEngine(new RiskEngine(this))
    , m_aiAssistant(new AIAssistant(m_dataManager, m_riskEngine, this))
    , m_reportGenerator(new ReportGenerator(this))
{
    setWindowTitle("Global Energy Risk Monitor — AI-Powered Dashboard");
    setMinimumSize(1200, 750);
    resize(1440, 900);

    setStyleSheet(StyleManager::instance().getApplicationStyleSheet());

    setupUI();
    setupConnections();

    // Restore window state
    auto &cfg = ConfigManager::instance();
    if (!cfg.windowGeometry().isEmpty()) {
        restoreGeometry(cfg.windowGeometry());
    }

    // Initial data load
    m_dataManager->refreshAllData();
    m_dataManager->startAutoRefresh(cfg.refreshInterval());

    // Update AI insights
    updateAIInsights();
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event)
{
    auto &cfg = ConfigManager::instance();
    cfg.setWindowGeometry(saveGeometry());
    cfg.setWindowState(saveState());
    m_dataManager->stopAutoRefresh();
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        toggleFullscreen();
    } else if (event->key() == Qt::Key_F5) {
        onRefreshClicked();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    setupSidebar();
    mainLayout->addWidget(m_sidebar);

    // Content area
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setContentsMargins(0, 0, 0, 0);

    setupDashboardPage();
    setupCountriesPage();
    setupChartsPage();
    setupAIInsightsPage();
    setupChatPage();
    setupAlertsPage();
    setupSettingsPage();

    mainLayout->addWidget(m_stackedWidget, 1);

    setupStatusBar();

    // Menu bar
    auto *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Export HTML Report", this, &MainWindow::onExportReport);
    fileMenu->addAction("Export CSV Data", this, [this]() {
        QString path = QFileDialog::getSaveFileName(this, "Export CSV", "", "CSV Files (*.csv)");
        if (!path.isEmpty()) {
            m_reportGenerator->exportToCsv(m_dataManager->getCountryData(), path);
        }
    });
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    auto *viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction("Fullscreen (F11)", this, &MainWindow::toggleFullscreen);
    viewMenu->addAction("Refresh (F5)", this, &MainWindow::onRefreshClicked);

    auto *toolsMenu = menuBar()->addMenu("Tools");
    toolsMenu->addAction("Emergency Simulation", this, &MainWindow::onEmergencySimulation);
}

void MainWindow::setupSidebar()
{
    m_sidebar = new QFrame();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(220);
    m_sidebar->setStyleSheet(StyleManager::instance().getSidebarStyleSheet());

    QVBoxLayout *sideLayout = new QVBoxLayout(m_sidebar);
    sideLayout->setContentsMargins(12, 20, 12, 20);
    sideLayout->setSpacing(4);

    // Logo / title area
    QLabel *logoLabel = new QLabel("🌿");
    logoLabel->setObjectName("sidebarTitle");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setFont(QFont("Segoe UI", 28));
    sideLayout->addWidget(logoLabel);

    QLabel *titleLabel = new QLabel("Energy Monitor");
    titleLabel->setObjectName("sidebarTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("AI-Powered Dashboard");
    subtitleLabel->setObjectName("sidebarSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(subtitleLabel);

    sideLayout->addSpacing(24);

    // Navigation buttons
    QStringList navItems = {
        "  ◉  Dashboard",
        "  ◎  Countries",
        "  ◈  Charts",
        "  ◆  AI Insights",
        "  ◇  AI Chat",
        "  ◈  Alerts",
        "  ◉  Settings"
    };

    for (int i = 0; i < navItems.size(); ++i) {
        auto *btn = new QPushButton(navItems[i]);
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        if (i == 0) btn->setProperty("active", true);
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            onNavigationClicked(i);
        });
        m_navButtons.append(btn);
        sideLayout->addWidget(btn);
    }

    sideLayout->addStretch();

    // Refresh button
    QPushButton *refreshBtn = new QPushButton("  ↻  Refresh Data");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    sideLayout->addWidget(refreshBtn);

    // Fullscreen button
    QPushButton *fsBtn = new QPushButton("  ⛶  Fullscreen");
    fsBtn->setCursor(Qt::PointingHandCursor);
    connect(fsBtn, &QPushButton::clicked, this, &MainWindow::toggleFullscreen);
    sideLayout->addWidget(fsBtn);
}

void MainWindow::setupDashboardPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);

    // Header
    QLabel *header = new QLabel("World Energy Overview");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    // Top metric cards row
    QHBoxLayout *metricsRow = new QHBoxLayout();
    metricsRow->setSpacing(12);

    m_metricCards.clear();
    auto addMetric = [&](const QString &title, const QString &value,
                        const QString &subtitle, const QColor &color) {
        QFrame *card = createMetricCard(title, value, subtitle, color);
        m_metricCards.append(card);
        metricsRow->addWidget(card);
    };

    addMetric("Oil Price", "$78.50", "USD/barrel", StyleManager::warningOrange());
    addMetric("Natural Gas", "$4.20", "USD/MMBtu", StyleManager::accentBlue());
    addMetric("Renewable Share", "32.0%", "Global", StyleManager::successGreen());
    addMetric("Carbon Emissions", "35,000", "MT CO₂/year", StyleManager::dangerRed());
    addMetric("Supply Balance", "+2.1%", "Supply vs Demand", StyleManager::primaryGreen());

    layout->addLayout(metricsRow);

    // Global risk gauge + weather section
    QHBoxLayout *midRow = new QHBoxLayout();
    midRow->setSpacing(16);

    // Risk gauge card
    m_riskGaugeFrame = new QFrame();
    m_riskGaugeFrame->setObjectName("card");
    m_riskGaugeFrame->setStyleSheet(StyleManager::instance().getCardStyleSheet());
    m_riskGaugeFrame->setMinimumWidth(300);

    QVBoxLayout *gaugeLayout = new QVBoxLayout(m_riskGaugeFrame);
    gaugeLayout->setContentsMargins(20, 16, 20, 16);

    QLabel *gaugeTitle = new QLabel("Global Risk Score");
    gaugeTitle->setObjectName("subHeaderLabel");
    gaugeTitle->setFont(StyleManager::subHeaderFont());
    gaugeLayout->addWidget(gaugeTitle);

    m_globalRiskValue = new QLabel("42.5");
    m_globalRiskValue->setObjectName("metricValue");
    m_globalRiskValue->setAlignment(Qt::AlignCenter);
    QFont riskFont = StyleManager::headerFont();
    riskFont.setPointSize(42);
    m_globalRiskValue->setFont(riskFont);
    gaugeLayout->addWidget(m_globalRiskValue);

    m_globalRiskLabel = new QLabel("Moderate Risk");
    m_globalRiskLabel->setObjectName("statusWarning");
    m_globalRiskLabel->setAlignment(Qt::AlignCenter);
    m_globalRiskLabel->setFont(StyleManager::subHeaderFont());
    gaugeLayout->addWidget(m_globalRiskLabel);

    QProgressBar *riskBar = new QProgressBar();
    riskBar->setRange(0, 100);
    riskBar->setValue(42);
    riskBar->setTextVisible(false);
    riskBar->setFixedHeight(8);
    gaugeLayout->addWidget(riskBar);

    midRow->addWidget(m_riskGaugeFrame);

    // Weather impact panel
    QFrame *weatherFrame = new QFrame();
    weatherFrame->setObjectName("card");
    weatherFrame->setStyleSheet(StyleManager::instance().getCardStyleSheet());

    QVBoxLayout *weatherLayout = new QVBoxLayout(weatherFrame);
    weatherLayout->setContentsMargins(20, 16, 20, 16);

    QLabel *weatherTitle = new QLabel("Weather Impact on Energy");
    weatherTitle->setObjectName("subHeaderLabel");
    weatherTitle->setFont(StyleManager::subHeaderFont());
    weatherLayout->addWidget(weatherTitle);

    m_weatherGrid = new QGridLayout();
    m_weatherGrid->setSpacing(8);
    weatherLayout->addLayout(m_weatherGrid);

    midRow->addWidget(weatherFrame, 1);
    layout->addLayout(midRow);

    // Country risk cards
    QLabel *countriesHeader = new QLabel("Country Risk Overview");
    countriesHeader->setObjectName("subHeaderLabel");
    countriesHeader->setFont(StyleManager::subHeaderFont());
    layout->addWidget(countriesHeader);

    m_countryCardsScroll = new QScrollArea();
    m_countryCardsScroll->setWidgetResizable(true);
    m_countryCardsScroll->setFrameShape(QFrame::NoFrame);
    m_countryCardsScroll->setFixedHeight(280);
    m_countryCardsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *cardsWidget = new QWidget();
    m_countryCardsGrid = new QGridLayout(cardsWidget);
    m_countryCardsGrid->setSpacing(12);
    m_countryCardsScroll->setWidget(cardsWidget);

    layout->addWidget(m_countryCardsScroll);

    // Quick charts row
    QHBoxLayout *chartsRow = new QHBoxLayout();
    chartsRow->setSpacing(12);

    // Price trend chart
    QFrame *priceFrame = new QFrame();
    priceFrame->setObjectName("chartContainer");
    priceFrame->setStyleSheet(StyleManager::instance().getChartContainerStyleSheet());
    QVBoxLayout *priceLayout = new QVBoxLayout(priceFrame);
    priceLayout->setContentsMargins(8, 8, 8, 8);

    m_priceChart = new QChart();
    m_priceChart->setTitle("Energy Prices (24h)");
    m_priceChart->setAnimationOptions(QChart::SeriesAnimations);
    m_priceChart->setBackgroundVisible(false);
    m_priceChart->legend()->setAlignment(Qt::AlignBottom);
    m_priceChart->setTitleFont(StyleManager::subHeaderFont());
    m_priceChart->setTitleBrush(StyleManager::softForestGreen());

    m_priceChartView = new QChartView(m_priceChart);
    m_priceChartView->setRenderHint(QPainter::Antialiasing);
    m_priceChartView->setMinimumHeight(250);
    priceLayout->addWidget(m_priceChartView);
    chartsRow->addWidget(priceFrame);

    // Renewable pie chart
    QFrame *renewFrame = new QFrame();
    renewFrame->setObjectName("chartContainer");
    renewFrame->setStyleSheet(StyleManager::instance().getChartContainerStyleSheet());
    QVBoxLayout *renewLayout = new QVBoxLayout(renewFrame);
    renewLayout->setContentsMargins(8, 8, 8, 8);

    m_renewableChart = new QChart();
    m_renewableChart->setTitle("Renewable vs Fossil");
    m_renewableChart->setAnimationOptions(QChart::SeriesAnimations);
    m_renewableChart->setBackgroundVisible(false);
    m_renewableChart->setTitleFont(StyleManager::subHeaderFont());
    m_renewableChart->setTitleBrush(StyleManager::softForestGreen());

    m_renewableChartView = new QChartView(m_renewableChart);
    m_renewableChartView->setRenderHint(QPainter::Antialiasing);
    m_renewableChartView->setMinimumHeight(250);
    renewLayout->addWidget(m_renewableChartView);
    chartsRow->addWidget(renewFrame);

    layout->addLayout(chartsRow);
    layout->addStretch();

    scroll->setWidget(page);
    m_stackedWidget->addWidget(scroll);
}

void MainWindow::setupCountriesPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);

    QLabel *header = new QLabel("Country Energy Data");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    // Filters row
    QHBoxLayout *filterRow = new QHBoxLayout();
    m_regionFilter = new QComboBox();
    m_regionFilter->addItems({"All Regions", "Europe", "Asia", "North America",
                              "South America", "Middle East", "Africa", "Oceania"});
    connect(m_regionFilter, &QComboBox::currentTextChanged,
            this, &MainWindow::updateCountryTable);
    filterRow->addWidget(new QLabel("Region:"));
    filterRow->addWidget(m_regionFilter);
    filterRow->addStretch();

    QPushButton *exportCsvBtn = new QPushButton("Export CSV");
    exportCsvBtn->setObjectName("secondaryBtn");
    connect(exportCsvBtn, &QPushButton::clicked, this, [this]() {
        QString path = QFileDialog::getSaveFileName(this, "Export CSV", "", "CSV (*.csv)");
        if (!path.isEmpty()) {
            m_reportGenerator->exportToCsv(m_dataManager->getCountryData(), path);
        }
    });
    filterRow->addWidget(exportCsvBtn);
    layout->addLayout(filterRow);

    // Table
    m_countryTable = new QTableWidget();
    m_countryTable->setColumnCount(9);
    m_countryTable->setHorizontalHeaderLabels({
        "Country", "Region", "Demand (GW)", "Supply (GW)",
        "Renewable %", "Fossil %", "Carbon Intensity", "Risk Score", "Risk Level"
    });
    m_countryTable->horizontalHeader()->setStretchLastSection(true);
    m_countryTable->verticalHeader()->setVisible(false);
    m_countryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_countryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_countryTable->setAlternatingRowColors(true);
    m_countryTable->setSortingEnabled(true);

    layout->addWidget(m_countryTable, 1);

    m_stackedWidget->addWidget(page);
}

void MainWindow::setupChartsPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);

    QLabel *header = new QLabel("Analytics & Charts");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    // Energy mix bar chart
    QFrame *mixFrame = new QFrame();
    mixFrame->setObjectName("chartContainer");
    mixFrame->setStyleSheet(StyleManager::instance().getChartContainerStyleSheet());
    QVBoxLayout *mixLayout = new QVBoxLayout(mixFrame);
    mixLayout->setContentsMargins(8, 8, 8, 8);

    m_energyMixChart = new QChart();
    m_energyMixChart->setTitle("Energy Mix by Country");
    m_energyMixChart->setAnimationOptions(QChart::SeriesAnimations);
    m_energyMixChart->setBackgroundVisible(false);
    m_energyMixChart->legend()->setAlignment(Qt::AlignBottom);
    m_energyMixChart->setTitleFont(StyleManager::subHeaderFont());
    m_energyMixChart->setTitleBrush(StyleManager::softForestGreen());

    m_energyMixChartView = new QChartView(m_energyMixChart);
    m_energyMixChartView->setRenderHint(QPainter::Antialiasing);
    m_energyMixChartView->setMinimumHeight(350);
    mixLayout->addWidget(m_energyMixChartView);
    layout->addWidget(mixFrame);

    // Carbon chart
    QFrame *carbonFrame = new QFrame();
    carbonFrame->setObjectName("chartContainer");
    carbonFrame->setStyleSheet(StyleManager::instance().getChartContainerStyleSheet());
    QVBoxLayout *carbonLayout = new QVBoxLayout(carbonFrame);
    carbonLayout->setContentsMargins(8, 8, 8, 8);

    m_carbonChart = new QChart();
    m_carbonChart->setTitle("Carbon Emissions Trend");
    m_carbonChart->setAnimationOptions(QChart::SeriesAnimations);
    m_carbonChart->setBackgroundVisible(false);
    m_carbonChart->setTitleFont(StyleManager::subHeaderFont());
    m_carbonChart->setTitleBrush(StyleManager::softForestGreen());

    m_carbonChartView = new QChartView(m_carbonChart);
    m_carbonChartView->setRenderHint(QPainter::Antialiasing);
    m_carbonChartView->setMinimumHeight(300);
    carbonLayout->addWidget(m_carbonChartView);
    layout->addWidget(carbonFrame);

    layout->addStretch();
    scroll->setWidget(page);
    m_stackedWidget->addWidget(scroll);
}

void MainWindow::setupAIInsightsPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);

    QLabel *header = new QLabel("AI-Powered Insights");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    // Risk summary card
    QFrame *summaryCard = new QFrame();
    summaryCard->setObjectName("cardHighlight");
    summaryCard->setStyleSheet(StyleManager::instance().getCardStyleSheet());

    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(20, 16, 20, 16);

    QLabel *summaryTitle = new QLabel("Risk Summary");
    summaryTitle->setObjectName("subHeaderLabel");
    summaryTitle->setFont(StyleManager::subHeaderFont());
    summaryLayout->addWidget(summaryTitle);

    m_riskSummaryLabel = new QLabel();
    m_riskSummaryLabel->setWordWrap(true);
    m_riskSummaryLabel->setFont(StyleManager::monoFont());
    summaryLayout->addWidget(m_riskSummaryLabel);

    layout->addWidget(summaryCard);

    // Emergency simulation
    QFrame *simCard = new QFrame();
    simCard->setObjectName("card");
    simCard->setStyleSheet(StyleManager::instance().getCardStyleSheet());

    QVBoxLayout *simLayout = new QVBoxLayout(simCard);
    simLayout->setContentsMargins(20, 16, 20, 16);

    QLabel *simTitle = new QLabel("Energy Emergency Simulation");
    simTitle->setObjectName("subHeaderLabel");
    simTitle->setFont(StyleManager::subHeaderFont());
    simLayout->addWidget(simTitle);

    QHBoxLayout *simRow = new QHBoxLayout();
    m_emergencyCombo = new QComboBox();
    auto scenarios = m_riskEngine->getEmergencyScenarios();
    for (const auto &s : scenarios) {
        m_emergencyCombo->addItem(s.name);
    }
    simRow->addWidget(m_emergencyCombo, 1);

    QPushButton *simBtn = new QPushButton("Run Simulation");
    connect(simBtn, &QPushButton::clicked, this, &MainWindow::onEmergencySimulation);
    simRow->addWidget(simBtn);
    simLayout->addLayout(simRow);

    m_simResultLabel = new QLabel("Select a scenario and click 'Run Simulation'");
    m_simResultLabel->setWordWrap(true);
    m_simResultLabel->setFont(StyleManager::monoFont());
    simLayout->addWidget(m_simResultLabel);

    layout->addWidget(simCard);

    // Insights list
    QLabel *insightsHeader = new QLabel("Generated Insights");
    insightsHeader->setObjectName("subHeaderLabel");
    insightsHeader->setFont(StyleManager::subHeaderFont());
    layout->addWidget(insightsHeader);

    m_insightsScroll = new QScrollArea();
    m_insightsScroll->setWidgetResizable(true);
    m_insightsScroll->setFrameShape(QFrame::NoFrame);

    QWidget *insightsWidget = new QWidget();
    m_insightsLayout = new QVBoxLayout(insightsWidget);
    m_insightsLayout->setSpacing(8);
    m_insightsLayout->setContentsMargins(0, 0, 0, 0);
    m_insightsScroll->setWidget(insightsWidget);

    layout->addWidget(m_insightsScroll, 1);
    layout->addStretch();

    scroll->setWidget(page);
    m_stackedWidget->addWidget(scroll);
}

void MainWindow::setupChatPage()
{
    QWidget *page = new QWidget();
    page->setStyleSheet(StyleManager::instance().getChatStyleSheet());

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);

    QLabel *header = new QLabel("AI Energy Assistant");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    QLabel *subtitle = new QLabel("Ask about energy risks, trends, forecasts, and more");
    subtitle->setObjectName("metricLabel");
    layout->addWidget(subtitle);

    // Suggested queries
    QHBoxLayout *suggestRow = new QHBoxLayout();
    suggestRow->setSpacing(6);
    auto queries = m_aiAssistant->getSuggestedQueries();
    for (int i = 0; i < qMin(4, queries.size()); ++i) {
        QPushButton *btn = new QPushButton(queries[i]);
        btn->setObjectName("secondaryBtn");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMaximumHeight(32);
        QFont btnFont = StyleManager::smallFont();
        btn->setFont(btnFont);
        connect(btn, &QPushButton::clicked, this, [this, q = queries[i]]() {
            m_chatInput->setText(q);
            onChatSend();
        });
        suggestRow->addWidget(btn);
    }
    suggestRow->addStretch();
    layout->addLayout(suggestRow);

    // Chat messages area
    m_chatScroll = new QScrollArea();
    m_chatScroll->setWidgetResizable(true);
    m_chatScroll->setFrameShape(QFrame::NoFrame);
    m_chatScroll->setObjectName("chatMessages");

    QWidget *chatWidget = new QWidget();
    m_chatLayout = new QVBoxLayout(chatWidget);
    m_chatLayout->setSpacing(8);
    m_chatLayout->setContentsMargins(8, 8, 8, 8);
    m_chatLayout->addStretch();
    m_chatScroll->setWidget(chatWidget);

    layout->addWidget(m_chatScroll, 1);

    // Welcome message
    QFrame *welcomeBubble = createChatBubble(
        "Welcome! I'm your AI Energy Assistant. Ask me anything about global energy risks, "
        "renewable trends, oil prices, carbon emissions, or country-specific data.\n\n"
        "Try: \"What is India's current energy risk?\"", false);
    m_chatLayout->insertWidget(m_chatLayout->count() - 1, welcomeBubble);

    // Input row
    QHBoxLayout *inputRow = new QHBoxLayout();
    inputRow->setSpacing(8);

    m_chatInput = new QLineEdit();
    m_chatInput->setObjectName("chatInput");
    m_chatInput->setPlaceholderText("Ask about energy risks, trends, forecasts...");
    m_chatInput->setMinimumHeight(44);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &MainWindow::onChatSend);
    inputRow->addWidget(m_chatInput, 1);

    QPushButton *sendBtn = new QPushButton("Send");
    sendBtn->setMinimumHeight(44);
    sendBtn->setMinimumWidth(80);
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onChatSend);
    inputRow->addWidget(sendBtn);

    layout->addLayout(inputRow);

    m_stackedWidget->addWidget(page);
}

void MainWindow::setupAlertsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);

    QLabel *header = new QLabel("Energy Alerts & Notifications");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    m_alertsScroll = new QScrollArea();
    m_alertsScroll->setWidgetResizable(true);
    m_alertsScroll->setFrameShape(QFrame::NoFrame);

    QWidget *alertsWidget = new QWidget();
    m_alertsLayout = new QVBoxLayout(alertsWidget);
    m_alertsLayout->setSpacing(6);
    m_alertsLayout->setContentsMargins(0, 0, 0, 0);
    m_alertsScroll->setWidget(alertsWidget);

    layout->addWidget(m_alertsScroll, 1);

    m_stackedWidget->addWidget(page);
}

void MainWindow::setupSettingsPage()
{
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);

    QLabel *header = new QLabel("Settings");
    header->setObjectName("headerLabel");
    header->setFont(StyleManager::headerFont());
    layout->addWidget(header);

    auto &cfg = ConfigManager::instance();

    // Data refresh section
    QVBoxLayout *refreshLayout = new QVBoxLayout();
    QHBoxLayout *refreshRow = new QHBoxLayout();
    QComboBox *refreshCombo = new QComboBox();
    refreshCombo->addItem("10 seconds", 10000);
    refreshCombo->addItem("30 seconds", 30000);
    refreshCombo->addItem("1 minute", 60000);
    refreshCombo->addItem("5 minutes", 300000);

    int idx = refreshCombo->findData(cfg.refreshInterval());
    if (idx >= 0) refreshCombo->setCurrentIndex(idx);

    connect(refreshCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this, refreshCombo](int i) {
                int interval = refreshCombo->itemData(i).toInt();
                ConfigManager::instance().setRefreshInterval(interval);
                m_dataManager->stopAutoRefresh();
                m_dataManager->startAutoRefresh(interval);
            });

    refreshRow->addWidget(new QLabel("Auto-refresh interval:"));
    refreshRow->addWidget(refreshCombo);
    refreshRow->addStretch();
    refreshLayout->addLayout(refreshRow);
    layout->addWidget(createSettingsSection("Data Settings", refreshLayout));

    // Chart animation
    QVBoxLayout *displayLayout = new QVBoxLayout();
    QHBoxLayout *animRow = new QHBoxLayout();
    QComboBox *animCombo = new QComboBox();
    animCombo->addItem("Disabled", 0);
    animCombo->addItem("Fast (400ms)", 400);
    animCombo->addItem("Normal (800ms)", 800);
    animCombo->addItem("Slow (1200ms)", 1200);
    int animIdx = animCombo->findData(cfg.chartAnimationDuration());
    if (animIdx >= 0) animCombo->setCurrentIndex(animIdx);

    connect(animCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [animCombo](int i) {
                ConfigManager::instance().setChartAnimationDuration(
                    animCombo->itemData(i).toInt());
            });

    animRow->addWidget(new QLabel("Chart animation:"));
    animRow->addWidget(animCombo);
    animRow->addStretch();
    displayLayout->addLayout(animRow);
    layout->addWidget(createSettingsSection("Display Settings", displayLayout));

    // About
    QVBoxLayout *aboutLayout = new QVBoxLayout();
    QLabel *aboutLabel = new QLabel(
        "Global Energy Risk Monitor v2.0\n"
        "AI-Powered Real-Time Energy Intelligence Dashboard\n\n"
        "Built with Qt " QT_VERSION_STR " and C++17\n"
        "Features: Real-time data, AI insights, predictive analytics,\n"
        "emergency simulation, and comprehensive reporting.");
    aboutLabel->setWordWrap(true);
    aboutLayout->addWidget(aboutLabel);
    layout->addWidget(createSettingsSection("About", aboutLayout));

    layout->addStretch();
    scroll->setWidget(page);
    m_stackedWidget->addWidget(scroll);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Ready");
    m_lastUpdateLabel = new QLabel("Last update: --");
    m_dataPointsLabel = new QLabel("Data points: 0");

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_dataPointsLabel);
    statusBar()->addPermanentWidget(m_lastUpdateLabel);
}

void MainWindow::setupConnections()
{
    connect(m_dataManager, &DataManager::dataUpdated, this, &MainWindow::onDataUpdated);
    connect(m_dataManager, &DataManager::loadingStateChanged, this, [this](bool loading) {
        m_statusLabel->setText(loading ? "Loading data..." : "Ready");
    });
    connect(m_dataManager, &DataManager::errorOccurred, this, [this](const QString &err) {
        statusBar()->showMessage("Error: " + err, 5000);
    });

    connect(m_reportGenerator, &ReportGenerator::exportCompleted, this, [this](const QString &path) {
        QMessageBox::information(this, "Export Complete",
                                 "Report exported to:\n" + path);
    });
    connect(m_reportGenerator, &ReportGenerator::exportFailed, this, [this](const QString &err) {
        QMessageBox::warning(this, "Export Failed", err);
    });
}

void MainWindow::setupAnimations()
{
    // Animate metric cards on load
    for (int i = 0; i < m_metricCards.size(); ++i) {
        animateWidget(m_metricCards[i], i * 80);
    }
}

void MainWindow::onDataUpdated()
{
    updateDashboard();
    updateCountryTable();
    updateCharts();
    updateAlertsList();
    updateStatusBar();
}

void MainWindow::onNavigationClicked(int index)
{
    m_currentPage = index;
    m_stackedWidget->setCurrentIndex(index);

    for (int i = 0; i < m_navButtons.size(); ++i) {
        m_navButtons[i]->setProperty("active", i == index);
        m_navButtons[i]->style()->unpolish(m_navButtons[i]);
        m_navButtons[i]->style()->polish(m_navButtons[i]);
    }
}

void MainWindow::onChatSend()
{
    QString text = m_chatInput->text().trimmed();
    if (text.isEmpty()) return;

    // Add user bubble
    QFrame *userBubble = createChatBubble(text, true);
    m_chatLayout->insertWidget(m_chatLayout->count() - 1, userBubble);

    m_chatInput->clear();

    // Process and add AI response
    QString response = m_aiAssistant->processQuery(text);
    QFrame *aiBubble = createChatBubble(response, false);
    m_chatLayout->insertWidget(m_chatLayout->count() - 1, aiBubble);

    // Scroll to bottom
    QTimer::singleShot(100, this, [this]() {
        m_chatScroll->verticalScrollBar()->setValue(
            m_chatScroll->verticalScrollBar()->maximum());
    });
}

void MainWindow::onExportReport()
{
    QString path = QFileDialog::getSaveFileName(
        this, "Export Report", "EnergyRiskReport.html", "HTML Files (*.html)");
    if (path.isEmpty()) return;

    ReportData data;
    data.title = "Global Energy Risk Monitor Report";
    data.generatedAt = QDateTime::currentDateTime();
    data.globalMetrics = m_dataManager->getGlobalMetrics();
    data.countryData = m_dataManager->getCountryData();
    data.insights = m_aiAssistant->generateDashboardInsights();
    data.alerts = m_dataManager->getAlerts();

    m_reportGenerator->exportToHtml(data, path);
}

void MainWindow::onEmergencySimulation()
{
    int idx = m_emergencyCombo->currentIndex();
    auto scenarios = m_riskEngine->getEmergencyScenarios();
    if (idx < 0 || idx >= scenarios.size()) return;

    const auto &scenario = scenarios[idx];
    auto baseline = m_dataManager->getGlobalMetrics();
    auto result = m_riskEngine->simulateEmergency(baseline, scenario);

    auto countries = m_dataManager->getCountryData();
    double baselineRisk = m_riskEngine->calculateGlobalRisk(baseline, countries);
    double resultRisk = m_riskEngine->calculateGlobalRisk(result, countries);

    QString report;
    report += QString("━━ Emergency Simulation: %1 ━━\n\n").arg(scenario.name);
    report += scenario.description + "\n\n";
    report += QString("Expected Impact: %1\n\n").arg(riskLevelToString(scenario.expectedImpact));
    report += "Metric Changes:\n";
    report += QString("  Oil Price:      $%1 → $%2\n")
                  .arg(baseline.oilPriceUSD, 0, 'f', 2)
                  .arg(result.oilPriceUSD, 0, 'f', 2);
    report += QString("  Natural Gas:    $%1 → $%2\n")
                  .arg(baseline.naturalGasPriceUSD, 0, 'f', 2)
                  .arg(result.naturalGasPriceUSD, 0, 'f', 2);
    report += QString("  Coal:           $%1 → $%2\n")
                  .arg(baseline.coalPriceUSD, 0, 'f', 2)
                  .arg(result.coalPriceUSD, 0, 'f', 2);
    report += QString("  Supply:         %1 → %2 TWh\n")
                  .arg(baseline.globalSupplyTWh, 0, 'f', 0)
                  .arg(result.globalSupplyTWh, 0, 'f', 0);
    report += QString("  Renewable:      %1% → %2%\n")
                  .arg(baseline.globalRenewablePercent, 0, 'f', 1)
                  .arg(result.globalRenewablePercent, 0, 'f', 1);
    report += QString("  Emissions:      %1 → %2 MT\n\n")
                  .arg(baseline.globalCarbonEmissionsMT, 0, 'f', 0)
                  .arg(result.globalCarbonEmissionsMT, 0, 'f', 0);
    report += QString("Global Risk: %1 → %2 (%3 → %4)\n")
                  .arg(baselineRisk, 0, 'f', 1)
                  .arg(resultRisk, 0, 'f', 1)
                  .arg(riskLevelToString(m_riskEngine->classifyRisk(baselineRisk)))
                  .arg(riskLevelToString(m_riskEngine->classifyRisk(resultRisk)));

    m_simResultLabel->setText(report);
}

void MainWindow::onRefreshClicked()
{
    m_dataManager->refreshAllData();
    updateAIInsights();
    statusBar()->showMessage("Data refreshed", 3000);
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        ConfigManager::instance().setFullscreenMode(false);
    } else {
        showFullScreen();
        ConfigManager::instance().setFullscreenMode(true);
    }
}

void MainWindow::updateDashboard()
{
    updateMetricCards();
    updateRiskGauge();
    updateWeatherPanel();

    // Update country risk cards
    auto countries = m_dataManager->getCountryData();

    // Clear existing cards
    QLayoutItem *child;
    while ((child = m_countryCardsGrid->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Sort by risk score descending
    auto sorted = countries;
    std::sort(sorted.begin(), sorted.end(),
              [](const CountryEnergyData &a, const CountryEnergyData &b) {
                  return a.riskScore > b.riskScore;
              });

    int cols = 5;
    for (int i = 0; i < sorted.size(); ++i) {
        QFrame *card = createRiskCard(sorted[i]);
        m_countryCardsGrid->addWidget(card, i / cols, i % cols);
    }
}

void MainWindow::updateMetricCards()
{
    auto metrics = m_dataManager->getGlobalMetrics();

    auto updateCard = [](QFrame *card, const QString &value, const QString &subtitle) {
        auto labels = card->findChildren<QLabel*>();
        if (labels.size() >= 2) {
            labels[1]->setText(value);
        }
        if (labels.size() >= 3) {
            labels[2]->setText(subtitle);
        }
    };

    if (m_metricCards.size() >= 5) {
        updateCard(m_metricCards[0], QString("$%1").arg(metrics.oilPriceUSD, 0, 'f', 2),
                   metrics.oilPriceUSD > 85 ? "▲ Elevated" : "USD/barrel");
        updateCard(m_metricCards[1], QString("$%1").arg(metrics.naturalGasPriceUSD, 0, 'f', 2),
                   "USD/MMBtu");
        updateCard(m_metricCards[2], QString("%1%").arg(metrics.globalRenewablePercent, 0, 'f', 1),
                   QString("Growth: %1%/yr").arg(metrics.renewableGrowthRate, 0, 'f', 1));
        updateCard(m_metricCards[3], QString("%1").arg(metrics.globalCarbonEmissionsMT, 0, 'f', 0),
                   "MT CO₂/year");

        double balance = (metrics.globalSupplyTWh - metrics.globalDemandTWh) /
                         metrics.globalDemandTWh * 100.0;
        updateCard(m_metricCards[4],
                   QString("%1%2%").arg(balance >= 0 ? "+" : "").arg(balance, 0, 'f', 1),
                   balance >= 0 ? "Surplus" : "Deficit");
    }
}

void MainWindow::updateRiskGauge()
{
    auto metrics = m_dataManager->getGlobalMetrics();
    auto countries = m_dataManager->getCountryData();
    double risk = m_riskEngine->calculateGlobalRisk(metrics, countries);
    RiskLevel level = m_riskEngine->classifyRisk(risk);

    m_globalRiskValue->setText(QString::number(risk, 'f', 1));

    QString levelText = riskLevelToString(level);
    m_globalRiskLabel->setText(levelText + " Risk");

    QColor color = riskLevelColor(level);
    m_globalRiskValue->setStyleSheet(
        QString("font-size: 42px; font-weight: bold; color: %1;").arg(color.name()));
    m_globalRiskLabel->setStyleSheet(
        QString("font-size: 14px; font-weight: 600; color: %1;").arg(color.name()));

    // Update progress bar
    auto *bar = m_riskGaugeFrame->findChild<QProgressBar*>();
    if (bar) {
        bar->setValue(static_cast<int>(risk));
        QString barStyle = QString(
            "QProgressBar { background-color: #E8F5EC; border: none; border-radius: 4px; height: 8px; }"
            "QProgressBar::chunk { background-color: %1; border-radius: 4px; }").arg(color.name());
        bar->setStyleSheet(barStyle);
    }
}

void MainWindow::updateCountryTable()
{
    auto countries = m_dataManager->getCountryData();
    QString filter = m_regionFilter ? m_regionFilter->currentText() : "All Regions";

    m_countryTable->setSortingEnabled(false);
    m_countryTable->setRowCount(0);

    for (const auto &c : countries) {
        if (filter != "All Regions" && c.region != filter) continue;

        int row = m_countryTable->rowCount();
        m_countryTable->insertRow(row);

        m_countryTable->setItem(row, 0, new QTableWidgetItem(c.countryName));
        m_countryTable->setItem(row, 1, new QTableWidgetItem(c.region));
        m_countryTable->setItem(row, 2, new QTableWidgetItem(
            QString::number(c.totalDemandGW, 'f', 1)));
        m_countryTable->setItem(row, 3, new QTableWidgetItem(
            QString::number(c.totalSupplyGW, 'f', 1)));
        m_countryTable->setItem(row, 4, new QTableWidgetItem(
            QString::number(c.renewablePercentage, 'f', 1)));
        m_countryTable->setItem(row, 5, new QTableWidgetItem(
            QString::number(c.fossilPercentage, 'f', 1)));
        m_countryTable->setItem(row, 6, new QTableWidgetItem(
            QString::number(c.carbonIntensityGCO2, 'f', 0)));

        auto *scoreItem = new QTableWidgetItem(QString::number(c.riskScore, 'f', 1));
        scoreItem->setForeground(riskLevelColor(c.riskLevel));
        scoreItem->setFont(QFont("Segoe UI", 11, QFont::Bold));
        m_countryTable->setItem(row, 7, scoreItem);

        auto *levelItem = new QTableWidgetItem(riskLevelToString(c.riskLevel));
        levelItem->setForeground(riskLevelColor(c.riskLevel));
        levelItem->setFont(QFont("Segoe UI", 11, QFont::Bold));
        m_countryTable->setItem(row, 8, levelItem);
    }

    m_countryTable->setSortingEnabled(true);
    m_countryTable->resizeColumnsToContents();
}

void MainWindow::updateCharts()
{
    updatePriceChart();
    updateRenewableChart();
    updateEnergyMixChart();
    updateCarbonChart();
}

void MainWindow::updatePriceChart()
{
    m_priceChart->removeAllSeries();
    for (auto *axis : m_priceChart->axes()) {
        m_priceChart->removeAxis(axis);
    }

    auto oilData = m_dataManager->getHistoricalPrices("Oil", 48);
    auto gasData = m_dataManager->getHistoricalPrices("NaturalGas", 48);

    if (oilData.isEmpty()) return;

    auto *oilSeries = new QLineSeries();
    oilSeries->setName("Oil ($/barrel)");
    oilSeries->setColor(StyleManager::warningOrange());
    QPen oilPen(StyleManager::warningOrange());
    oilPen.setWidth(2);
    oilSeries->setPen(oilPen);

    auto *gasSeries = new QLineSeries();
    gasSeries->setName("Gas ($/MMBtu ×10)");
    gasSeries->setColor(StyleManager::accentBlue());
    QPen gasPen(StyleManager::accentBlue());
    gasPen.setWidth(2);
    gasSeries->setPen(gasPen);

    qreal minTime = std::numeric_limits<qreal>::max();
    qreal maxTime = std::numeric_limits<qreal>::lowest();
    qreal minVal = std::numeric_limits<qreal>::max();
    qreal maxVal = std::numeric_limits<qreal>::lowest();

    for (const auto &pt : oilData) {
        qreal ms = pt.timestamp.toMSecsSinceEpoch();
        oilSeries->append(ms, pt.value);
        minTime = qMin(minTime, ms);
        maxTime = qMax(maxTime, ms);
        minVal = qMin(minVal, pt.value);
        maxVal = qMax(maxVal, pt.value);
    }

    for (const auto &pt : gasData) {
        qreal ms = pt.timestamp.toMSecsSinceEpoch();
        gasSeries->append(ms, pt.value * 10.0);
        minVal = qMin(minVal, pt.value * 10.0);
        maxVal = qMax(maxVal, pt.value * 10.0);
    }

    m_priceChart->addSeries(oilSeries);
    m_priceChart->addSeries(gasSeries);

    auto *axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm");
    axisX->setTitleText("Time");
    axisX->setLabelsColor(StyleManager::secondaryText());
    m_priceChart->addAxis(axisX, Qt::AlignBottom);
    oilSeries->attachAxis(axisX);
    gasSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(minVal * 0.95, maxVal * 1.05);
    axisY->setTitleText("Price");
    axisY->setLabelsColor(StyleManager::secondaryText());
    m_priceChart->addAxis(axisY, Qt::AlignLeft);
    oilSeries->attachAxis(axisY);
    gasSeries->attachAxis(axisY);
}

void MainWindow::updateRenewableChart()
{
    m_renewableChart->removeAllSeries();

    auto metrics = m_dataManager->getGlobalMetrics();

    auto *series = new QPieSeries();
    auto *renewSlice = series->append("Renewable", metrics.globalRenewablePercent);
    renewSlice->setColor(StyleManager::successGreen());
    renewSlice->setLabelVisible(true);
    renewSlice->setLabelColor(StyleManager::darkText());

    auto *fossilSlice = series->append("Fossil", 100.0 - metrics.globalRenewablePercent);
    fossilSlice->setColor(StyleManager::mediumGray());
    fossilSlice->setLabelVisible(true);
    fossilSlice->setLabelColor(StyleManager::darkText());

    series->setHoleSize(0.45);
    m_renewableChart->addSeries(series);
}

void MainWindow::updateEnergyMixChart()
{
    m_energyMixChart->removeAllSeries();
    for (auto *axis : m_energyMixChart->axes()) {
        m_energyMixChart->removeAxis(axis);
    }

    auto countries = m_dataManager->getCountryData();
    if (countries.isEmpty()) return;

    // Show top 8 countries
    auto sorted = countries;
    std::sort(sorted.begin(), sorted.end(),
              [](const CountryEnergyData &a, const CountryEnergyData &b) {
                  return a.totalDemandGW > b.totalDemandGW;
              });

    auto *renewSet = new QBarSet("Renewable");
    renewSet->setColor(StyleManager::successGreen());
    auto *fossilSet = new QBarSet("Fossil");
    fossilSet->setColor(StyleManager::mediumGray());

    QStringList categories;
    int count = qMin(8, sorted.size());
    for (int i = 0; i < count; ++i) {
        *renewSet << sorted[i].renewablePercentage;
        *fossilSet << sorted[i].fossilPercentage;
        categories << sorted[i].countryCode;
    }

    auto *series = new QBarSeries();
    series->append(renewSet);
    series->append(fossilSet);
    m_energyMixChart->addSeries(series);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(StyleManager::secondaryText());
    m_energyMixChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Percentage");
    axisY->setLabelsColor(StyleManager::secondaryText());
    m_energyMixChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void MainWindow::updateCarbonChart()
{
    m_carbonChart->removeAllSeries();
    for (auto *axis : m_carbonChart->axes()) {
        m_carbonChart->removeAxis(axis);
    }

    auto data = m_dataManager->getHistoricalPrices("CarbonEmissions", 48);
    if (data.isEmpty()) return;

    auto *lineSeries = new QLineSeries();
    lineSeries->setColor(StyleManager::dangerRed());
    QPen pen(StyleManager::dangerRed());
    pen.setWidth(2);
    lineSeries->setPen(pen);

    auto *lowerSeries = new QLineSeries();

    qreal minTime = std::numeric_limits<qreal>::max();
    qreal maxTime = std::numeric_limits<qreal>::lowest();
    qreal minVal = std::numeric_limits<qreal>::max();
    qreal maxVal = std::numeric_limits<qreal>::lowest();

    for (const auto &pt : data) {
        qreal ms = pt.timestamp.toMSecsSinceEpoch();
        lineSeries->append(ms, pt.value);
        lowerSeries->append(ms, 0);
        minTime = qMin(minTime, ms);
        maxTime = qMax(maxTime, ms);
        minVal = qMin(minVal, pt.value);
        maxVal = qMax(maxVal, pt.value);
    }

    auto *areaSeries = new QAreaSeries(lineSeries, lowerSeries);
    areaSeries->setName("CO₂ Emissions (MT)");
    QColor fillColor = StyleManager::dangerRed();
    fillColor.setAlpha(40);
    areaSeries->setBrush(fillColor);
    areaSeries->setPen(pen);

    m_carbonChart->addSeries(areaSeries);

    auto *axisX = new QDateTimeAxis();
    axisX->setFormat("hh:mm");
    axisX->setLabelsColor(StyleManager::secondaryText());
    m_carbonChart->addAxis(axisX, Qt::AlignBottom);
    areaSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setRange(minVal * 0.98, maxVal * 1.02);
    axisY->setTitleText("MT CO₂");
    axisY->setLabelsColor(StyleManager::secondaryText());
    m_carbonChart->addAxis(axisY, Qt::AlignLeft);
    areaSeries->attachAxis(axisY);
}

void MainWindow::updateAlertsList()
{
    auto alerts = m_dataManager->getAlerts();

    // Clear
    QLayoutItem *child;
    while ((child = m_alertsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (const auto &alert : alerts) {
        m_alertsLayout->addWidget(createAlertCard(alert));
    }
    m_alertsLayout->addStretch();
}

void MainWindow::updateAIInsights()
{
    auto insights = m_aiAssistant->generateDashboardInsights();
    QString summary = m_aiAssistant->generateRiskSummary();
    m_riskSummaryLabel->setText(summary);

    // Clear
    QLayoutItem *child;
    while ((child = m_insightsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (const auto &insight : insights) {
        m_insightsLayout->addWidget(createInsightCard(insight));
    }
    m_insightsLayout->addStretch();
}

void MainWindow::updateStatusBar()
{
    auto metrics = m_dataManager->getGlobalMetrics();
    m_lastUpdateLabel->setText("Last update: " +
                               metrics.lastUpdated.toString("hh:mm:ss"));

    auto countries = m_dataManager->getCountryData();
    int dataPoints = countries.size() * 9;
    m_dataPointsLabel->setText(QString("Data points: %1").arg(dataPoints));
}

void MainWindow::updateWeatherPanel()
{
    auto weather = m_dataManager->getWeatherImpacts();

    QLayoutItem *child;
    while ((child = m_weatherGrid->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int col = 0;
    for (const auto &w : weather) {
        QFrame *card = new QFrame();
        card->setObjectName("card");
        card->setStyleSheet(StyleManager::instance().getCardStyleSheet());
        card->setMinimumWidth(140);

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(10, 8, 10, 8);
        cardLayout->setSpacing(2);

        QLabel *regionLabel = new QLabel(w.region);
        regionLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
        regionLabel->setStyleSheet("color: #38804C;");
        cardLayout->addWidget(regionLabel);

        QLabel *tempLabel = new QLabel(QString("%1°C | %2 m/s")
                                          .arg(w.temperature, 0, 'f', 0)
                                          .arg(w.windSpeed, 0, 'f', 0));
        tempLabel->setFont(StyleManager::smallFont());
        cardLayout->addWidget(tempLabel);

        QLabel *impactLabel = new QLabel(
            QString("%1%2%").arg(w.energyImpactPercent >= 0 ? "+" : "")
                .arg(w.energyImpactPercent, 0, 'f', 1));
        impactLabel->setFont(QFont("Segoe UI", 11, QFont::Bold));
        impactLabel->setStyleSheet(w.energyImpactPercent >= 0
                                       ? "color: #38A856;" : "color: #DC4640;");
        cardLayout->addWidget(impactLabel);

        m_weatherGrid->addWidget(card, 0, col++);
    }
}

QFrame *MainWindow::createMetricCard(const QString &title, const QString &value,
                                      const QString &subtitle, const QColor &accentColor)
{
    QFrame *card = new QFrame();
    card->setObjectName("card");
    card->setStyleSheet(StyleManager::instance().getCardStyleSheet());
    card->setMinimumHeight(110);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(4);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("metricLabel");
    titleLabel->setFont(StyleManager::smallFont());
    layout->addWidget(titleLabel);

    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("metricValue");
    QFont valFont("Segoe UI", 22, QFont::Bold);
    valueLabel->setFont(valFont);
    valueLabel->setStyleSheet(QString("color: %1;").arg(accentColor.name()));
    layout->addWidget(valueLabel);

    QLabel *subLabel = new QLabel(subtitle);
    subLabel->setFont(StyleManager::smallFont());
    subLabel->setStyleSheet("color: #586E5E;");
    layout->addWidget(subLabel);

    // Add shadow
    auto *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 3);
    card->setGraphicsEffect(shadow);

    return card;
}

QFrame *MainWindow::createRiskCard(const CountryEnergyData &country)
{
    QFrame *card = new QFrame();
    card->setObjectName("card");
    card->setStyleSheet(StyleManager::instance().getCardStyleSheet());
    card->setMinimumWidth(200);
    card->setMaximumWidth(260);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(4);

    // Country name + code
    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *nameLabel = new QLabel(country.countryName);
    nameLabel->setFont(QFont("Segoe UI", 11, QFont::Bold));
    nameLabel->setStyleSheet("color: #212B26;");
    headerRow->addWidget(nameLabel);

    QLabel *codeLabel = new QLabel(country.countryCode);
    codeLabel->setFont(StyleManager::smallFont());
    codeLabel->setStyleSheet("color: #8A9A8E;");
    headerRow->addWidget(codeLabel);
    headerRow->addStretch();
    layout->addLayout(headerRow);

    // Risk score
    QColor riskColor = riskLevelColor(country.riskLevel);
    QLabel *scoreLabel = new QLabel(QString::number(country.riskScore, 'f', 1));
    scoreLabel->setFont(QFont("Segoe UI", 20, QFont::Bold));
    scoreLabel->setStyleSheet(QString("color: %1;").arg(riskColor.name()));
    layout->addWidget(scoreLabel);

    // Risk level badge
    QLabel *levelLabel = new QLabel(riskLevelToString(country.riskLevel));
    levelLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    levelLabel->setStyleSheet(
        QString("color: white; background-color: %1; border-radius: 4px; padding: 2px 8px;")
            .arg(riskColor.name()));
    levelLabel->setFixedHeight(20);
    levelLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(levelLabel, 0, Qt::AlignLeft);

    // Mini stats
    QLabel *statsLabel = new QLabel(
        QString("Renewable: %1% | Carbon: %2")
            .arg(country.renewablePercentage, 0, 'f', 0)
            .arg(country.carbonIntensityGCO2, 0, 'f', 0));
    statsLabel->setFont(StyleManager::smallFont());
    statsLabel->setStyleSheet("color: #586E5E;");
    layout->addWidget(statsLabel);

    // Renewable bar
    QProgressBar *bar = new QProgressBar();
    bar->setRange(0, 100);
    bar->setValue(static_cast<int>(country.renewablePercentage));
    bar->setTextVisible(false);
    bar->setFixedHeight(4);
    bar->setStyleSheet(
        "QProgressBar { background: #E8F5EC; border: none; border-radius: 2px; }"
        "QProgressBar::chunk { background: #4C9460; border-radius: 2px; }");
    layout->addWidget(bar);

    return card;
}

QFrame *MainWindow::createAlertCard(const EnergyAlert &alert)
{
    QFrame *card = new QFrame();
    card->setObjectName("alertCard");
    card->setStyleSheet(StyleManager::instance().getAlertCardStyleSheet(
        riskLevelToString(alert.severity)));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(4);

    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *titleLabel = new QLabel(alert.title);
    titleLabel->setFont(QFont("Segoe UI", 11, QFont::DemiBold));
    titleLabel->setWordWrap(true);
    headerRow->addWidget(titleLabel, 1);

    QLabel *timeLabel = new QLabel(alert.timestamp.toString("hh:mm"));
    timeLabel->setFont(StyleManager::smallFont());
    timeLabel->setStyleSheet("color: #8A9A8E;");
    headerRow->addWidget(timeLabel);
    layout->addLayout(headerRow);

    QLabel *descLabel = new QLabel(alert.description);
    descLabel->setFont(StyleManager::smallFont());
    descLabel->setStyleSheet("color: #586E5E;");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    return card;
}

QFrame *MainWindow::createInsightCard(const AIInsight &insight)
{
    QFrame *card = new QFrame();
    card->setObjectName("card");
    card->setStyleSheet(StyleManager::instance().getCardStyleSheet());

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(6);

    QHBoxLayout *headerRow = new QHBoxLayout();
    QLabel *titleLabel = new QLabel(insight.title);
    titleLabel->setFont(QFont("Segoe UI", 12, QFont::DemiBold));
    titleLabel->setStyleSheet("color: #212B26;");
    headerRow->addWidget(titleLabel, 1);

    QColor sevColor = riskLevelColor(insight.severity);
    QLabel *sevLabel = new QLabel(riskLevelToString(insight.severity));
    sevLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    sevLabel->setStyleSheet(
        QString("color: white; background-color: %1; border-radius: 4px; padding: 2px 8px;")
            .arg(sevColor.name()));
    sevLabel->setFixedHeight(20);
    headerRow->addWidget(sevLabel);
    layout->addLayout(headerRow);

    QLabel *descLabel = new QLabel(insight.description);
    descLabel->setFont(StyleManager::bodyFont());
    descLabel->setStyleSheet("color: #586E5E;");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    QLabel *recLabel = new QLabel("💡 " + insight.recommendation);
    recLabel->setFont(StyleManager::bodyFont());
    recLabel->setStyleSheet("color: #38804C; font-style: italic;");
    recLabel->setWordWrap(true);
    layout->addWidget(recLabel);

    QLabel *confLabel = new QLabel(
        QString("Confidence: %1% | Category: %2")
            .arg(insight.confidence * 100.0, 0, 'f', 0)
            .arg(insight.category));
    confLabel->setFont(StyleManager::smallFont());
    confLabel->setStyleSheet("color: #8A9A8E;");
    layout->addWidget(confLabel);

    return card;
}

QFrame *MainWindow::createChatBubble(const QString &text, bool isUser)
{
    QFrame *bubble = new QFrame();
    bubble->setObjectName(isUser ? "userMessage" : "aiMessage");

    QVBoxLayout *layout = new QVBoxLayout(bubble);
    layout->setContentsMargins(14, 10, 14, 10);

    QLabel *label = new QLabel(text);
    label->setWordWrap(true);
    label->setFont(isUser ? StyleManager::bodyFont() : StyleManager::monoFont());
    label->setStyleSheet(isUser ? "color: white;" : "color: #212B26;");
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(label);

    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm"));
    timeLabel->setFont(QFont("Segoe UI", 8));
    timeLabel->setStyleSheet(isUser ? "color: rgba(255,255,255,0.6);" : "color: #8A9A8E;");
    timeLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(timeLabel);

    return bubble;
}

QWidget *MainWindow::createSettingsSection(const QString &title, QLayout *content)
{
    QFrame *section = new QFrame();
    section->setObjectName("card");
    section->setStyleSheet(StyleManager::instance().getCardStyleSheet());

    QVBoxLayout *layout = new QVBoxLayout(section);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("subHeaderLabel");
    titleLabel->setFont(StyleManager::subHeaderFont());
    layout->addWidget(titleLabel);

    layout->addLayout(content);

    return section;
}

void MainWindow::animateWidget(QWidget *widget, int delay)
{
    auto *effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    effect->setOpacity(0);

    auto *anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(400);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    QTimer::singleShot(delay, anim, [anim]() { anim->start(QAbstractAnimation::DeleteWhenStopped); });
}
