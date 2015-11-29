#ifndef CLIPPER_H
#define CLIPPER_H

#include <QSize>
#include <QPixmap>

class Clipper
{
public:
    Clipper(const QSize &_size, const int _wspace, const int _hspace);

    QImage clip(const QImage &_image) const;
protected:
    QRect getImageRect(const QImage &_image) const;
    QImage cutImage(const QImage &_image, QRect _rect) const;
    QRect scaleRect(const double _scale, const QRect &_rect) const;

private:
    QSize m_size;
    QSize m_sizeWithSpaces;
};

#endif // CLIPPER_H
