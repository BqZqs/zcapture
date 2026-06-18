/**
 * 主窗口。
 * 提供保存路径选择器 + 截图按钮 + 状态提示。
 * 点击截图按钮后隐藏自身，弹出全屏 CaptureOverlay 等待用户选区。
 * 截图完成后保存为带时间戳的 PNG 文件。
 */
#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void browsePath();                                   // 弹出目录选择对话框
    void startCapture();                                 // 隐藏主窗口，弹出截图覆盖层
    void onCaptureFinished(const QPixmap& pixmap);       // CaptureOverlay 完成后的回调

private:
    QLineEdit* m_pathEdit;           // 保存路径输入框
    QPushButton* m_captureBtn;       // 截图按钮
    QLabel* m_status;                // 状态提示文字
    QString m_saveDir;               // 截图时刻确认的保存目录
};
