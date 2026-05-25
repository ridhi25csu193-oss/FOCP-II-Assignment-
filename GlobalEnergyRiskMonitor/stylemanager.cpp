#include "stylemanager.h"

StyleManager &StyleManager::instance()
{
    static StyleManager mgr;
    return mgr;
}

QFont StyleManager::headerFont()
{
    QFont f("Segoe UI", 18, QFont::Bold);
    return f;
}

QFont StyleManager::subHeaderFont()
{
    QFont f("Segoe UI", 13, QFont::DemiBold);
    return f;
}

QFont StyleManager::bodyFont()
{
    QFont f("Segoe UI", 11);
    return f;
}

QFont StyleManager::smallFont()
{
    QFont f("Segoe UI", 9);
    return f;
}

QFont StyleManager::monoFont()
{
    QFont f("Consolas", 10);
    f.setStyleHint(QFont::Monospace);
    return f;
}

QString StyleManager::getApplicationStyleSheet() const
{
    return R"(
        /* ═══ Global Application Theme ═══ */
        QMainWindow {
            background-color: #F0F8F2;
        }

        QWidget {
            font-family: 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;
            color: #212B26;
        }

        /* ── Scrollbar ── */
        QScrollBar:vertical {
            background: #F0F8F2;
            width: 8px;
            border-radius: 4px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #B0C8B8;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: #8AAE8A;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            background: #F0F8F2;
            height: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:horizontal {
            background: #B0C8B8;
            border-radius: 4px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #8AAE8A;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
        }

        /* ── Buttons ── */
        QPushButton {
            background-color: #4C9460;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 20px;
            font-size: 12px;
            font-weight: 600;
            min-height: 32px;
        }
        QPushButton:hover {
            background-color: #3A8050;
        }
        QPushButton:pressed {
            background-color: #2E6A40;
        }
        QPushButton:disabled {
            background-color: #C8D8CC;
            color: #8A9A8E;
        }
        QPushButton#secondaryBtn {
            background-color: transparent;
            color: #4C9460;
            border: 2px solid #4C9460;
        }
        QPushButton#secondaryBtn:hover {
            background-color: #E8F5EC;
        }
        QPushButton#dangerBtn {
            background-color: #DC4640;
        }
        QPushButton#dangerBtn:hover {
            background-color: #C03830;
        }

        /* ── Input fields ── */
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: white;
            border: 2px solid #DCF0E1;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 12px;
            color: #212B26;
            selection-background-color: #BAE0C8;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border-color: #4C9460;
        }

        /* ── ComboBox ── */
        QComboBox {
            background-color: white;
            border: 2px solid #DCF0E1;
            border-radius: 8px;
            padding: 6px 12px;
            font-size: 12px;
            color: #212B26;
            min-height: 28px;
        }
        QComboBox:hover {
            border-color: #8AAE8A;
        }
        QComboBox::drop-down {
            border: none;
            width: 28px;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            border: 2px solid #DCF0E1;
            border-radius: 4px;
            selection-background-color: #E8F5EC;
            selection-color: #212B26;
            padding: 4px;
        }

        /* ── Labels ── */
        QLabel {
            color: #212B26;
        }
        QLabel#headerLabel {
            font-size: 20px;
            font-weight: bold;
            color: #2E6A40;
        }
        QLabel#subHeaderLabel {
            font-size: 14px;
            font-weight: 600;
            color: #4C9460;
        }
        QLabel#metricValue {
            font-size: 28px;
            font-weight: bold;
            color: #38804C;
        }
        QLabel#metricLabel {
            font-size: 11px;
            color: #586E5E;
            font-weight: 500;
        }
        QLabel#statusGood {
            color: #38A856;
        }
        QLabel#statusWarning {
            color: #F0A032;
        }
        QLabel#statusDanger {
            color: #DC4640;
        }

        /* ── Tab Widget ── */
        QTabWidget::pane {
            background-color: white;
            border: 1px solid #DCF0E1;
            border-radius: 10px;
            padding: 8px;
        }
        QTabBar::tab {
            background-color: #E8F5EC;
            color: #586E5E;
            border: none;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            padding: 10px 20px;
            font-size: 12px;
            font-weight: 600;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background-color: white;
            color: #2E6A40;
            border-bottom: 3px solid #4C9460;
        }
        QTabBar::tab:hover:!selected {
            background-color: #DCECE2;
        }

        /* ── Table ── */
        QTableWidget, QTableView {
            background-color: white;
            border: 1px solid #DCF0E1;
            border-radius: 8px;
            gridline-color: #E8F5EC;
            selection-background-color: #E8F5EC;
            selection-color: #212B26;
            font-size: 11px;
        }
        QHeaderView::section {
            background-color: #F0F8F2;
            color: #38804C;
            border: none;
            border-bottom: 2px solid #BAE0C8;
            padding: 8px 12px;
            font-weight: 600;
            font-size: 11px;
        }

        /* ── Group Box ── */
        QGroupBox {
            background-color: white;
            border: 1px solid #DCF0E1;
            border-radius: 10px;
            margin-top: 16px;
            padding: 16px;
            font-weight: 600;
        }
        QGroupBox::title {
            color: #38804C;
            subcontrol-origin: margin;
            padding: 0 8px;
        }

        /* ── Progress Bar ── */
        QProgressBar {
            background-color: #E8F5EC;
            border: none;
            border-radius: 6px;
            height: 12px;
            text-align: center;
            font-size: 9px;
            color: #586E5E;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4C9460, stop:1 #38A856);
            border-radius: 6px;
        }

        /* ── Splitter ── */
        QSplitter::handle {
            background-color: #DCF0E1;
            width: 2px;
            height: 2px;
        }

        /* ── Tool Tip ── */
        QToolTip {
            background-color: #2E6A40;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 6px 10px;
            font-size: 11px;
        }

        /* ── Menu ── */
        QMenuBar {
            background-color: white;
            border-bottom: 1px solid #DCF0E1;
            padding: 2px;
        }
        QMenuBar::item {
            padding: 6px 14px;
            border-radius: 4px;
            color: #586E5E;
        }
        QMenuBar::item:selected {
            background-color: #E8F5EC;
            color: #2E6A40;
        }
        QMenu {
            background-color: white;
            border: 1px solid #DCF0E1;
            border-radius: 8px;
            padding: 4px;
        }
        QMenu::item {
            padding: 8px 24px;
            border-radius: 4px;
        }
        QMenu::item:selected {
            background-color: #E8F5EC;
            color: #2E6A40;
        }

        /* ── Status Bar ── */
        QStatusBar {
            background-color: white;
            border-top: 1px solid #DCF0E1;
            color: #586E5E;
            font-size: 11px;
        }
    )";
}

QString StyleManager::getCardStyleSheet() const
{
    return R"(
        QFrame#card {
            background-color: white;
            border: 1px solid #E0EEDF;
            border-radius: 12px;
            padding: 16px;
        }
        QFrame#card:hover {
            border-color: #8AAE8A;
            background-color: #FCFEFC;
        }
        QFrame#cardHighlight {
            background-color: white;
            border: 2px solid #4C9460;
            border-radius: 12px;
            padding: 16px;
        }
        QFrame#glassCard {
            background-color: rgba(255, 255, 255, 0.85);
            border: 1px solid rgba(76, 148, 96, 0.2);
            border-radius: 12px;
            padding: 16px;
        }
    )";
}

QString StyleManager::getChartContainerStyleSheet() const
{
    return R"(
        QFrame#chartContainer {
            background-color: white;
            border: 1px solid #E0EEDF;
            border-radius: 12px;
            padding: 12px;
        }
    )";
}

QString StyleManager::getSidebarStyleSheet() const
{
    return R"(
        QFrame#sidebar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #38804C, stop:0.5 #2E6A40, stop:1 #1E4A2C);
            border-right: 1px solid rgba(255, 255, 255, 0.1);
        }
        QFrame#sidebar QPushButton {
            background-color: transparent;
            color: rgba(255, 255, 255, 0.85);
            border: none;
            border-radius: 8px;
            padding: 12px 16px;
            text-align: left;
            font-size: 12px;
            font-weight: 500;
        }
        QFrame#sidebar QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.12);
            color: white;
        }
        QFrame#sidebar QPushButton:checked,
        QFrame#sidebar QPushButton[active="true"] {
            background-color: rgba(255, 255, 255, 0.18);
            color: white;
            font-weight: 600;
        }
        QFrame#sidebar QLabel {
            color: rgba(255, 255, 255, 0.9);
        }
        QFrame#sidebar QLabel#sidebarTitle {
            color: white;
            font-size: 16px;
            font-weight: bold;
        }
        QFrame#sidebar QLabel#sidebarSubtitle {
            color: rgba(255, 255, 255, 0.6);
            font-size: 10px;
        }
    )";
}

QString StyleManager::getAlertCardStyleSheet(const QString &severity) const
{
    QString borderColor = "#BAE0C8";
    QString bgColor = "#F0F8F2";
    QString indicatorColor = "#4C9460";

    if (severity == "Critical") {
        borderColor = "#F5B7B1";
        bgColor = "#FDF2F2";
        indicatorColor = "#DC4640";
    } else if (severity == "High") {
        borderColor = "#FAD7A0";
        bgColor = "#FEF9E7";
        indicatorColor = "#F0A032";
    } else if (severity == "Moderate") {
        borderColor = "#FCE4B0";
        bgColor = "#FFFEF0";
        indicatorColor = "#E8C840";
    }

    return QString(R"(
        QFrame#alertCard {
            background-color: %1;
            border: 1px solid %2;
            border-left: 4px solid %3;
            border-radius: 8px;
            padding: 10px 14px;
            margin-bottom: 6px;
        }
    )").arg(bgColor, borderColor, indicatorColor);
}

QString StyleManager::getChatStyleSheet() const
{
    return R"(
        QFrame#chatContainer {
            background-color: white;
            border: 1px solid #E0EEDF;
            border-radius: 12px;
        }
        QFrame#chatMessages {
            background-color: #F8FCF9;
            border: none;
        }
        QFrame#userMessage {
            background-color: #4C9460;
            color: white;
            border-radius: 12px;
            border-bottom-right-radius: 4px;
            padding: 10px 14px;
            margin: 4px 8px 4px 60px;
        }
        QFrame#aiMessage {
            background-color: #E8F5EC;
            color: #212B26;
            border-radius: 12px;
            border-bottom-left-radius: 4px;
            padding: 10px 14px;
            margin: 4px 60px 4px 8px;
        }
        QLineEdit#chatInput {
            border: 2px solid #DCF0E1;
            border-radius: 20px;
            padding: 10px 16px;
            font-size: 13px;
            background-color: white;
        }
        QLineEdit#chatInput:focus {
            border-color: #4C9460;
        }
    )";
}
