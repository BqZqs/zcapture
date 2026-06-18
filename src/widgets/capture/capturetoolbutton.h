/**
 * 工具按钮。
 * QWidget 子类，负责确认/取消按钮自身的绘制。
 * 不处理点击逻辑——点击判断由 ButtonHandler::hitTest 负责，
 * 点击后的行为由 tools/ 中的动作工具类负责。
 */
#pragma once

#include <QWidget>

class CaptureToolButton : public QWidget
{
    Q_OBJECT
public:
    // 按钮类型：目前只有确认和取消
    enum Type { Save, Cancel };

    explicit CaptureToolButton(Type type, QWidget* parent = nullptr);

    Type toolType() const { return m_type; }

protected:
    void paintEvent(QPaintEvent*) override;   // 自绘圆角矩形 + 图标文字

private:
    Type m_type;
};
