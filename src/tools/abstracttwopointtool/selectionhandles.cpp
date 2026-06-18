#include "selectionhandles.h"

#include <QPainter>

void SelectionHandles::updateLayout(const QRect& selection)
{
    const int hs = 8;
    const QRect& r = selection;
    int hhs = hs / 2;

    m_handles[TopLeft]     = QRect(r.left() - hhs, r.top() - hhs, hs, hs);
    m_handles[TopRight]    = QRect(r.right() - hhs, r.top() - hhs, hs, hs);
    m_handles[BottomLeft]  = QRect(r.left() - hhs, r.bottom() - hhs, hs, hs);
    m_handles[BottomRight] = QRect(r.right() - hhs, r.bottom() - hhs, hs, hs);

    m_handles[Top]    = QRect(r.center().x() - hhs, r.top() - hhs, hs, hs);
    m_handles[Bottom] = QRect(r.center().x() - hhs, r.bottom() - hhs, hs, hs);
    m_handles[Left]   = QRect(r.left() - hhs, r.center().y() - hhs, hs, hs);
    m_handles[Right]  = QRect(r.right() - hhs, r.center().y() - hhs, hs, hs);
}

void SelectionHandles::paint(QPainter& painter) const
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    for (int i = 0; i < 8; ++i) {
        painter.drawRect(m_handles[i]);
    }
}

SelectionHandles::Handle SelectionHandles::hitTest(QPoint pos) const
{
    for (int i = 0; i < 8; ++i) {
        if (m_handles[i].contains(pos))
            return static_cast<Handle>(i);
    }
    return None;
}

QCursor SelectionHandles::cursorFor(Handle h) const
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
    default:          return Qt::CrossCursor;
    }
}

QRect SelectionHandles::applyResize(const QRect& original,
                                     Handle h,
                                     QPoint delta) const
{
    const QRect& r = original;
    QRect result;

    switch (h) {
    case TopLeft:
        result = QRect(r.topLeft() + delta, r.bottomRight()).normalized();
        break;
    case TopRight:
        result = QRect(QPoint(r.right() + delta.x(), r.top() + delta.y()),
                        r.bottomLeft()).normalized();
        break;
    case BottomLeft:
        result = QRect(QPoint(r.left() + delta.x(), r.bottom() + delta.y()),
                        r.topRight()).normalized();
        break;
    case BottomRight:
        result = QRect(r.topLeft(), r.bottomRight() + delta).normalized();
        break;
    case Top:
        result = QRect(QPoint(r.left(), r.top() + delta.y()),
                        r.bottomRight()).normalized();
        break;
    case Bottom:
        result = QRect(r.topLeft(),
                        QPoint(r.right(), r.bottom() + delta.y())).normalized();
        break;
    case Left:
        result = QRect(QPoint(r.left() + delta.x(), r.top()),
                        r.bottomRight()).normalized();
        break;
    case Right:
        result = QRect(r.topLeft(),
                        QPoint(r.right() + delta.x(), r.bottom())).normalized();
        break;
    default:
        result = original;
        break;
    }
    return result;
}
