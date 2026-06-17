#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QRect>

#include "tools/abstractactiontool/captureactionbar.h"

class CaptureOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureOverlay();

signals:
    void captureFinished(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    QPixmap m_screenshot;
    QPoint m_startPoint;
    QRect m_selection;
    bool m_hasSelection = false;
    bool m_confirmed = false;

    CaptureActionBar m_actionBar;
};
