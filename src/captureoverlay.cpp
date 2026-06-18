#include "captureoverlay.h"
#include "widgets/capture/selectionwidget.h"
#include "widgets/capture/capturetoolbutton.h"
#include "tools/abstractactiontool/captureactions.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>

CaptureOverlay::CaptureOverlay()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setMouseTracking(true);

    // 抓取主屏
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        m_screenshot = screen->grabWindow(0);
        setGeometry(screen->geometry());
    }

    // 选区控件（eventFilter 拦截鼠标事件）
    m_selectionWidget = new SelectionWidget(this);
    connect(m_selectionWidget, &SelectionWidget::selectionChanged,
            this, &CaptureOverlay::onSelectionChanged);
    connect(m_selectionWidget, &SelectionWidget::selectionConfirmed,
            this, &CaptureOverlay::onSelectionConfirmed);

    auto* cancelBtn = new CaptureToolButton(CaptureToolButton::Cancel, this);
    auto* saveBtn   = new CaptureToolButton(CaptureToolButton::Save, this);
    connect(cancelBtn, &CaptureToolButton::clicked, this, [this]() {
        m_actions->executeCancel();
    });
    connect(saveBtn, &CaptureToolButton::clicked, this, [this]() {
        m_actions->executeSave(m_screenshot, m_selectionWidget->selection());
    });
    m_buttonHandler.setButtons({ cancelBtn, saveBtn });

    // 动作逻辑（裁剪截图 → 发射信号）
    m_actions = new CaptureActions(this);
    connect(m_actions, &CaptureActions::finished,
            this, &CaptureOverlay::onActionFinished);

    QApplication::setOverrideCursor(Qt::CrossCursor);
}

// ================================================================
// 绘制
// ================================================================
void CaptureOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_screenshot);

    if (m_selectionWidget->isVisible()) {
        QRect sel = m_selectionWidget->selection();

        // 选区外暗化遮罩
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        QRegion full(rect());
        QRegion inner(sel);
        painter.setClipRegion(full.subtracted(inner));
        painter.drawRect(rect());
        painter.setClipRect(rect());
    }

    if (!m_selectionWidget->isConfirmed()) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(rect().adjusted(0, 0, 0, -20),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         "Drag to select a region  |  ESC to cancel");
    }
}

// ================================================================
// 键盘
// ================================================================
void CaptureOverlay::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
        m_actions->executeCancel();
    if (m_selectionWidget->isConfirmed() &&
        (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
        m_actions->executeSave(m_screenshot, m_selectionWidget->selection());
}

// ================================================================
// 信号槽
// ================================================================
void CaptureOverlay::onSelectionChanged(const QRect& /*rect*/)
{
    update();
}

void CaptureOverlay::onSelectionConfirmed()
{
    m_buttonHandler.updatePosition(m_selectionWidget->selection());
    m_buttonHandler.show();
    update();
}

void CaptureOverlay::onActionFinished(const QPixmap& result)
{
    QApplication::restoreOverrideCursor();
    emit captureFinished(result);
    close();
}
