#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QString>
#include <QColor>
#include <QFont>

class StyleManager
{
public:
    static StyleManager &instance();

    QString getApplicationStyleSheet() const;
    QString getCardStyleSheet() const;
    QString getChartContainerStyleSheet() const;
    QString getSidebarStyleSheet() const;
    QString getAlertCardStyleSheet(const QString &severity) const;
    QString getChatStyleSheet() const;

    // Colors
    static QColor primaryGreen()       { return QColor(76, 148, 96); }
    static QColor softForestGreen()    { return QColor(56, 128, 76); }
    static QColor sageGreen()          { return QColor(138, 174, 138); }
    static QColor mintGreen()          { return QColor(186, 220, 194); }
    static QColor lightMint()          { return QColor(220, 240, 225); }
    static QColor paleGreen()          { return QColor(240, 248, 242); }
    static QColor white()             { return QColor(255, 255, 255); }
    static QColor lightGray()          { return QColor(245, 247, 246); }
    static QColor mediumGray()         { return QColor(200, 210, 204); }
    static QColor darkText()           { return QColor(33, 43, 38); }
    static QColor secondaryText()      { return QColor(88, 100, 94); }
    static QColor accentBlue()         { return QColor(70, 140, 200); }
    static QColor warningOrange()      { return QColor(240, 160, 50); }
    static QColor dangerRed()          { return QColor(220, 70, 60); }
    static QColor successGreen()       { return QColor(56, 168, 86); }

    // Fonts
    static QFont headerFont();
    static QFont subHeaderFont();
    static QFont bodyFont();
    static QFont smallFont();
    static QFont monoFont();

private:
    StyleManager() = default;
};

#endif // STYLEMANAGER_H
