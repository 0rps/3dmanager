#include "model.h"

#include <QBuffer>
#include <QDataStream>

Model::Model():
    m_id(-1),
    m_armyId(-1),
    m_isBug(false),
    m_unitedId(-1)
{
}

Model::Model(const int _id)
{
    m_id = _id;
}

int Model::id() const
{
    return m_id;
}

void Model::setId(const int _id)
{
    m_id =_id;
}

bool Model::isValid() const
{
    return m_id > 0;
}

long Model::unitedId() const
{
    return m_unitedId;
}

void Model::setUnitedId(const long _id)
{
    m_unitedId = _id;
}


bool Model::isBug() const
{
    return m_isBug;
}

void Model::setBug(const bool _isBug)
{
    m_isBug = _isBug;
}

QString Model::name() const
{
    return m_name;
}


bool Model::inClassifier() const
{
    return m_unitedId > 0;
}

bool Model::isShow() const
{
    return m_isShow;
}

void Model::setShow(const bool _isShow)
{
    m_isShow = _isShow;
}


void Model::setName(const QString &_name)
{
    m_name = _name;
}

void Model::setImage(const QByteArray &_array)
{
    m_image.loadFromData(_array);
}

QImage Model::image() const
{
    return m_image;
}

QByteArray Model::imageData() const
{
    QByteArray p_data;

    QBuffer p_buffer(&p_data);
    p_buffer.open(QBuffer::WriteOnly);
    QDataStream p_stream(&p_buffer);
    p_stream << image;
    p_buffer.close();

    return p_data;
}

bool Model::hasImage() const
{
    return false == m_image.isNull();
}

void Model::setImage(const QImage &_image)
{
    m_image = _image;
}

int Model::armyId() const
{
    return m_armyId;
}

bool Model::hasArmy() const
{
    return m_armyId > 0;
}

void Model::setArmy(const int _id)
{
    m_armyId = _id;
}

bool Model::hasMax() const
{
    return false == m_max.isEmpty();
}

void Model::setMax(const QByteArray &_array)
{
    m_max = _array;
}

QByteArray Model::max() const
{
    return m_max;
}

bool Model::hasTexture() const
{
    return false == m_texture.isEmpty() ;
}

void Model::setTexture(const QByteArray &_array)
{
    return m_texture = _array;
}

QByteArray Model::texture() const
{
    return m_texture;
}

bool Model::has3ds() const
{
    return false == m_3ds.isEmpty();
}

void Model::set3ds(const QByteArray &_array)
{
    m_3ds = _array;
}

QByteArray Model::trids() const
{
    return m_3ds;
}

bool Model::addToAlbum() const
{
}


void Model::setAddToAlbum(const bool _isAdd) const
{
}
