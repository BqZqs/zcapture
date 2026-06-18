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
    enum Handle {
        None = -1,
        TopLeft, Top, TopRight, Right,
        BottomRight, Bottom, BottomLeft, Left,
        Center
    };

    void updateHandleRects();
    QCursor cursorForHandle(Handle h) const;

    QPixmap m_screenshot;
    QPoint m_startPoint;
    QRect m_selection;
    QRect m_selectionBeforeDrag;
    bool m_hasSelection = false;
    bool m_confirmed = false;

    // resize / move handles
    QRect m_handles[9];     // 0..7 = 8 handles, 8 = center
    Handle m_activeHandle = None;
    QPoint m_dragStart;

    CaptureActionBar m_actionBar;
};
