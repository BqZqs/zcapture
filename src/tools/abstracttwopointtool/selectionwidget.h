/**
 * 选区控件（含八向句柄）。
 * 参照 Flameshot SelectionWidget 设计，通过 eventFilter 拦截父窗口鼠标事件。
 * 三态切换：Idle → Drawing → Confirmed。
 * 句柄绘制和缩放算法内聚在本类中。
 */
#pragma once

#include <QWidget>
#include <QPoint>
#include <QRect>

class SelectionWidget : public QWidget
{
    Q_OBJECT
public:
    enum class State { Idle, Drawing, Confirmed };
    enum Handle {
        None = -1,
        TopLeft, Top, TopRight, Right,
        BottomRight, Bottom, BottomLeft, Left
    };

    explicit SelectionWidget(QWidget* parent = nullptr);

    bool isConfirmed() const { return m_state == State::Confirmed; }
    QRect selection() const { return m_selection; }

signals:
    void selectionChanged(const QRect& rect);
    void selectionStarted();
    void selectionConfirmed();
    void selectionCancelled();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void paintEvent(QPaintEvent*) override;

private:
    void parentMousePressEvent(QMouseEvent* e);
    void parentMouseMoveEvent(QMouseEvent* e);
    void parentMouseReleaseEvent(QMouseEvent* e);
    void beginDrawing(QPoint pos);
    void updateCursor(QMouseEvent* e);

    // ---- 句柄（内聚实现） ----
    void updateHandleLayout();                              // 根据当前选区更新句柄坐标
    void paintHandles(QPainter& painter) const;             // 绘制八个白色句柄
    Handle hitTestHandle(QPoint pos) const;                 // 判断鼠标命中了哪个句柄
    QRect applyResize(const QRect& original, Handle h, QPoint delta) const;
    static QCursor cursorForHandle(Handle h);               // 句柄对应的鼠标样式

    QRect m_selection;
    QRect m_selectionBeforeDrag;
    QPoint m_startPoint;
    State m_state = State::Idle;
    Handle m_activeHandle = None;
    QPoint m_dragStart;
    QRect m_handleRects[8];                                 // 八个句柄矩形
};
