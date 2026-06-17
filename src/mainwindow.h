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
    void browsePath();
    void startCapture();
    void onCaptureFinished(const QPixmap& pixmap);

private:
    QLineEdit* m_pathEdit;
    QPushButton* m_captureBtn;
    QLabel* m_status;
    QString m_saveDir;
};
