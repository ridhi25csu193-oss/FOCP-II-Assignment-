#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class SplashScreen : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress WRITE setProgress)

public:
    explicit SplashScreen(QWidget *parent = nullptr);

    double progress() const { return m_progress; }
    void setProgress(double val);

    void startAnimation();

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_progress = 0.0;
    double m_particleAngle = 0.0;
    QTimer *m_animTimer;
    QPropertyAnimation *m_progressAnim;
    QStringList m_loadingSteps;
    int m_currentStep = 0;
};

#endif // SPLASHSCREEN_H
