/**
 * 全屏截图覆盖层。
 * 抓取主屏图像后覆盖整个屏幕。用户拖拽选区，
 * 确认后显示 ✓/✕ 按钮。选区交互委托给 SelectionWidget（eventFilter 模式）。
 * 完成或取消时发射 captureFinished 信号。
 */
#pragma once

#include <QWidget>
#include <QPixmap>
#include "tools/abstractactiontool/captureactionbar.h"

class SelectionWidget;

class CaptureOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureOverlay();

signals:
    void captureFinished(const QPixmap& pixmap);   // 有效图像 = 保存，空 = 取消

protected:
    void paintEvent(QPaintEvent*) override;          // 画截图 + 暗化遮罩 + 提示文字
    void mousePressEvent(QMouseEvent*) override;     // 仅处理确认按钮点击
    void keyPressEvent(QKeyEvent*) override;         // ESC 取消 / Enter 确认

private slots:
    void onSelectionChanged(const QRect& rect);       // 选区变化 → 重绘遮罩
    void onSelectionConfirmed();                       // 选区确认 → 计算按钮位置

private:
    QPixmap m_screenshot;               // 截取的全屏图像
    SelectionWidget* m_selectionWidget;  // 选区子控件（eventFilter 拦截鼠标）
    CaptureActionBar m_actionBar;        // ✓/✕ 按钮条
};
