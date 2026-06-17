#include "mainwindow.h"
#include "captureoverlay.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDateTime>
#include <QStandardPaths>
#include <QApplication>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
  : QWidget(parent)
{
    setWindowTitle("ZCapture");
    setFixedSize(460, 200);

    auto* lay = new QVBoxLayout(this);
    lay->setSpacing(12);
    lay->setContentsMargins(20, 15, 20, 15);

    auto* title = new QLabel("<h2>ZCapture</h2>");
    title->setAlignment(Qt::AlignCenter);
    lay->addWidget(title);

    auto* pathRow = new QHBoxLayout();
    pathRow->addWidget(new QLabel("Save Path:"));
    m_pathEdit = new QLineEdit();
    m_pathEdit->setText(
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    m_pathEdit->setPlaceholderText("Select a folder...");
    pathRow->addWidget(m_pathEdit, 1);
    auto* browseBtn = new QPushButton("Browse");
    browseBtn->setFixedWidth(72);
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::browsePath);
    pathRow->addWidget(browseBtn);
    lay->addLayout(pathRow);

    m_captureBtn = new QPushButton("Capture Screenshot");
    m_captureBtn->setFixedHeight(42);
    m_captureBtn->setStyleSheet(
        "QPushButton {"
        "  font-size: 14px; font-weight: bold;"
        "  background: #0078D4; color: white; border-radius: 6px;"
        "}"
        "QPushButton:hover  { background: #106EBE; }"
        "QPushButton:disabled { background: #A0A0A0; }");
    connect(m_captureBtn, &QPushButton::clicked, this, &MainWindow::startCapture);
    lay->addWidget(m_captureBtn);

    m_status = new QLabel("Ready");
    m_status->setAlignment(Qt::AlignCenter);
    m_status->setStyleSheet("color: gray;");
    lay->addWidget(m_status);
}

void MainWindow::browsePath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Save Directory",
                                                    m_pathEdit->text());
    if (!dir.isEmpty()) m_pathEdit->setText(dir);
}

void MainWindow::startCapture()
{
    m_saveDir = m_pathEdit->text().trimmed();
    if (m_saveDir.isEmpty()) {
        m_status->setText("Select a save path first!");
        m_status->setStyleSheet("color: red;");
        return;
    }
    hide();
    QApplication::processEvents();

    QTimer::singleShot(300, this, [this]() {
        auto* overlay = new CaptureOverlay();
        connect(overlay, &CaptureOverlay::captureFinished,
                this, &MainWindow::onCaptureFinished);
        overlay->showFullScreen();
    });
}

void MainWindow::onCaptureFinished(const QPixmap& pixmap)
{
    show();
    activateWindow();

    if (pixmap.isNull()) {
        m_status->setText("Capture cancelled");
        m_status->setStyleSheet("color: gray;");
        return;
    }

    QString file = m_saveDir + "/zcapture_" +
                   QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") +
                   ".png";
    if (pixmap.save(file, "PNG")) {
        m_status->setText("Saved: " + file);
        m_status->setStyleSheet("color: green;");
    } else {
        m_status->setText("Failed to save!");
        m_status->setStyleSheet("color: red;");
    }
}
