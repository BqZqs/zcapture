#include "buttonhandler.h"
#include "capturetoolbutton.h"

void ButtonHandler::setButtons(const QVector<CaptureToolButton*>& buttons)
{
    // 清理旧按钮
    for (auto* b : m_buttons)
        delete b;
    m_buttons = buttons;
}

void ButtonHandler::updatePosition(const QRect& selection)
{
    if (m_buttons.size() < 2) return;

    const int gap = 10;           // 按钮与选区底边的间距
    const int spacing = 12;       // 两按钮间的间距
    const int bw = m_buttons[0]->width();
    const int bh = m_buttons[0]->height();

    int totalW = bw * 2 + spacing;
    int x = selection.center().x() - totalW / 2;      // 整体居中于选区
    int y = selection.bottom() + gap;                  // 紧贴选区下方

    // 取消在左，确认在右
    m_buttons[0]->move(x, y);              // 取消按钮
    m_buttons[1]->move(x + bw + spacing, y); // 保存按钮
}

CaptureToolButton* ButtonHandler::hitTest(QPoint pos) const
{
    for (auto* btn : m_buttons)
        if (btn->geometry().contains(pos))
            return btn;
    return nullptr;
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
