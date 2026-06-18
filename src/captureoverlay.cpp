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

    QApplication::setOverrideCursor(Qt::CrossCursor);
}

void CaptureOverlay::updateHandlesAndBar()
{
    m_handles.updateLayout(m_selection);
    m_actionBar.updateLayout(m_selection);
}

// ----------------------------------------------------------------
// paint
// ----------------------------------------------------------------
void CaptureOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_screenshot);

    if (m_hasSelection) {
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        QRegion full(rect());
        QRegion inner(m_selection);
        painter.setClipRegion(full.subtracted(inner));
        painter.drawRect(rect());
        painter.setClipRect(rect());

        painter.setPen(QPen(m_confirmed ? Qt::green : Qt::red, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(m_selection);

        if (m_confirmed) {
            m_handles.paint(painter);
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

// ----------------------------------------------------------------
// mouse
// ----------------------------------------------------------------
void CaptureOverlay::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    QPoint p = e->pos();

    if (m_confirmed) {
        ActionButton btn = m_actionBar.hitTest(p);
        if (btn == ActionButton::Save) {
            QApplication::restoreOverrideCursor();
            emit captureFinished(m_screenshot.copy(m_selection));
            close();
            return;
        }
        if (btn == ActionButton::Cancel) {
            QApplication::restoreOverrideCursor();
            emit captureFinished(QPixmap());
            close();
            return;
        }

        m_selectionBeforeDrag = m_selection;
        m_dragStart = p;
        m_activeHandle = m_handles.hitTest(p);
        return;
    }

    m_startPoint = p;
    m_hasSelection = true;
    m_selection = QRect(m_startPoint, QSize(0, 0));
}

void CaptureOverlay::mouseMoveEvent(QMouseEvent* e)
{
    if (m_confirmed) {
        if (m_activeHandle != SelectionHandles::None) {
            QPoint delta = e->pos() - m_dragStart;
            m_selection = m_handles.applyResize(
                m_selectionBeforeDrag, m_activeHandle, delta);
            updateHandlesAndBar();
            update();
        } else {
            auto h = m_handles.hitTest(e->pos());
            if (h != SelectionHandles::None)
                setCursor(m_handles.cursorFor(h));
            else
                setCursor(Qt::CrossCursor);
        }
        return;
    }

    if (m_hasSelection) {
        m_selection = QRect(m_startPoint, e->pos()).normalized();
        update();
    }
}

void CaptureOverlay::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_confirmed) {
        m_activeHandle = SelectionHandles::None;
        return;
    }

    if (m_hasSelection) {
        m_selection = m_selection.normalized();
        if (m_selection.width() > 10 && m_selection.height() > 10) {
            m_confirmed = true;
            updateHandlesAndBar();
            update();
        } else {
            QApplication::restoreOverrideCursor();
            emit captureFinished(QPixmap());
            close();
        }
    }
}

// ----------------------------------------------------------------
// keyboard
// ----------------------------------------------------------------
void CaptureOverlay::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        QApplication::restoreOverrideCursor();
        emit captureFinished(QPixmap());
        close();
    }
    if (m_confirmed && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {
        QApplication::restoreOverrideCursor();
        emit captureFinished(m_screenshot.copy(m_selection));
        close();
    }
}
