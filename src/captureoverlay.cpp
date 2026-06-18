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

// ----------------------------------------------------------------
// handle geometry
// ----------------------------------------------------------------
void CaptureOverlay::updateHandleRects()
{
    const int hs = 8;  // handle size
    const QRect& r = m_selection;
    int hhs = hs / 2;

    // corners
    m_handles[TopLeft]     = QRect(r.left() - hhs, r.top() - hhs, hs, hs);
    m_handles[TopRight]    = QRect(r.right() - hhs, r.top() - hhs, hs, hs);
    m_handles[BottomLeft]  = QRect(r.left() - hhs, r.bottom() - hhs, hs, hs);
    m_handles[BottomRight] = QRect(r.right() - hhs, r.bottom() - hhs, hs, hs);

    // edges
    m_handles[Top]    = QRect(r.center().x() - hhs, r.top() - hhs, hs, hs);
    m_handles[Bottom] = QRect(r.center().x() - hhs, r.bottom() - hhs, hs, hs);
    m_handles[Left]   = QRect(r.left() - hhs, r.center().y() - hhs, hs, hs);
    m_handles[Right]  = QRect(r.right() - hhs, r.center().y() - hhs, hs, hs);

    // center
    m_handles[Center] = r;

    // action bar
    m_actionBar.updateLayout(r);
}

QCursor CaptureOverlay::cursorForHandle(Handle h) const
{
    switch (h) {
    case TopLeft:
    case BottomRight: return Qt::SizeFDiagCursor;
    case TopRight:
    case BottomLeft:  return Qt::SizeBDiagCursor;
    case Left:
    case Right:       return Qt::SizeHorCursor;
    case Top:
    case Bottom:      return Qt::SizeVerCursor;
    case Center:      return Qt::SizeAllCursor;
    default:          return Qt::CrossCursor;
    }
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
            // draw 8 handles
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::white);
            for (int i = 0; i < 8; ++i) {
                painter.drawRect(m_handles[i]);
            }
            // draw action buttons
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
        // check action buttons first
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
        m_activeHandle = None;

        // hit test handles (check corners/edges first, center last)
        for (int i = 0; i < 9; ++i) {
            if (m_handles[i].contains(p)) {
                m_activeHandle = static_cast<Handle>(i);
                break;
            }
        }
        return;
    }

    m_startPoint = p;
    m_hasSelection = true;
    m_selection = QRect(m_startPoint, QSize(0, 0));
}

void CaptureOverlay::mouseMoveEvent(QMouseEvent* e)
{
    if (m_confirmed) {
        if (m_activeHandle != None) {
            QPoint delta = e->pos() - m_dragStart;
            QRect& r = m_selectionBeforeDrag;
            auto& sel = m_selection;

            switch (m_activeHandle) {
            case TopLeft:
                sel = QRect(r.topLeft() + delta, r.bottomRight()).normalized();
                break;
            case TopRight:
                sel = QRect(QPoint(r.right() + delta.x(), r.top() + delta.y()),
                            r.bottomLeft()).normalized();
                break;
            case BottomLeft:
                sel = QRect(QPoint(r.left() + delta.x(), r.bottom() + delta.y()),
                            r.topRight()).normalized();
                break;
            case BottomRight:
                sel = QRect(r.topLeft(), r.bottomRight() + delta).normalized();
                break;
            case Top:
                sel = QRect(QPoint(r.left(), r.top() + delta.y()),
                            r.bottomRight()).normalized();
                break;
            case Bottom:
                sel = QRect(r.topLeft(),
                            QPoint(r.right(), r.bottom() + delta.y())).normalized();
                break;
            case Left:
                sel = QRect(QPoint(r.left() + delta.x(), r.top()),
                            r.bottomRight()).normalized();
                break;
            case Right:
                sel = QRect(r.topLeft(),
                            QPoint(r.right() + delta.x(), r.bottom())).normalized();
                break;
            case Center:
                sel = r.translated(delta);
                break;
            default: return;
            }
            updateHandleRects();
            update();
        } else {
            // hover — update cursor
            for (int i = 0; i < 9; ++i) {
                if (m_handles[i].contains(e->pos())) {
                    setCursor(cursorForHandle(static_cast<Handle>(i)));
                    return;
                }
            }
            setCursor(Qt::CrossCursor);
        }
        return;
    }

    // initial selection drag
    if (m_hasSelection) {
        m_selection = QRect(m_startPoint, e->pos()).normalized();
        update();
    }
}

void CaptureOverlay::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_confirmed) {
        m_activeHandle = None;
        return;
    }

    if (m_hasSelection) {
        m_selection = m_selection.normalized();
        if (m_selection.width() > 10 && m_selection.height() > 10) {
            m_confirmed = true;
            updateHandleRects();
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
