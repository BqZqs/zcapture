#pragma once

#include <QRect>
#include <QPoint>

class QPainter;

enum class ActionButton { None, Save, Cancel };

class CaptureActionBar
{
public:
    void updateLayout(const QRect& selection);
    void paint(QPainter& painter) const;
    ActionButton hitTest(QPoint pos) const;

private:
    QRect m_saveBtn;
    QRect m_cancelBtn;
};
