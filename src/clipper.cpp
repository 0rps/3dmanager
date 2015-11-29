#include "clipper.h"

Clipper::Clipper(const QSize &_size, const int _wspace, const int _hspace):
    m_sizeWithSpaces(_size)
{
    m_size.setWidth(_size.width() - _wspace*2);
    m_size.setHeight(_size.height() - _hspace*2);
}

QImage Clipper::clip(const QImage &_image) const
{
    QRect p_rect = getImageRect(_image);
    QImage p_image;

    if ( p_rect.width() <= m_size.width() && p_rect.height() <= m_size.height() )
    {
        p_image = _image;
    } else if (p_rect.width() > m_size.width() && p_rect.height() > m_size.width())
    {
        double wcoeff = (double)p_rect.width() / (double)m_size.width();
        double hcoeff = (double)p_rect.height() / (double)m_size.height();

        if (wcoeff > hcoeff)
        {
            p_image = _image.scaledToWidth(_image.width()/wcoeff, Qt::SmoothTransformation);
            p_rect = scaleRect(wcoeff, p_rect);
        }
        else
        {
            p_image = _image.scaledToHeight(_image.height()/hcoeff, Qt::SmoothTransformation);
            p_rect = scaleRect(hcoeff, p_rect);
        }

    } else if (p_rect.width() > m_size.width())
    {
        double p_scale = (double)p_rect.width()/(double)m_size.width();
        p_image = _image.scaledToWidth(_image.height()/p_scale, Qt::SmoothTransformation);
        p_rect = scaleRect( p_scale, p_rect);
    } else if (p_rect.height() > m_size.height())
    {
        double p_scale = (double)p_rect.height()/(double)m_size.height();
        p_image = _image.scaledToHeight(_image.height()/p_scale, Qt::SmoothTransformation);
        p_rect = scaleRect( p_scale , p_rect);
    }

    return cutImage(p_image, p_rect);

}

QRect Clipper::getImageRect(const QImage &_image) const
{
    const QRgb p_back = _image.pixel(0,0);

    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;

    /// find top
    bool flag = true;
    for (int i = 0; i < _image.height() && flag; i++)
    {
        for (int j = 0; j < _image.width() && flag; j++)
        {
            if (_image.pixel(j,i) != p_back)
            {
                top = i;
                flag = false;
            }
        }
    }

    /// find bottom
    flag = true;
    for (int i = _image.height()-1; i >= top && flag; i--)
    {
        for (int j = 0; j < _image.width() && flag; j++)
        {
            if (_image.pixel(j,i) != p_back)
            {
                bottom = i;
                flag = false;
            }
        }
    }

    /// find left
    flag = true;
    for (int i = 0; i < _image.width() && flag; i++)
    {
        for (int j = top; j <= bottom && flag; j++)
        {
            if (_image.pixel(i,j) != p_back)
            {
                left = i;
                flag = false;
            }
        }
    }

    /// find right
    flag = true;
    for (int i = _image.width() - 1; i >= left && flag; i--)
    {
        for (int j = top; j <= bottom && flag; j++)
        {
            if (_image.pixel(i,j) != p_back)
            {
                right = i;
                flag = false;
            }
        }
    }

    return QRect(left,top, right-left, bottom-top);
}

QImage Clipper::cutImage(const QImage &_image, QRect _rect) const
{
    int deltaw = m_sizeWithSpaces.width() - _rect.width();
    int deltah = m_sizeWithSpaces.height() - _rect.height();

    _rect.setLeft(_rect.left() - deltaw / 2);
    _rect.setRight(_rect.right() + deltaw - deltaw/2);
    _rect.setTop(_rect.top() - deltah / 2);
    _rect.setBottom(_rect.bottom() + deltah - deltah / 2);

    return _image.copy(_rect);
}

QRect Clipper::scaleRect(const double _scale, const QRect &_rect) const
{
    QPoint p_topLeft;
    p_topLeft.setX(_rect.left() / _scale);
    p_topLeft.setY(_rect.top() / _scale);

    QSize p_size;
    p_size.setHeight(_rect.height()/_scale);
    p_size.setWidth(_rect.width() / _scale);

    return QRect(p_topLeft, p_size);
}
