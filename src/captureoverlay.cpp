#include "captureoverlay.h"
#include "tools/abstracttwopointtool/selectionwidget.h"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>

CaptureOverlay::CaptureOverlay()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setMouseTracking(true);

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        m_screenshot = screen->grabWindow(0);
        setGeometry(screen->geometry());
    }

    // selection widget (eventFilter on this overlay)
    m_selectionWidget = new SelectionWidget(this);
    connect(m_selectionWidget, &SelectionWidget::selectionChanged,
            this, &CaptureOverlay::onSelectionChanged);
    connect(m_selectionWidget, &SelectionWidget::selectionConfirmed,
            this, &CaptureOverlay::onSelectionConfirmed);

    QApplication::setOverrideCursor(Qt::CrossCursor);
}

// ----------------------------------------------------------------
// paint
// ----------------------------------------------------------------
void CaptureOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_screenshot);

    if (m_selectionWidget->isVisible()) {
        QRect sel = m_selectionWidget->selection();

        // darkened non-selection area
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        QRegion full(rect());
        QRegion inner(sel);
        painter.setClipRegion(full.subtracted(inner));
        painter.drawRect(rect());
        painter.setClipRect(rect());

        if (m_selectionWidget->isConfirmed()) {
            m_actionBar.paint(painter);
        }
    }

    if (!m_selectionWidget->isConfirmed()) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 14));
        painter.drawText(rect().adjusted(0, 0, 0, -20),
                         Qt::AlignHCenter | Qt::AlignBottom,
                         "Drag to select a region  |  ESC to cancel");
    }
}

// ----------------------------------------------------------------
// mouse (only action bar clicks)
// ----------------------------------------------------------------
void CaptureOverlay::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) return;

    if (m_selectionWidget->isConfirmed()) {
        ActionButton btn = m_actionBar.hitTest(e->pos());
        if (btn == ActionButton::Save) {
            QApplication::restoreOverrideCursor();
            emit captureFinished(
                m_screenshot.copy(m_selectionWidget->selection()));
            close();
        } else if (btn == ActionButton::Cancel) {
            QApplication::restoreOverrideCursor();
            emit captureFinished(QPixmap());
            close();
        }
    }
}

// ----------------------------------------------------------------
// keyboard
// ----------------------------------------------------------------
void CaptureOverlay::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) {
        QApplication::restoreOverrideCursor();
        emit captureFinished(QPixmap());
        close();
    }
    if (m_selectionWidget->isConfirmed() &&
        (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)) {
        QApplication::restoreOverrideCursor();
        emit captureFinished(
            m_screenshot.copy(m_selectionWidget->selection()));
        close();
    }
}

// ----------------------------------------------------------------
// slots
// ----------------------------------------------------------------
void CaptureOverlay::onSelectionChanged(const QRect& /*rect*/)
{
    update(); // repaint mask
}

void CaptureOverlay::onSelectionConfirmed()
{
    QRect sel = m_selectionWidget->selection();
    m_actionBar.updateLayout(sel);
    update();
}
