#include "selectionwidget.h"
#include <QPainter>
#include <QMouseEvent>

SelectionWidget::SelectionWidget(QWidget* parent)
  : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);  // 本身不消费鼠标事件
    hide();                                           // 初始隐藏
    parent->installEventFilter(this);                 // 拦截父窗口鼠标事件
    parent->setMouseTracking(true);                   // 父窗口启用鼠标追踪
}

// ================================================================
// eventFilter — 把父窗口的鼠标事件路由到内部处理方法
// ================================================================
bool SelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
        parentMousePressEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseMove)
        parentMouseMoveEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseButtonRelease)
        parentMouseReleaseEvent(static_cast<QMouseEvent*>(event));

    return false; // 不消费事件，让父窗口也能响应（比如确认按钮点击）
}

// ================================================================
// 绘制：选区边框（红色=拖拽中 / 绿色=已确认）+ 确认后的句柄
// ================================================================
void SelectionWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setPen(QPen(m_state == State::Confirmed ? Qt::green : Qt::red, 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect());

    if (m_state == State::Confirmed)
        m_handles.paint(p);              // 确认后绘制八向句柄
}

// ================================================================
// 鼠标按下
// ================================================================
void SelectionWidget::parentMousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    QPoint pos = e->pos();

    // 已确认状态：尝试拖拽句柄
    if (m_state == State::Confirmed) {
        m_selectionBeforeDrag = m_selection;
        m_dragStart = pos;
        m_activeHandle = m_handles.hitTest(mapFromParent(pos));
        return;
    }

    // 空闲/绘制状态：开始新的选区拖拽
    beginDrawing(pos);
}

void SelectionWidget::beginDrawing(QPoint pos)
{
    m_state = State::Drawing;
    m_startPoint = pos;
    m_selection = QRect(m_startPoint, QSize(0, 0));
    setGeometry(m_selection);
    show();
    emit selectionStarted();
}

// ================================================================
// 鼠标移动 — 拖拽选区 / 缩放句柄
// ================================================================
void SelectionWidget::parentMouseMoveEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();

    // 已确认 + 拖拽句柄 → 缩放选区
    if (m_state == State::Confirmed) {
        if (m_activeHandle != SelectionHandles::None) {
            QPoint delta = pos - m_dragStart;
            m_selection = m_handles.applyResize(
                m_selectionBeforeDrag, m_activeHandle, delta);
            m_handles.updateLayout(rect());
            setGeometry(m_selection);
            emit selectionChanged(m_selection);
        } else {
            updateCursor(e);            // 未拖拽 → 仅切换光标样式
        }
        return;
    }

    // 绘制状态 → 实时更新选区大小
    if (m_state == State::Drawing) {
        m_selection = QRect(m_startPoint, pos).normalized();
        m_handles.updateLayout(m_selection);
        setGeometry(m_selection);
        emit selectionChanged(m_selection);
    }
}

// ================================================================
// 鼠标释放 — 确认选区 / 取消
// ================================================================
void SelectionWidget::parentMouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    // 已确认状态：停止拖拽句柄
    if (m_state == State::Confirmed) {
        m_activeHandle = SelectionHandles::None;
        return;
    }

    // 绘制状态：选区边长 > 10px 才确认
    if (m_state == State::Drawing) {
        m_selection = m_selection.normalized();
        if (m_selection.width() > 10 && m_selection.height() > 10) {
            m_state = State::Confirmed;
            m_handles.updateLayout(rect());
            setGeometry(m_selection);
            emit selectionConfirmed();
        } else {
            m_state = State::Idle;
            hide();
            emit selectionCancelled();
        }
    }
}

// ================================================================
// 光标：根据鼠标所在句柄切换缩放箭头样式
// ================================================================
void SelectionWidget::updateCursor(QMouseEvent* e)
{
    auto h = m_handles.hitTest(mapFromParent(e->pos()));
    if (h != SelectionHandles::None)
        parentWidget()->setCursor(m_handles.cursorFor(h));
    else
        parentWidget()->setCursor(Qt::CrossCursor);
}
