/**
 * 按钮布局管理器。
 * 管理多个 CaptureToolButton 的位置排列。
 * 根据选区矩形自动计算每个按钮的坐标。
 * 按钮本身的点击由 CaptureToolButton 自行处理。
 */
#pragma once

#include <QVector>
#include <QRect>

class CaptureToolButton;

class ButtonHandler
{
public:
    void setButtons(const QVector<CaptureToolButton*>& buttons);
    void updatePosition(const QRect& selection);     // 根据选区重新布局
    void show();                                      // 显示所有按钮
    void hide();                                      // 隐藏所有按钮

private:
    QVector<CaptureToolButton*> m_buttons;
};
