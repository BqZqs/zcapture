/**
 * 选区控件。
 * 参照 Flameshot SelectionWidget 设计，通过 eventFilter 拦截父窗口鼠标事件。
 * 三态切换：Idle（空闲）→ Drawing（拖拽选区）→ Confirmed（确认，可缩放）。
 * 本身 WA_TransparentForMouseEvents，不消费事件，eventFilter 返回 false。
 */
#pragma once

#include <QWidget>
#include <QPoint>
#include <QRect>
#include "selectionhandles.h"

class SelectionWidget : public QWidget
{
    Q_OBJECT
public:
    // 三态：空闲 → 绘制中 → 已确认
    enum class State { Idle, Drawing, Confirmed };

    explicit SelectionWidget(QWidget* parent = nullptr);

    bool isConfirmed() const { return m_state == State::Confirmed; }
    QRect selection() const { return m_selection; }

signals:
    void selectionChanged(const QRect& rect);      // 选区变化（实时）
    void selectionStarted();                        // 开始拖拽选区
    void selectionConfirmed();                      // 选区确认（边长 > 10px）
    void selectionCancelled();                      // 选区太小，视为取消

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;   // 拦截父窗口鼠标事件
    void paintEvent(QPaintEvent*) override;                   // 画选区边框 + 句柄

private:
    void parentMousePressEvent(QMouseEvent* e);    // 拖拽开始 / 句柄命中
    void parentMouseMoveEvent(QMouseEvent* e);     // 拖拽缩放 / 实时更新
    void parentMouseReleaseEvent(QMouseEvent* e);  // 确认 / 取消
    void beginDrawing(QPoint pos);                  // 进入 Drawing 状态
    void updateCursor(QMouseEvent* e);              // 根据句柄切换光标样式

    SelectionHandles m_handles;                     // 八向句柄工具
    QPoint m_startPoint;                            // 初始拖拽起点
    QRect m_selection;                              // 当前选区
    QRect m_selectionBeforeDrag;                    // 拖拽前的选区（用于计算新尺寸）
    State m_state = State::Idle;
    SelectionHandles::Handle m_activeHandle = SelectionHandles::None;
    QPoint m_dragStart;                             // 拖拽起点（相对于父窗口）
};
