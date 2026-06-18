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
    void captureFinished(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private slots:
    void onSelectionChanged(const QRect& rect);
    void onSelectionConfirmed();

private:
    QPixmap m_screenshot;
    SelectionWidget* m_selectionWidget;
    CaptureActionBar m_actionBar;
};
