/**
 * 确认 / 取消按钮条。
 * 在选区确认后绘制两个圆角按钮：红色 ✕（取消）和绿色 ✓（保存）。
 * 属于抽象动作工具（不绘制图形，只触发操作），放在抽象动作工具目录下。
 */
#pragma once

#include <QRect>
#include <QPoint>

class QPainter;

enum class ActionButton { None, Save, Cancel };

class CaptureActionBar
{
public:
    void updateLayout(const QRect& selection);   // 根据选区位置计算按钮坐标
    void paint(QPainter& painter) const;          // 绘制两个按钮
    ActionButton hitTest(QPoint pos) const;       // 判断点击了哪个按钮

private:
    QRect m_saveBtn;       // 保存按钮矩形
    QRect m_cancelBtn;     // 取消按钮矩形
};
