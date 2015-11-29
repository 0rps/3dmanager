#include <QIODevice>

#include <lib3ds/io.h>
#include <lib3ds/file.h>

#include "wrapperqiodevice.h"

namespace lib3ds {


static Lib3dsBool fileIoErrorFunc(void *self)
{
  QIODevice *dev = reinterpret_cast<QIODevice*>(self);
  return !dev || !dev->isOpen();
}

static long fileIoSeekFunc(void *self, long offset, Lib3dsIoSeek origin)
{
  QIODevice *dev = reinterpret_cast<QIODevice*>(self);
  qint64 resultPos = dev->pos();

  switch (origin) {
    case LIB3DS_SEEK_SET:
      resultPos = offset;
      break;
    case LIB3DS_SEEK_CUR:
      resultPos = resultPos + offset;
      break;
    case LIB3DS_SEEK_END:
      resultPos = dev->size() - offset;
      break;
    default:
      ASSERT(0);
      return(0);
  }
  return dev->seek(resultPos);
}

static long fileIoTellFunc(void *self)
{
  QIODevice *dev = reinterpret_cast<QIODevice*>(self);
  return dev->pos();
}

static size_t fileIoReadFunc(void *self, void *buffer, size_t size)
{
  QIODevice *dev = reinterpret_cast<QIODevice*>(self);
  return dev->read((char*)buffer, size);
}

static size_t fileIoWriteFunc(void *self, const void *buffer, size_t size)
{
  QIODevice *dev = reinterpret_cast<QIODevice*>(self);
  return dev->write((char*)buffer, size);
}
//*************************************************************

Lib3dsFile* lib3ds_iodevice_load(QIODevice& _ioDevice)
{
    if(!_ioDevice.open(QIODevice::ReadOnly))
        return 0;

    Lib3dsFile* file = lib3ds_file_new();
    if (!file)
      return 0;

    Lib3dsIo * io = lib3ds_io_new(&_ioDevice,
                       fileIoErrorFunc,
                       fileIoSeekFunc,
                       fileIoTellFunc,
                       fileIoReadFunc,
                       fileIoWriteFunc);

    if(!io)
    {
        qWarning("Error : Unable to create IO ");
        lib3ds_file_free(file);
        return 0;
    }

    if (!lib3ds_file_read(file, io))
    {
        qWarning("Error : Unable to read ");
        lib3ds_file_free(file);
        lib3ds_io_free(io);
        return 0;
    }

    return file;
}
} //ns
