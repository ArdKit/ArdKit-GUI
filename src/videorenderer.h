#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QMutex>

class VideoRenderer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)

public:
    explicit VideoRenderer(QQuickItem *parent = nullptr);
    ~VideoRenderer() override;

    void paint(QPainter *painter) override;

    bool isPlaying() const { return m_playing; }
    void setPlaying(bool playing);

public slots:
    // 更新视频帧
    void updateFrame(const QImage &frame);

    // 清除画面
    void clearFrame();

signals:
    void playingChanged();

private:
    QMutex m_frameMutex;
    QImage m_currentFrame;
    bool m_playing;
    bool m_hasFrame;
};

#endif // VIDEORENDERER_H
