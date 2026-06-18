#include "captureactions.h"

CaptureActions::CaptureActions(QObject* parent)
  : QObject(parent)
{}

void CaptureActions::executeSave(const QPixmap& screenshot, const QRect& selection)
{
    emit finished(screenshot.copy(selection));   // 裁剪选区 → 发射信号
}

void CaptureActions::executeCancel()
{
    emit finished(QPixmap());                    // 空图 = 取消
}
