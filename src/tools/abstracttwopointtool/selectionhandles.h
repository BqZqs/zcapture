/**
 * 八向缩放句柄。
 * 提供选区确认后的八个白色小方块（四角 + 四边中点），
 * 用于拖拽调整选区大小。属于两点定形工具的辅助类。
 */
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

    void updateLayout(const QRect& selection);         // 根据选区位置重新计算八个句柄坐标
    void paint(QPainter& painter) const;                // 绘制八个白色句柄方块
    Handle hitTest(QPoint pos) const;                   // 判断鼠标点击了哪个句柄
    QCursor cursorFor(Handle h) const;                  // 返回该句柄对应的鼠标样式
    QRect applyResize(const QRect& original, Handle h, QPoint delta) const;
    // 根据拖拽方向和偏移量计算缩放后的新选区

private:
    QRect m_handles[8];      // 八个句柄的矩形区域
};
