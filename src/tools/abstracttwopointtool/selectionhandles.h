#pragma once

#include <QRect>
#include <QPoint>
#include <QCursor>

class QPainter;

class SelectionHandles
{
public:
    enum Handle {
        None = -1,
        TopLeft, Top, TopRight, Right,
        BottomRight, Bottom, BottomLeft, Left
    };

    void updateLayout(const QRect& selection);
    void paint(QPainter& painter) const;
    Handle hitTest(QPoint pos) const;
    QCursor cursorFor(Handle h) const;

    /// Apply a resize to the original selection given a drag delta
    QRect applyResize(const QRect& original, Handle h, QPoint delta) const;

private:
    QRect m_handles[8];
};
