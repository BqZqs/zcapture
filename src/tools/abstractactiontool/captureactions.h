/**
 * 确认 / 取消动作。
 * 纯逻辑类——不负责绘制，不负责布局，只决定点击按钮后"做什么"。
 * 属于抽象动作工具类，按 Flameshot 归属应放在抽象动作工具目录下。
 */
#pragma once

#include <QPixmap>
#include <QObject>

class CaptureActions : public QObject
{
    Q_OBJECT
public:
    explicit CaptureActions(QObject* parent = nullptr);

    /// 执行保存动作：裁剪指定区域并发射确认信号
    void executeSave(const QPixmap& screenshot, const QRect& selection);

    /// 执行取消动作：发射空图像信号
    void executeCancel();

signals:
    void finished(const QPixmap& result);   // 非空 = 保存，空 = 取消
};
