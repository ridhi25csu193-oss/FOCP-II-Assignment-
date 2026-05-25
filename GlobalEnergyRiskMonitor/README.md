# 🌿 Global Energy Risk Monitor — AI-Powered Dashboard

A modern, futuristic, AI-powered real-time energy monitoring dashboard built with Qt/C++17. Features a nature-inspired green/white eco-tech aesthetic with glassmorphism elements, real-time data simulation, AI-powered analytics, and comprehensive reporting.

![Qt](https://img.shields.io/badge/Qt-6.x-green) ![C++](https://img.shields.io/badge/C++-17-blue) ![License](https://img.shields.io/badge/License-MIT-brightgreen)

---

## Features

### 🎨 Modern UI/UX
- **Eco-tech green/white color theme** with sage, mint, and forest green palette
- **Glassmorphism cards** with rounded corners, soft shadows, and smooth gradients
- **Animated startup splash screen** with particle effects and progress animation
- **Professional sidebar navigation** with gradient background
- **Responsive dashboard layout** with metric cards, charts, and data panels
- **Full-screen mode** (F11) for presentations

### 📊 Dashboard Components
- **World Energy Overview** — Global metrics at a glance
- **Live Risk Indicators** — Real-time global risk score with gauge
- **Country Risk Cards** — 20 countries with risk scoring
- **Renewable vs Fossil Comparison** — Interactive pie charts
- **Real-time Charts** — Price trends, energy mix, carbon emissions
- **Weather Impact Panel** — Regional weather effects on energy
- **Alert Section** — Categorized energy alerts with severity levels
- **AI Insight Section** — Auto-generated analysis and recommendations

### 🤖 AI Integration
- **Predictive Risk Analysis** — Trend detection and forecasting
- **AI Insight Generator** — Automated textual insights with confidence scores
- **AI Chat Assistant** — Natural language queries about energy data
  - "What is India's current energy risk?"
  - "Show renewable trends in Europe"
  - "Why is oil volatility increasing?"
  - "Current carbon emission status?"
- **Risk Summaries** — AI-generated executive summaries
- **Emergency Simulation** — 5 pre-built crisis scenarios

### 📈 Data & Analytics
- **20 countries** with comprehensive energy profiles
- **Real-time simulation** — Data updates every 30 seconds (configurable)
- **Historical price tracking** — Oil, Natural Gas, Coal trends
- **Carbon emission monitoring** — Global CO₂ tracking
- **Energy mix analysis** — Renewable vs fossil breakdown by country
- **Weather impact analysis** — 7 regions with energy impact metrics

### 📋 Reporting
- **HTML Report Export** — Styled report with metrics, tables, insights
- **CSV Data Export** — Country data in spreadsheet format
- **JSON Export** — Machine-readable data export

### ⚡ Technical Features
- **C++17** with Qt Widgets and Qt Charts
- **MVC-inspired architecture** — Clean separation of concerns
- **Thread-safe data management** with QMutex
- **Configurable settings** via QSettings
- **Smooth chart animations** with Qt Charts
- **Keyboard shortcuts** — F5 (Refresh), F11 (Fullscreen)

---

## Project Structure

```
GlobalEnergyRiskMonitor/
├── GlobalEnergyRiskMonitor.pro  — Qt project file
├── main.cpp                     — Application entry point
├── mainwindow.h/.cpp            — Main dashboard UI (7 pages)
├── datamodels.h                 — Data structures and enums
├── datamanager.h/.cpp           — Data management and simulation
├── riskengine.h/.cpp            — Risk calculation and AI analysis
├── aiassistant.h/.cpp           — AI chat and insights module
├── stylemanager.h/.cpp          — Eco-tech theme and stylesheets
├── configmanager.h/.cpp         — Application settings
├── reportgenerator.h/.cpp       — HTML/CSV/JSON export
├── splashscreen.h/.cpp          — Animated startup screen
└── README.md                    — This file
```

---

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                    MainWindow (UI)                    │
│  ┌─────────┐ ┌──────────┐ ┌────────┐ ┌───────────┐ │
│  │Dashboard │ │Countries │ │ Charts │ │ AI Pages  │ │
│  └────┬─────┘ └────┬─────┘ └───┬────┘ └─────┬─────┘ │
│       │            │           │             │       │
├───────┼────────────┼───────────┼─────────────┼───────┤
│       ▼            ▼           ▼             ▼       │
│  ┌─────────────────────────────────────────────────┐ │
│  │              DataManager (Backend)              │ │
│  │  • Real-time simulation  • API integration      │ │
│  │  • Historical data       • Auto-refresh         │ │
│  └───────────────────┬─────────────────────────────┘ │
│                      │                               │
│  ┌───────────────────┼─────────────────────────────┐ │
│  │   RiskEngine      │      AIAssistant            │ │
│  │  • Risk scoring   │   • NLP query processing    │ │
│  │  • Forecasting    │   • Insight generation      │ │
│  │  • Trend analysis │   • Chat interface          │ │
│  │  • Emergency sim  │   • Risk summaries          │ │
│  └───────────────────┴─────────────────────────────┘ │
│                                                       │
│  ┌──────────┐  ┌──────────────┐  ┌────────────────┐ │
│  │ StyleMgr │  │ ConfigMgr    │  │ ReportGenerator │ │
│  │ (Theme)  │  │ (Settings)   │  │ (Export)        │ │
│  └──────────┘  └──────────────┘  └────────────────┘ │
└─────────────────────────────────────────────────────┘
```

---

## Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| Qt Core    | 5.15+ / 6.x | Core framework |
| Qt Widgets | 5.15+ / 6.x | UI components |
| Qt Charts  | 5.15+ / 6.x | Chart visualization |
| Qt Network | 5.15+ / 6.x | API connectivity |
| C++17      | GCC 7+ / MSVC 2017+ / Clang 5+ | Language standard |

---

## Build & Setup Instructions

### Prerequisites
1. Install **Qt Creator** (5.15+ or 6.x recommended)
2. Ensure Qt modules are installed: `core`, `gui`, `widgets`, `charts`, `network`
3. C++17 compatible compiler

### Build Steps

#### Option 1: Qt Creator (Recommended)
1. Open `GlobalEnergyRiskMonitor.pro` in Qt Creator
2. Configure the project with your Qt kit
3. Click **Build** → **Build Project** (Ctrl+B)
4. Click **Run** (Ctrl+R)

#### Option 2: Command Line
```bash
cd GlobalEnergyRiskMonitor
qmake GlobalEnergyRiskMonitor.pro
make -j$(nproc)    # Linux/Mac
# or: nmake        # Windows with MSVC
# or: mingw32-make # Windows with MinGW
./GlobalEnergyRiskMonitor
```

---

## Usage Guide

### Navigation
- Use the **sidebar** to switch between pages
- **Dashboard** — Main overview with metrics, charts, country cards
- **Countries** — Detailed table with filtering and sorting
- **Charts** — Energy mix, carbon trends, price analysis
- **AI Insights** — AI-generated analysis and emergency simulation
- **AI Chat** — Ask questions in natural language
- **Alerts** — Real-time energy alerts
- **Settings** — Configure refresh rate, animations

### Keyboard Shortcuts
| Key | Action |
|-----|--------|
| F5  | Refresh data |
| F11 | Toggle fullscreen |

### AI Chat Examples
| Query | Response |
|-------|----------|
| "What is India's current energy risk?" | Detailed India risk analysis |
| "Show renewable trends in Europe" | European renewable breakdown |
| "Oil price forecast" | 24h price prediction with confidence |
| "Carbon emission status" | Global CO₂ analysis by country |
| "Compare renewable vs fossil" | Visual comparison with bars |

### Emergency Scenarios
1. **Oil Supply Shock** — 30% supply disruption
2. **Renewable Grid Failure** — European grid instability
3. **Extreme Weather Event** — Category 5 hurricane
4. **Nuclear Incident** — Facility shutdown
5. **Carbon Tax Implementation** — $50/ton CO₂ tax

---

## Data Sources

The application uses **realistic simulated data** that mimics real-world patterns. The architecture supports integration with real APIs:

| Data Type | Simulated | API Ready |
|-----------|-----------|-----------|
| Oil/Gas/Coal Prices | ✓ | OpenAPI Energy |
| Renewable Share | ✓ | IRENA API |
| Carbon Emissions | ✓ | Carbon Interface |
| Weather Impact | ✓ | OpenWeather API |
| Energy Demand/Supply | ✓ | ElectricityMap |
| News & Alerts | ✓ | NewsAPI |

To connect real APIs, add API keys in Settings and implement the corresponding fetch methods in `DataManager`.

---

## Modules Explained

### DataManager (`datamanager.h/.cpp`)
Manages all data flow. Uses realistic simulation with 20 countries, 7 weather regions, and commodity prices. Supports auto-refresh with configurable intervals. Thread-safe with QMutex.

### RiskEngine (`riskengine.h/.cpp`)
AI-powered risk analysis engine. Calculates weighted risk scores using oil dependency, carbon intensity, supply deficit, and renewable gap. Includes trend analysis (linear regression), volatility calculation, and forecasting with confidence intervals.

### AIAssistant (`aiassistant.h/.cpp`)
Natural language processing module. Parses user queries using keyword matching and context analysis. Generates insights from data patterns. Supports country-specific queries, market analysis, forecasting, and comparison queries.

### StyleManager (`stylemanager.h/.cpp`)
Centralized theme management. Defines the eco-tech green/white color palette with 16+ color definitions. Generates Qt stylesheets for all UI components including glassmorphism cards, sidebar gradients, chat bubbles, and alert cards.

### SplashScreen (`splashscreen.h/.cpp`)
Custom-painted animated startup screen. Features particle animation, gradient background, leaf eco-icon, and smooth progress bar with glow effects. Uses QPainter for all rendering.

### ConfigManager (`configmanager.h/.cpp`)
Persistent settings using QSettings. Stores refresh intervals, API keys, display preferences, chart animation settings, and window state.

### ReportGenerator (`reportgenerator.h/.cpp`)
Export engine supporting HTML (styled report with CSS), CSV (spreadsheet data), and JSON (machine-readable). HTML reports include metrics grids, country tables, AI insights, and alert sections.

---

## Presentation Tips

1. **Start with the splash screen** — Shows professional polish
2. **Dashboard overview** — Demonstrate live updating metrics
3. **AI Chat** — Ask real questions, show intelligent responses
4. **Emergency Simulation** — Run "Oil Supply Shock" to show impact
5. **Export Report** — Generate HTML report and open in browser
6. **Fullscreen** — Press F11 for immersive presentation
7. **Country Analysis** — Filter by region, sort by risk score
8. **Charts** — Show animated chart transitions

---

## License

This project is developed for academic purposes. Built with Qt Framework.
