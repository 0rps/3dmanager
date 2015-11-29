#ifndef MODEL_H
#define MODEL_H

#include <QByteArray>
#include <QImage>
#include <QString>

class Model
{
public:
    Model(const int _id);
    Model();

    int id() const;
    void setId(const int _id);
    bool isValid() const;

    long unitedId() const;
    void setUnitedId(const long _id);
    bool inClassifier() const;

    bool isShow() const;
    void setShow(const bool _isShow);

    bool isBug() const;
    void setBug(const bool _isBug);

    QString name() const;
    void setName( const QString &_name);

    void setImage(const QByteArray &_array);
    QImage image() const;
    QByteArray imageData() const;
    bool hasImage() const;
    void setImage(const QImage &_image);

    int armyId() const;
    bool hasArmy() const;
    void setArmy(const int _id);

    bool hasMax() const;
    void setMax(const QByteArray &_array);
    QByteArray max() const;

    bool hasTexture() const;
    void setTexture(const QByteArray &_array);
    QByteArray texture() const;

    bool has3ds() const;
    void set3ds(const QByteArray &_array);
    QByteArray trids() const;

private:
    int m_id;
    int m_armyId;
    long m_unitedId;

    bool m_isBug;
    bool m_isShow;

    QString m_name;

    QImage m_image;

    QByteArray m_max;
    QByteArray m_3ds;
    QByteArray m_texture;
};

#endif // MODEL_H
