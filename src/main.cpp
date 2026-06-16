#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QDateTime>
#include <QStandardPaths>
#include <QTimer>

// ============================================================
// CaptureOverlay — fullscreen region selector
// ============================================================
class CaptureOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureOverlay()
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        setCursor(Qt::CrossCursor);

        QScreen* screen = QApplication::primaryScreen();
        if (screen) {
            m_screenshot = screen->grabWindow(0);
            setGeometry(screen->geometry());
        }
    }

signals:
    void captureFinished(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, m_screenshot);

        // darkened non‑selection area
        if (m_hasSelection) {
            painter.setBrush(QColor(0, 0, 0, 120));
            painter.setPen(Qt::NoPen);
            QRegion full(rect());
            QRegion inner(m_selection);
            painter.setClipRegion(full.subtracted(inner));
            painter.drawRect(rect());
            painter.setClipRect(rect());

            // selection border
            painter.setPen(QPen(Qt::red, 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(m_selection);
        }

        // hint text
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(rect().adjusted(0, 0, 0, -20),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         "Drag to select a region  |  ESC to cancel");
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton) {
            m_startPoint = e->pos();
            m_hasSelection = true;
            m_selection = QRect(m_startPoint, QSize(0, 0));
        }
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        if (m_hasSelection) {
            m_selection = QRect(m_startPoint, e->pos()).normalized();
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton && m_hasSelection) {
            QRect sel = m_selection;
            if (sel.width() > 5 && sel.height() > 5) {
                emit captureFinished(m_screenshot.copy(sel));
            } else {
                emit captureFinished(QPixmap()); // cancelled
            }
        }
        close();
    }

    void keyPressEvent(QKeyEvent* e) override
    {
        if (e->key() == Qt::Key_Escape) {
            emit captureFinished(QPixmap());
            close();
        }
    }

private:
    QPixmap m_screenshot;
    QPoint m_startPoint;
    QRect m_selection;
    bool m_hasSelection = false;
};

// ============================================================
// MainWindow
// ============================================================
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow()
    {
        setWindowTitle("ZCapture");
        setFixedSize(460, 200);

        auto* lay = new QVBoxLayout(this);
        lay->setSpacing(12);
        lay->setContentsMargins(20, 15, 20, 15);

        auto* title = new QLabel("<h2>ZCapture</h2>");
        title->setAlignment(Qt::AlignCenter);
        lay->addWidget(title);

        // save path row
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

        // capture button
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

        // status label
        m_status = new QLabel("Ready");
        m_status->setAlignment(Qt::AlignCenter);
        m_status->setStyleSheet("color: gray;");
        lay->addWidget(m_status);
    }

private slots:
    void browsePath()
    {
        QString dir = QFileDialog::getExistingDirectory(this, "Save Directory",
                                                        m_pathEdit->text());
        if (!dir.isEmpty()) m_pathEdit->setText(dir);
    }

    void startCapture()
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

    void onCaptureFinished(const QPixmap& pixmap)
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

private:
    QLineEdit* m_pathEdit;
    QPushButton* m_captureBtn;
    QLabel* m_status;
    QString m_saveDir;
};

// ============================================================
#include "main.moc"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");
    app.setApplicationName("ZCapture");
    app.setOrganizationName("ZCapture");

    MainWindow window;
    window.show();

    return app.exec();
}
