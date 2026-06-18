#pragma once

#include <QWidget>
#include <QPoint>
#include <QRect>
#include <QCursor>

#include "selectionhandles.h"

class SelectionWidget : public QWidget
{
    Q_OBJECT
public:
    enum class State { Idle, Drawing, Confirmed };

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

    SelectionHandles m_handles;
    QPoint m_startPoint;
    QRect m_selection;
    QRect m_selectionBeforeDrag;
    State m_state = State::Idle;
    SelectionHandles::Handle m_activeHandle = SelectionHandles::None;
    QPoint m_dragStart;
};
