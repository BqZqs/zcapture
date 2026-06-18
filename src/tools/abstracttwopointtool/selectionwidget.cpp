#include "selectionwidget.h"

#include <QPainter>
#include <QMouseEvent>

SelectionWidget::SelectionWidget(QWidget* parent)
  : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    hide();
    parent->installEventFilter(this);
    parent->setMouseTracking(true);
}

// ----------------------------------------------------------------
// event filter
// ----------------------------------------------------------------
bool SelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
        parentMousePressEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseMove)
        parentMouseMoveEvent(static_cast<QMouseEvent*>(event));
    else if (event->type() == QEvent::MouseButtonRelease)
        parentMouseReleaseEvent(static_cast<QMouseEvent*>(event));
    return false; // don't consume
}

// ----------------------------------------------------------------
// paint
// ----------------------------------------------------------------
void SelectionWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setPen(QPen(m_state == State::Confirmed ? Qt::green : Qt::red, 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect());

    if (m_state == State::Confirmed) {
        m_handles.paint(p);
    }
}

// ----------------------------------------------------------------
// parent mouse events
// ----------------------------------------------------------------
void SelectionWidget::parentMousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    QPoint pos = e->pos();

    if (m_state == State::Confirmed) {
        m_selectionBeforeDrag = m_selection;
        m_dragStart = pos;
        m_activeHandle = m_handles.hitTest(mapFromParent(pos));
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

void SelectionWidget::parentMouseMoveEvent(QMouseEvent* e)
{
    QPoint pos = e->pos();

    if (m_state == State::Confirmed) {
        if (m_activeHandle != SelectionHandles::None) {
            QPoint delta = pos - m_dragStart;
            m_selection = m_handles.applyResize(
                m_selectionBeforeDrag, m_activeHandle, delta);
            m_handles.updateLayout(rect());
            setGeometry(m_selection);
            emit selectionChanged(m_selection);
        } else {
            updateCursor(e);
        }
        return;
    }

    if (m_state == State::Drawing) {
        m_selection = QRect(m_startPoint, pos).normalized();
        m_handles.updateLayout(m_selection);
        setGeometry(m_selection);
        emit selectionChanged(m_selection);
    }
}

void SelectionWidget::parentMouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_state == State::Confirmed) {
        m_activeHandle = SelectionHandles::None;
        return;
    }

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

void SelectionWidget::updateCursor(QMouseEvent* e)
{
    auto h = m_handles.hitTest(mapFromParent(e->pos()));
    if (h != SelectionHandles::None)
        parentWidget()->setCursor(m_handles.cursorFor(h));
    else
        parentWidget()->setCursor(Qt::CrossCursor);
}
