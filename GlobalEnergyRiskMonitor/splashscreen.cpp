#include "splashscreen.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QtMath>
#include <QApplication>
#include <QScreen>

SplashScreen::SplashScreen(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_animTimer(new QTimer(this))
    , m_progressAnim(new QPropertyAnimation(this, "progress", this))
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(600, 400);

    // Center on screen
    if (QScreen *screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    m_loadingSteps = {
        "Initializing system...",
        "Loading energy databases...",
        "Connecting to data feeds...",
        "Calibrating risk engine...",
        "Starting AI modules...",
        "Building dashboard...",
        "Ready!"
    };

    connect(m_animTimer, &QTimer::timeout, this, [this]() {
        m_particleAngle += 2.0;
        if (m_particleAngle >= 360.0) m_particleAngle = 0.0;
        update();
    });

    m_progressAnim->setDuration(3500);
    m_progressAnim->setStartValue(0.0);
    m_progressAnim->setEndValue(1.0);
    m_progressAnim->setEasingCurve(QEasingCurve::InOutCubic);

    connect(m_progressAnim, &QPropertyAnimation::finished, this, [this]() {
        QTimer::singleShot(400, this, &SplashScreen::finished);
    });
}

void SplashScreen::setProgress(double val)
{
    m_progress = val;
    m_currentStep = qBound(0, static_cast<int>(val * m_loadingSteps.size()),
                           m_loadingSteps.size() - 1);
    update();
}

void SplashScreen::startAnimation()
{
    show();
    m_animTimer->start(30);
    m_progressAnim->start();
}

void SplashScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background with rounded corners
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect(), 20, 20);
    p.setClipPath(bgPath);

    // Gradient background
    QLinearGradient bgGrad(0, 0, width(), height());
    bgGrad.setColorAt(0.0, QColor(46, 106, 64));
    bgGrad.setColorAt(0.5, QColor(56, 128, 76));
    bgGrad.setColorAt(1.0, QColor(36, 86, 50));
    p.fillRect(rect(), bgGrad);

    // Animated particles
    p.setPen(Qt::NoPen);
    for (int i = 0; i < 12; ++i) {
        double angle = m_particleAngle + i * 30.0;
        double rad = qDegreesToRadians(angle);
        double radius = 120 + qSin(rad * 2.0) * 30.0;
        double x = width() / 2.0 + qCos(rad) * radius;
        double y = height() / 2.0 - 30 + qSin(rad) * radius * 0.6;
        double size = 3.0 + qSin(rad + i) * 2.0;
        double alpha = 0.15 + qSin(rad * 1.5) * 0.1;

        QRadialGradient particleGrad(x, y, size * 3);
        particleGrad.setColorAt(0, QColor(255, 255, 255, static_cast<int>(alpha * 255)));
        particleGrad.setColorAt(1, QColor(255, 255, 255, 0));
        p.setBrush(particleGrad);
        p.drawEllipse(QPointF(x, y), size * 3, size * 3);
    }

    // Subtle grid lines
    p.setPen(QPen(QColor(255, 255, 255, 12), 1));
    for (int x = 0; x < width(); x += 40) {
        p.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += 40) {
        p.drawLine(0, y, width(), y);
    }

    // Leaf / eco icon
    p.setPen(Qt::NoPen);
    QRadialGradient iconGlow(width() / 2.0, 120, 50);
    iconGlow.setColorAt(0, QColor(186, 224, 200, 80));
    iconGlow.setColorAt(1, QColor(186, 224, 200, 0));
    p.setBrush(iconGlow);
    p.drawEllipse(QPointF(width() / 2.0, 120), 50, 50);

    // Leaf shape
    QPainterPath leaf;
    double cx = width() / 2.0;
    double cy = 120;
    leaf.moveTo(cx, cy - 25);
    leaf.cubicTo(cx + 20, cy - 20, cx + 25, cy, cx, cy + 25);
    leaf.cubicTo(cx - 25, cy, cx - 20, cy - 20, cx, cy - 25);
    p.setBrush(QColor(186, 224, 200));
    p.drawPath(leaf);

    // Leaf vein
    p.setPen(QPen(QColor(56, 128, 76), 1.5));
    p.drawLine(QPointF(cx, cy - 20), QPointF(cx, cy + 20));
    p.drawLine(QPointF(cx, cy - 8), QPointF(cx + 10, cy - 14));
    p.drawLine(QPointF(cx, cy), QPointF(cx - 10, cy - 6));
    p.drawLine(QPointF(cx, cy + 8), QPointF(cx + 8, cy + 2));

    // Title text
    p.setPen(QColor(255, 255, 255));
    QFont titleFont("Segoe UI", 22, QFont::Bold);
    p.setFont(titleFont);
    p.drawText(QRect(0, 160, width(), 40), Qt::AlignCenter, "Global Energy Risk Monitor");

    // Subtitle
    QFont subFont("Segoe UI", 11);
    p.setFont(subFont);
    p.setPen(QColor(186, 224, 200));
    p.drawText(QRect(0, 200, width(), 25), Qt::AlignCenter,
               "AI-Powered Real-Time Energy Intelligence");

    // Progress bar background
    int barX = 80;
    int barY = 280;
    int barW = width() - 160;
    int barH = 6;
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 30));
    p.drawRoundedRect(barX, barY, barW, barH, 3, 3);

    // Progress bar fill
    QLinearGradient barGrad(barX, barY, barX + barW, barY);
    barGrad.setColorAt(0.0, QColor(186, 224, 200));
    barGrad.setColorAt(1.0, QColor(130, 200, 160));
    p.setBrush(barGrad);
    int fillW = static_cast<int>(barW * m_progress);
    p.drawRoundedRect(barX, barY, fillW, barH, 3, 3);

    // Glow on progress tip
    if (fillW > 0) {
        QRadialGradient tipGlow(barX + fillW, barY + barH / 2.0, 15);
        tipGlow.setColorAt(0, QColor(186, 224, 200, 120));
        tipGlow.setColorAt(1, QColor(186, 224, 200, 0));
        p.setBrush(tipGlow);
        p.drawEllipse(QPointF(barX + fillW, barY + barH / 2.0), 15, 15);
    }

    // Loading step text
    p.setPen(QColor(186, 224, 200, 200));
    QFont stepFont("Segoe UI", 10);
    p.setFont(stepFont);
    if (m_currentStep < m_loadingSteps.size()) {
        p.drawText(QRect(0, 300, width(), 25), Qt::AlignCenter,
                   m_loadingSteps[m_currentStep]);
    }

    // Progress percentage
    p.setPen(QColor(255, 255, 255, 180));
    QFont pctFont("Segoe UI", 9);
    p.setFont(pctFont);
    p.drawText(QRect(0, 322, width(), 20), Qt::AlignCenter,
               QString("%1%").arg(static_cast<int>(m_progress * 100)));

    // Version / footer
    p.setPen(QColor(255, 255, 255, 80));
    QFont footFont("Segoe UI", 8);
    p.setFont(footFont);
    p.drawText(QRect(0, height() - 35, width(), 20), Qt::AlignCenter,
               "v2.0  |  Powered by Advanced AI Analytics");
}
