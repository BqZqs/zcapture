#include "captureoverlay.h"
#include "tools/abstracttwopointtool/selectionwidget.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>

CaptureOverlay::CaptureOverlay()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint); // 无边框置顶
    setMouseTracking(true);

    // 抓取主屏
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        m_screenshot = screen->grabWindow(0);
        setGeometry(screen->geometry());
    }

    // 选区控件：以 eventFilter 方式拦截本窗口的鼠标事件
    m_selectionWidget = new SelectionWidget(this);
    connect(m_selectionWidget, &SelectionWidget::selectionChanged,
            this, &CaptureOverlay::onSelectionChanged);
    connect(m_selectionWidget, &SelectionWidget::selectionConfirmed,
            this, &CaptureOverlay::onSelectionConfirmed);

    QApplication::setOverrideCursor(Qt::CrossCursor);   // 全局十字光标
}

// ================================================================
// 绘制：截图 → 遮罩 → 按钮 → 提示文字
// ================================================================
void CaptureOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_screenshot);         // 第 1 层：截图背景

    if (m_selectionWidget->isVisible()) {
        QRect sel = m_selectionWidget->selection();

        // 第 2 层：选区外半透明暗化遮罩
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        QRegion full(rect());
        QRegion inner(sel);
        painter.setClipRegion(full.subtracted(inner));
        painter.drawRect(rect());
        painter.setClipRect(rect());

        // 第 3 层：确认状态时绘制 ✓/✕ 按钮
        if (m_selectionWidget->isConfirmed()) {
            m_actionBar.paint(painter);
        }
    }

    // 未确认时在屏幕底部显示操作提示
    if (!m_selectionWidget->isConfirmed()) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(rect().adjusted(0, 0, 0, -20),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         "Drag to select a region  |  ESC to cancel");
    }
}

// ================================================================
// 鼠标：仅处理确认按钮区域的点击（选区拖拽由 SelectionWidget eventFilter 接管）
// ================================================================
void CaptureOverlay::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_selectionWidget->isConfirmed()) {
        ActionButton btn = m_actionBar.hitTest(e->pos());
        if (btn == ActionButton::Save) {
            QApplication::restoreOverrideCursor();
            emit captureFinished(m_screenshot.copy(m_selectionWidget->selection()));
            close();
        } else if (btn == ActionButton::Cancel) {
            QApplication::restoreOverrideCursor();
            emit captureFinished(QPixmap());             // 空图像 = 取消
            close();
        }
    }
}

// ================================================================
// 键盘：ESC = 取消  /  Enter = 保存
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
    update();   // 选区变化 → 重绘遮罩
}

void CaptureOverlay::onSelectionConfirmed()
{
    m_actionBar.updateLayout(m_selectionWidget->selection());
    update();
}
