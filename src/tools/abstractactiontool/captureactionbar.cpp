#include "captureactionbar.h"

#include <QPainter>
#include <QFont>

void CaptureActionBar::updateLayout(const QRect& selection)
{
    const int bw = 80;
    const int bh = 36;
    const int gap = 10;
    const int spacing = 12;

    int totalW = bw * 2 + spacing;
    int x = selection.center().x() - totalW / 2;
    int y = selection.bottom() + gap;

    m_cancelBtn = QRect(x, y, bw, bh);
    m_saveBtn = QRect(x + bw + spacing, y, bw, bh);
}

void CaptureActionBar::paint(QPainter& painter) const
{
    // Cancel (red)
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220, 50, 50));
    painter.drawRoundedRect(m_cancelBtn, 6, 6);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
    painter.drawText(m_cancelBtn, Qt::AlignCenter, "\u2715"); // ✕

    // Save (green)
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(50, 180, 70));
    painter.drawRoundedRect(m_saveBtn, 6, 6);
    painter.setPen(Qt::white);
    painter.drawText(m_saveBtn, Qt::AlignCenter, "\u2713"); // ✓
}

ActionButton CaptureActionBar::hitTest(QPoint pos) const
{
    if (m_saveBtn.contains(pos)) return ActionButton::Save;
    if (m_cancelBtn.contains(pos)) return ActionButton::Cancel;
    return ActionButton::None;
}
