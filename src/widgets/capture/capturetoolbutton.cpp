#include "capturetoolbutton.h"
#include <QPainter>
#include <QFont>

CaptureToolButton::CaptureToolButton(Type type, QWidget* parent)
  : QWidget(parent)
  , m_type(type)
{
    setFixedSize(80, 36);   // 固定尺寸
    setMouseTracking(true);
}

void CaptureToolButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 背景圆角矩形
    p.setPen(Qt::NoPen);
    if (m_type == Save) {
        p.setBrush(QColor(50, 180, 70));          // 绿色底
    } else {
        p.setBrush(QColor(220, 50, 50));          // 红色底
    }
    p.drawRoundedRect(rect(), 6, 6);

    // 图标文字居中
    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 16, QFont::Bold));
    p.drawText(rect(), Qt::AlignCenter,
                m_type == Save ? "\u2713" : "\u2715");   // ✓ 或 ✕
}
