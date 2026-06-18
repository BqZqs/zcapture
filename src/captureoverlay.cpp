#include "captureoverlay.h"
#include "widgets/capture/selectionwidget.h"
#include "widgets/capture/capturetoolbutton.h"

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

    // 确认/取消按钮
    auto* cancelBtn = new CaptureToolButton(CaptureToolButton::Cancel, this);
    auto* saveBtn   = new CaptureToolButton(CaptureToolButton::Save, this);
    m_buttonHandler.setButtons({ cancelBtn, saveBtn });

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
// 鼠标：仅处理按钮点击（选区拖拽由 SelectionWidget eventFilter 接管）
// ================================================================
void CaptureOverlay::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_selectionWidget->isConfirmed()) {
        auto* btn = m_buttonHandler.hitTest(e->pos());
        if (!btn) return;

        QApplication::restoreOverrideCursor();

        if (btn->toolType() == CaptureToolButton::Save) {
            emit captureFinished(m_screenshot.copy(m_selectionWidget->selection()));
        } else {
            emit captureFinished(QPixmap());       // 空图 = 取消
        }
        close();
    }
}

// ================================================================
// 键盘
// ================================================================
void CaptureOverlay::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        QApplication::restoreOverrideCursor();
        emit captureFinished(QPixmap());
        close();
    }
    if (m_selectionWidget->isConfirmed() &&
        (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {
        QApplication::restoreOverrideCursor();
        emit captureFinished(m_screenshot.copy(m_selectionWidget->selection()));
        close();
    }
}

// ================================================================
// SelectionWidget 信号槽
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
