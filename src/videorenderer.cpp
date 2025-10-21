#include "videorenderer.h"
#include <QDebug>

VideoRenderer::VideoRenderer(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_playing(false)
    , m_hasFrame(false)
{
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setAntialiasing(true);

    qDebug() << "VideoRenderer created";
}

VideoRenderer::~VideoRenderer()
{
    qDebug() << "VideoRenderer destroyed";
}

void VideoRenderer::paint(QPainter *painter)
{
    QMutexLocker locker(&m_frameMutex);

    if (!m_hasFrame || m_currentFrame.isNull()) {
        // 绘制黑色背景
        painter->fillRect(boundingRect(), Qt::black);
        return;
    }

    // 计算缩放后的矩形（保持宽高比）
    QRectF targetRect = boundingRect();
    QSizeF imageSize = m_currentFrame.size();

    qreal scaleX = targetRect.width() / imageSize.width();
    qreal scaleY = targetRect.height() / imageSize.height();
    qreal scale = qMin(scaleX, scaleY);

    qreal scaledWidth = imageSize.width() * scale;
    qreal scaledHeight = imageSize.height() * scale;

    qreal x = (targetRect.width() - scaledWidth) / 2.0;
    qreal y = (targetRect.height() - scaledHeight) / 2.0;

    QRectF drawRect(x, y, scaledWidth, scaledHeight);

    // 填充黑色背景
    painter->fillRect(targetRect, Qt::black);

    // 绘制视频帧（保持宽高比，居中显示）
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->drawImage(drawRect, m_currentFrame);
}

void VideoRenderer::updateFrame(const QImage &frame)
{
    {
        QMutexLocker locker(&m_frameMutex);
        m_currentFrame = frame;
        m_hasFrame = true;
    }

    // 触发重绘
    update();
}

void VideoRenderer::clearFrame()
{
    {
        QMutexLocker locker(&m_frameMutex);
        m_currentFrame = QImage();
        m_hasFrame = false;
    }

    update();
}

void VideoRenderer::setPlaying(bool playing)
{
    if (m_playing != playing) {
        m_playing = playing;
        emit playingChanged();

        if (!m_playing) {
            clearFrame();
        }
    }
}
