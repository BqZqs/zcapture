/**
 * 工具按钮（QWidget 子类）。
 * 自绘圆角矩形 + 图标，点击时发射 clicked 信号。
 * 外观绘制在 widgets 层，动作逻辑在 tools/ 层。
 */
#pragma once

#include <QWidget>

class CaptureToolButton : public QWidget
{
    Q_OBJECT
public:
    enum Type { Save, Cancel };

    explicit CaptureToolButton(Type type, QWidget* parent = nullptr);
    Type toolType() const { return m_type; }

signals:
    void clicked();                         // 点击后由 tools/ 中的动作类处理

protected:
    void paintEvent(QPaintEvent*) override;  // 自绘圆角矩形 + 图标
    void mousePressEvent(QMouseEvent*) override;

private:
    Type m_type;
};
