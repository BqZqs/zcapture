#include "captureoverlay.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>

CaptureOverlay::CaptureOverlay()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setMouseTracking(true);

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        m_screenshot = screen->grabWindow(0);
        setGeometry(screen->geometry());
    }
}

void CaptureOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_screenshot);

    if (m_hasSelection) {
        // darkened non-selection area
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        QRegion full(rect());
        QRegion inner(m_selection);
        painter.setClipRegion(full.subtracted(inner));
        painter.drawRect(rect());
        painter.setClipRect(rect());

        // selection border
        painter.setPen(QPen(m_confirmed ? Qt::green : Qt::red, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(m_selection);

        // action buttons
        if (m_confirmed) {
            m_actionBar.paint(painter);
        }
    }

    if (!m_confirmed) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(rect().adjusted(0, 0, 0, -20),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         "Drag to select a region  |  ESC to cancel");
    }
}

void CaptureOverlay::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_confirmed) {
        ActionButton action = m_actionBar.hitTest(e->pos());
        if (action == ActionButton::Save) {
            emit captureFinished(m_screenshot.copy(m_selection));
            close();
        } else if (action == ActionButton::Cancel) {
            emit captureFinished(QPixmap());
            close();
        }
        return;
    }

    m_startPoint = e->pos();
    m_hasSelection = true;
    m_selection = QRect(m_startPoint, QSize(0, 0));
}

void CaptureOverlay::mouseMoveEvent(QMouseEvent* e)
{
    if (!m_confirmed && m_hasSelection) {
        m_selection = QRect(m_startPoint, e->pos()).normalized();
        update();
    }
}

void CaptureOverlay::mouseReleaseEvent(QMouseEvent* e)
{
    if (m_confirmed) return;

    if (e->button() == Qt::LeftButton && m_hasSelection) {
        m_selection = m_selection.normalized();
        if (m_selection.width() > 10 && m_selection.height() > 10) {
            m_confirmed = true;
            m_actionBar.updateLayout(m_selection);
            update();
        } else {
            emit captureFinished(QPixmap());
            close();
        }
    }
}

void CaptureOverlay::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        emit captureFinished(QPixmap());
        close();
    }
    if (m_confirmed && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {
        emit captureFinished(m_screenshot.copy(m_selection));
        close();
    }
}
