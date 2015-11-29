#ifndef WRAPPERQIODEVICE_H
#define WRAPPERQIODEVICE_H

class QIODevice;
struct Lib3dsFile;

namespace lib3ds {

Lib3dsFile* lib3ds_iodevice_load(QIODevice& _ioDevice);

} //ns

#endif // WRAPPERQIODEVICE_H
