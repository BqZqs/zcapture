#include "capturetoolbutton.h"
#include <QPainter>
#include <QFont>
#include <QMouseEvent>

CaptureToolButton::CaptureToolButton(Type type, QWidget* parent)
  : QWidget(parent)
  , m_type(type)
{
    setFixedSize(80, 36);
    setMouseTracking(true);
}

void CaptureToolButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(Qt::NoPen);
    if (m_type == Save)
        p.setBrush(QColor(50, 180, 70));
    else
        p.setBrush(QColor(220, 50, 50));
    p.drawRoundedRect(rect(), 6, 6);

    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 16, QFont::Bold));
    p.drawText(rect(), Qt::AlignCenter,
                m_type == Save ? "\u2713" : "\u2715");
}

void CaptureToolButton::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        emit clicked();
}
