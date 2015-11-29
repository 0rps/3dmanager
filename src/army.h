#ifndef ARMY_H
#define ARMY_H

#include <QString>
#include <QHash>

enum ArmyType
{
    AT_INVALID = -1,
    AT_UNKNOWN = 0,
    AT_SV = 1
};

extern QHash<int,QString> staticAllArmies;

class Army
{
public:
    Army();

    int id() const;
    bool isValid() const;
    void setId(const int _id);

    ArmyType type() const;
    bool isValidArmyType() const;
    void setArmyType(const ArmyType _type) const;

    QString name() const;
    void setName(const QString &_name);

    bool isRussian() const;
    void setRussian(const bool _isRussian);

    static Army getArmy(const int _typeId);
    static Army getArmy(const ArmyType _type);
    static void updateArmiesInternally();
private:
    int m_id;
    ArmyType m_type;
    bool m_isRussian;
    QString m_name;


    static QHash<ArmyType,Army> m_armies;
};

#endif // ARMY_H
