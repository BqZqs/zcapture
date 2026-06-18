/**
 * 全屏截图覆盖层。
 * 抓取主屏图像后覆盖整个屏幕。用户拖拽选区，
 * 确认后通过 ButtonHandler 显示按钮。选区交互由 SelectionWidget 接管。
 */
#pragma once

#include <QWidget>
#include <QPixmap>
#include "widgets/capture/buttonhandler.h"

class SelectionWidget;
class CaptureActions;

class CaptureOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureOverlay();

signals:
    void captureFinished(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;     // 仅处理按钮点击
    void keyPressEvent(QKeyEvent*) override;         // ESC / Enter

private slots:
    void onSelectionChanged(const QRect& rect);
    void onSelectionConfirmed();
    void onActionFinished(const QPixmap& result);     // CaptureActions 完成回调

private:
    QPixmap m_screenshot;
    SelectionWidget* m_selectionWidget;   // 选区控件（eventFilter 拦截鼠标）
    ButtonHandler m_buttonHandler;        // 确认/取消按钮管理
    CaptureActions* m_actions;            // 确认/取消的动作逻辑
};
