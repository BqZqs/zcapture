#include "buttonhandler.h"
#include "capturetoolbutton.h"

void ButtonHandler::setButtons(const QVector<CaptureToolButton*>& buttons)
{
    for (auto* b : m_buttons)
        delete b;
    m_buttons = buttons;
}

void ButtonHandler::updatePosition(const QRect& selection)
{
    if (m_buttons.size() < 2) return;

    const int gap = 10;
    const int spacing = 12;
    const int bw = m_buttons[0]->width();
    const int bh = m_buttons[0]->height();

    int totalW = bw * 2 + spacing;
    int x = selection.center().x() - totalW / 2;
    int y = selection.bottom() + gap;

    m_buttons[0]->move(x, y);                       // 取消在左
    m_buttons[1]->move(x + bw + spacing, y);        // 确认在右
}

void ButtonHandler::show()
{
    for (auto* btn : m_buttons)
        btn->show();
}

void ButtonHandler::hide()
{
    for (auto* btn : m_buttons)
        btn->hide();
}
