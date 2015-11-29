#ifndef VIEWER3D_LOADER_H
#define VIEWER3D_LOADER_H

#include "abstractviewloader.h"
class QPointF;

class Viewer3ds;

class Viewer3dLoader : public AbstractViewLoader
{
    Q_OBJECT
public:
    Viewer3dLoader(QObject *parent = 0);

    EditItem::E_MODE mode() const;
    void setMode(EditItem::E_MODE _mode);

    void load(const QString& _dynamicTable, const QString& _valuesPrimaryKey);
    void load(const QByteArray& _model, const QByteArray &_texture);
    void loadAll(const QString &_model, const QString &_texture);

    QWidget* createWidget() const;

signals:
    void updateModel  (const QString& _fileName);
    void updateTexture(const QString& _fileName);

    void sgnUpdateModel(const QByteArray &_data);
    void sgnUpdateTexture(const QByteArray &_data);

private slots:
    void loadFile();
    void loadTexture();
private slots:
    void showMenu(Viewer3ds *_viewer, const QPoint &_globalPos);
    void requiestTexture(Viewer3ds *_viewer, const QString& _texturePath);
    void showPopup();
    void showDialog(QWidget* _wdg, const QString& _title);
    void clearModel();
    void saveFile();
    void saveFileTexture();

private:
    QWidget* createView() const;
    int codeMark(const QString &_dynamicTable, const QString &_valuesPrimaryKey);
    QString modelFileNameInBase(int _codeMark) const;
    QString textureFileNameInBase(int _codeMark) const;
    void setFileModel(const QString &name);
    void setFileTexture(const QString &name);
    void updateFile(const QString &srcFile, const QString &dstFile) const;

private:
    EditItem::E_MODE m_mode;
    QString m_fileName;
    QString m_fileTexture;
    int m_codeMark;
};

 //ise

#endif // VIEWER3D_LOADER_H
