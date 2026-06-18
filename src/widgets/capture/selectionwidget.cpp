#include "selectionwidget.h"
#include <QPainter>
#include <QMouseEvent>

// ================================================================
// 构造
// ================================================================
SelectionWidget::SelectionWidget(QWidget* parent)
  : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    hide();
    parent->installEventFilter(this);
    parent->setMouseTracking(true);
}

// ================================================================
// eventFilter
// ================================================================
bool SelectionWidget::eventFilter(QObject*, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
        parentMousePressEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseMove)
        parentMouseMoveEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseButtonRelease)
        parentMouseReleaseEvent(static_cast<QMouseEvent*>(event));
    return false;
}

// ================================================================
// 绘制
// ================================================================
void SelectionWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setPen(QPen(m_state == State::Confirmed ? Qt::green : Qt::red, 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect());

    if (m_state == State::Confirmed)
        paintHandles(p);
}

// ================================================================
// 鼠标按下
// ================================================================
void SelectionWidget::parentMousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;
    QPoint pos = e->pos();

    if (m_state == State::Confirmed) {
        m_selectionBeforeDrag = m_selection;
        m_dragStart = pos;
        m_activeHandle = hitTestHandle(mapFromParent(pos));
        return;
    }
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
// 鼠标移动
// ================================================================
void SelectionWidget::parentMouseMoveEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();

    if (m_state == State::Confirmed) {
        if (m_activeHandle != None) {
            m_selection = applyResize(m_selectionBeforeDrag, m_activeHandle, pos - m_dragStart);
            updateHandleLayout();
            setGeometry(m_selection);
            emit selectionChanged(m_selection);
        } else {
            updateCursor(e);
        }
        return;
    }

    if (m_state == State::Drawing) {
        m_selection = QRect(m_startPoint, pos).normalized();
        updateHandleLayout();
        setGeometry(m_selection);
        emit selectionChanged(m_selection);
    }
}

// ================================================================
// 鼠标释放
// ================================================================
void SelectionWidget::parentMouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_state == State::Confirmed) {
        m_activeHandle = None;
        return;
    }

    if (m_state == State::Drawing) {
        m_selection = m_selection.normalized();
        if (m_selection.width() > 10 && m_selection.height() > 10) {
            m_state = State::Confirmed;
            updateHandleLayout();
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
// 句柄实现（内聚）
// ================================================================
void SelectionWidget::updateHandleLayout()
{
    const int hs = 8;
    const QRect& r = m_selection;
    int hhs = hs / 2;

    m_handleRects[TopLeft]     = QRect(r.left()  - hhs, r.top()    - hhs, hs, hs);
    m_handleRects[TopRight]    = QRect(r.right() - hhs, r.top()    - hhs, hs, hs);
    m_handleRects[BottomLeft]  = QRect(r.left()  - hhs, r.bottom() - hhs, hs, hs);
    m_handleRects[BottomRight] = QRect(r.right() - hhs, r.bottom() - hhs, hs, hs);

    m_handleRects[Top]    = QRect(r.center().x() - hhs, r.top()    - hhs, hs, hs);
    m_handleRects[Bottom] = QRect(r.center().x() - hhs, r.bottom() - hhs, hs, hs);
    m_handleRects[Left]   = QRect(r.left()  - hhs, r.center().y() - hhs, hs, hs);
    m_handleRects[Right]  = QRect(r.right() - hhs, r.center().y() - hhs, hs, hs);
}

void SelectionWidget::paintHandles(QPainter& painter) const
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    for (int i = 0; i < 8; ++i)
        painter.drawRect(m_handleRects[i]);
}

SelectionWidget::Handle SelectionWidget::hitTestHandle(QPoint pos) const
{
    for (int i = 0; i < 8; ++i)
        if (m_handleRects[i].contains(pos))
            return static_cast<Handle>(i);
    return None;
}

QCursor SelectionWidget::cursorForHandle(Handle h)
{
    switch (h) {
    case TopLeft:     case BottomRight: return Qt::SizeFDiagCursor;
    case TopRight:    case BottomLeft:  return Qt::SizeBDiagCursor;
    case Left:        case Right:       return Qt::SizeHorCursor;
    case Top:         case Bottom:      return Qt::SizeVerCursor;
    default:          return Qt::CrossCursor;
    }
}

QRect SelectionWidget::applyResize(const QRect& original, Handle h, QPoint delta) const
{
    const QRect& r = original;
    switch (h) {
    case TopLeft:     return QRect(r.topLeft() + delta, r.bottomRight()).normalized();
    case TopRight:    return QRect(QPoint(r.right() + delta.x(), r.top() + delta.y()),
                                   r.bottomLeft()).normalized();
    case BottomLeft:  return QRect(QPoint(r.left() + delta.x(), r.bottom() + delta.y()),
                                   r.topRight()).normalized();
    case BottomRight: return QRect(r.topLeft(), r.bottomRight() + delta).normalized();
    case Top:         return QRect(QPoint(r.left(), r.top() + delta.y()),
                                   r.bottomRight()).normalized();
    case Bottom:      return QRect(r.topLeft(),
                                   QPoint(r.right(), r.bottom() + delta.y())).normalized();
    case Left:        return QRect(QPoint(r.left() + delta.x(), r.top()),
                                   r.bottomRight()).normalized();
    case Right:       return QRect(r.topLeft(),
                                   QPoint(r.right() + delta.x(), r.bottom())).normalized();
    default:          return original;
    }
}

// ================================================================
// 光标
// ================================================================
void SelectionWidget::updateCursor(QMouseEvent* e)
{
    Handle h = hitTestHandle(mapFromParent(e->pos()));
    if (h != None)
        parentWidget()->setCursor(cursorForHandle(h));
    else
        parentWidget()->setCursor(Qt::CrossCursor);
}
