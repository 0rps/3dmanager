#include <QWidget>
#include <QDebug>
#include <QCursor>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDialog>
#include <QFileDialog>
#include <QMenu>
#include <QFileInfo>
#include <QFile>
#include <QSqlQuery>

#include "viewer3ds.h"
#include "viewer3dloader.h"

Viewer3dLoader::Viewer3dLoader(QObject *parent) :
    AbstractViewLoader(parent),
    m_mode(EditItem::ModeNothing),
    m_codeMark(-1)
{ }

EditItem::E_MODE Viewer3dLoader::mode() const
{
    return m_mode;
}

void Viewer3dLoader::updateFile(const QString& srcFile, const QString& dstFile) const
{
    if(srcFile == dstFile || dstFile.isEmpty())
        return;

    QFile::remove( dstFile );

    if(!srcFile.isEmpty())
        QFile::copy(srcFile, dstFile);
}

void Viewer3dLoader::setMode(EditItem::E_MODE _mode)
{
    qDebug() << Q_FUNC_INFO << _mode;
    m_mode = _mode;

    if(_mode ==  EditItem::ModeSave)
    {
        updateFile(m_fileName   , modelFileNameInBase(m_codeMark));
        updateFile(m_fileTexture, textureFileNameInBase(m_codeMark));
    }
}

QWidget *Viewer3dLoader::createWidget() const
{
    return createView();
}

void Viewer3dLoader::load(const QString &_dynamicTable, const QString &_valuesPrimaryKey)
{
    m_codeMark = codeMark(_dynamicTable, _valuesPrimaryKey);

    qDebug() << "LOAD!" << _dynamicTable << _valuesPrimaryKey << " codeMark=" << m_codeMark;

#if 1
    setFileModel  ( modelFileNameInBase(m_codeMark) );
    setFileTexture( textureFileNameInBase(m_codeMark) );
#else
    setFileModel  ( "t90.3ds" );
#endif
}

QString Viewer3dLoader::modelFileNameInBase(int _codeMark) const
{
    return "file:///:3dsmodel:" + QString::number(_codeMark);
}

QString Viewer3dLoader::textureFileNameInBase(int _codeMark) const
{
    return "file:///:3dstexture:" + QString::number(_codeMark);
}

int Viewer3dLoader::codeMark(const QString &_dynamicTable, const QString &_valuesPrimaryKey)
{
    QSqlQuery query(QString("select provide_el_united_id from f_provide_el_photo('%1', '%2');").arg(_dynamicTable).arg(_valuesPrimaryKey) );
    if(!query.next())
    {
        qWarning()  << "error! empty result of sql:" << query.lastQuery()  ;
        return -1;
    }

    return query.value(0).toInt();
}

QWidget *Viewer3dLoader::createView() const
{
    Viewer3ds* pView = new Viewer3ds();

    connect(this, SIGNAL(sgnUpdateModel(QByteArray)), pView, SLOT(loadFile(QByteArray)) );
    connect(this, SIGNAL(sgnUpdateTexture(QByteArray)), pView, SLOT(loadTextureFile(QByteArray)) );

    connect(this, SIGNAL(updateModel(const QString &)), pView, SLOT(loadFile(const QString &)));
    connect(this, SIGNAL(updateTexture(const QString &)), pView, SLOT(loadTextureFile(const QString &)));

    connect(pView, SIGNAL(sgnNeedContextMenu(Viewer3ds *, const QPoint&)), SLOT(showMenu(Viewer3ds *, const QPoint&)));
    //connect(pView, SIGNAL(sgnNeedTexture(Viewer3ds *, const QString&)), SLOT(requiestTexture(Viewer3ds *, const QString&)), Qt::QueuedConnection);

    pView->loadFile(m_fileName);
    pView->loadTextureFile(m_fileTexture);
    return pView;
}

void Viewer3dLoader::showMenu(Viewer3ds* _viewer, const QPoint& _globalPos)
{
    if(!_viewer)
        return;

    QMenu menu;

    bool isDialog = _viewer->parent() && (qobject_cast<QDialog*>(_viewer->parent()));
    if(!isDialog)
        menu.addAction( tr("Развернуть"), this, SLOT(showPopup()) );

    QAction* pActionLoad = menu.addAction( tr("Загрузить файл"), this, SLOT(loadFile()) );
    pActionLoad->setEnabled( mode() == EditItem::ModeEdit );

    QAction* pActionLoadTexture = menu.addAction( tr("Загрузить текстуру"), this, SLOT(loadTexture()) );
    pActionLoadTexture->setEnabled( mode() == EditItem::ModeEdit );

    QAction* pActionClear = menu.addAction( tr("Очистить"), this, SLOT(clearModel()) );
    pActionClear->setEnabled( mode() == EditItem::ModeEdit );

    menu.addAction( tr("Сохранить файл"), this, SLOT(saveFile()) );
    menu.addAction( tr("Сохранить текстуру"), this, SLOT(saveFileTexture()) );

    QAction* pActionPlay = menu.addAction( tr("Старт/стоп анимация") );
    pActionPlay->setCheckable(true);
    pActionPlay->setChecked(_viewer->isAnimated());
    connect(pActionPlay, SIGNAL(triggered(bool)), sender(), SLOT(setAnimated(bool)));

    QAction* pActionMode = menu.addAction( tr("Свободная/фиксированная камера") );
    pActionMode->setCheckable(true);
    pActionMode->setChecked(_viewer->isFreeMode());
    connect(pActionMode, SIGNAL(triggered(bool)), sender(), SLOT(setFreeMode(bool)));

    menu.exec( _globalPos );
}

void Viewer3dLoader::setFileModel(const QString& name)
{
    m_fileName = name;
    emit updateModel(name);
}

void Viewer3dLoader::setFileTexture(const QString& name)
{
    m_fileTexture = name;
    emit updateTexture(m_fileTexture);
}

void Viewer3dLoader::requiestTexture(Viewer3ds *_viewer, const QString& _texturePath)
{
    if(!_viewer || !m_fileTexture.isNull())
        return;

    qDebug() << " requiestTexture " << _texturePath;

    QString fileTexture = QFileInfo(m_fileName).path() + "/" + _texturePath;
    if(QFileInfo(fileTexture).exists())
    {
        setFileTexture(fileTexture);
        return;
    }

    m_fileTexture = ""; // fix duplicate questions on getOpenFileName
    QString name = QFileDialog::getOpenFileName(0, "Select a texture " + fileTexture, QFileInfo(fileTexture).path(), "image files (*.png *.bmp *.jpg *.jpeg);;All files (*)");
    if (name.isEmpty())
        return;

    setFileTexture(name);
}

void Viewer3dLoader::showPopup()
{
    showDialog( createView(), tr("Просмотр трехмерного изображения"));
}

void Viewer3dLoader::showDialog(QWidget *_wdg, const QString &_title)
{
    if(!_wdg)
        return;

    QVBoxLayout* pVerticalLayout = new QVBoxLayout();
    pVerticalLayout->setSpacing(2);
    pVerticalLayout ->setContentsMargins(1, 1, 0, 3);

    pVerticalLayout->addWidget(_wdg);

    QDialogButtonBox* pButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    pVerticalLayout->addWidget(pButtonBox);

    QDialog dialog;
    connect(pButtonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    dialog.setWindowTitle(_title);
    dialog.setLayout(pVerticalLayout);
    dialog.setMinimumSize( 450, 450 );
    dialog.exec();
}

void Viewer3dLoader::loadAll(const QString &_model, const QString &_texture)
{
    setFileModel(_model);
    setFileTexture(_texture);

}

void Viewer3dLoader::load(const QByteArray &_model, const QByteArray &_texture)
{
    sgnUpdateModel(_model);
    sgnUpdateTexture(_texture);
}

void Viewer3dLoader::loadFile()
{
    QString name = QFileDialog::getOpenFileName(0, "Select a 3ds model", ".", "3DS files (*.3ds *.3DS);;All files (*)");
    if (name.isEmpty())
        return;

    setFileTexture( QString() );
    setFileModel(name);
}

void Viewer3dLoader::loadTexture()
{
    QString name = QFileDialog::getOpenFileName(0, "Select a texture " , ".", "image files (*.png *.bmp *.jpg *.jpeg);;All files (*)");
    if (name.isEmpty())
        return;

    setFileTexture(name);
}

void Viewer3dLoader::clearModel()
{
    setFileModel("");
    setFileTexture("");
}

void Viewer3dLoader::saveFile()
{
    QString name = QFileDialog::getSaveFileName(0, "Save a 3ds model", ".", "3DS files (*.3ds *.3DS);;All files (*)");
    if (name.isEmpty())
        return;

    QFile::copy(m_fileName, name);
}

void Viewer3dLoader::saveFileTexture()
{
    QString name = QFileDialog::getSaveFileName(0, "Select a texture " , ".", "image files (*.png *.bmp *.jpg *.jpeg);;All files (*)");
    if (name.isEmpty())
        return;

    QFile::copy(m_fileTexture, name);
}

 // ns
