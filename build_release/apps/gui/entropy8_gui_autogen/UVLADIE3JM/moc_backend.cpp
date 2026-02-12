/****************************************************************************
** Meta object code from reading C++ file 'backend.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/gui/src/backend.hpp"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'backend.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14ArchiveBackendE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN14ArchiveBackendE = QtMocHelpers::stringData(
    "ArchiveBackend",
    "extractionFinished",
    "",
    "success",
    "message",
    "formatIndexChanged",
    "methodIndexChanged",
    "codecIndexChanged",
    "splitValueChanged",
    "passwordChanged",
    "passwordRepeatChanged",
    "encryptFilenamesChanged",
    "solidArchiveChanged",
    "selfExtractingChanged",
    "verifyIntegrityChanged",
    "deleteAfterChanged",
    "archiveSeparatelyChanged",
    "archiveOpenChanged",
    "showViewerChanged",
    "archivePathChanged",
    "entriesChanged",
    "statusChanged",
    "handleDroppedUrls",
    "QList<QUrl>",
    "urls",
    "extractAll",
    "folderUrl",
    "formatSize",
    "bytes",
    "openFileFromPath",
    "filePath",
    "formats",
    "QVariantList",
    "formatIndex",
    "formatName",
    "formatColor",
    "formatSupported",
    "supportsEncryption",
    "methodIndex",
    "codecIndex",
    "splitValue",
    "password",
    "passwordRepeat",
    "encryptFilenames",
    "solidArchive",
    "selfExtracting",
    "verifyIntegrity",
    "deleteAfter",
    "archiveSeparately",
    "archiveOpen",
    "archiveEncrypted",
    "showViewer",
    "archivePath",
    "entries",
    "entryCount",
    "totalSize",
    "statusMessage",
    "statusError"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN14ArchiveBackendE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
      26,  180, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      18,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,  146,    2, 0x06,   27 /* Public */,
       5,    0,  151,    2, 0x06,   30 /* Public */,
       6,    0,  152,    2, 0x06,   31 /* Public */,
       7,    0,  153,    2, 0x06,   32 /* Public */,
       8,    0,  154,    2, 0x06,   33 /* Public */,
       9,    0,  155,    2, 0x06,   34 /* Public */,
      10,    0,  156,    2, 0x06,   35 /* Public */,
      11,    0,  157,    2, 0x06,   36 /* Public */,
      12,    0,  158,    2, 0x06,   37 /* Public */,
      13,    0,  159,    2, 0x06,   38 /* Public */,
      14,    0,  160,    2, 0x06,   39 /* Public */,
      15,    0,  161,    2, 0x06,   40 /* Public */,
      16,    0,  162,    2, 0x06,   41 /* Public */,
      17,    0,  163,    2, 0x06,   42 /* Public */,
      18,    0,  164,    2, 0x06,   43 /* Public */,
      19,    0,  165,    2, 0x06,   44 /* Public */,
      20,    0,  166,    2, 0x06,   45 /* Public */,
      21,    0,  167,    2, 0x06,   46 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      22,    1,  168,    2, 0x02,   47 /* Public */,
      25,    1,  171,    2, 0x02,   49 /* Public */,
      27,    1,  174,    2, 0x102,   51 /* Public | MethodIsConst  */,
      29,    1,  177,    2, 0x02,   53 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, QMetaType::QUrl,   26,
    QMetaType::QString, QMetaType::ULongLong,   28,
    QMetaType::Void, QMetaType::QString,   30,

 // properties: name, type, flags, notifyId, revision
      31, 0x80000000 | 32, 0x00015409, uint(-1), 0,
      33, QMetaType::Int, 0x00015103, uint(1), 0,
      34, QMetaType::QString, 0x00015001, uint(1), 0,
      35, QMetaType::QString, 0x00015001, uint(1), 0,
      36, QMetaType::Bool, 0x00015001, uint(1), 0,
      37, QMetaType::Bool, 0x00015001, uint(1), 0,
      38, QMetaType::Int, 0x00015103, uint(2), 0,
      39, QMetaType::Int, 0x00015103, uint(3), 0,
      40, QMetaType::QString, 0x00015103, uint(4), 0,
      41, QMetaType::QString, 0x00015103, uint(5), 0,
      42, QMetaType::QString, 0x00015103, uint(6), 0,
      43, QMetaType::Bool, 0x00015103, uint(7), 0,
      44, QMetaType::Bool, 0x00015103, uint(8), 0,
      45, QMetaType::Bool, 0x00015103, uint(9), 0,
      46, QMetaType::Bool, 0x00015103, uint(10), 0,
      47, QMetaType::Bool, 0x00015103, uint(11), 0,
      48, QMetaType::Bool, 0x00015103, uint(12), 0,
      49, QMetaType::Bool, 0x00015001, uint(13), 0,
      50, QMetaType::Bool, 0x00015001, uint(13), 0,
      51, QMetaType::Bool, 0x00015103, uint(14), 0,
      52, QMetaType::QString, 0x00015001, uint(15), 0,
      53, 0x80000000 | 32, 0x00015009, uint(16), 0,
      54, QMetaType::Int, 0x00015001, uint(16), 0,
      55, QMetaType::QString, 0x00015001, uint(16), 0,
      56, QMetaType::QString, 0x00015001, uint(17), 0,
      57, QMetaType::Bool, 0x00015001, uint(17), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject ArchiveBackend::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN14ArchiveBackendE.offsetsAndSizes,
    qt_meta_data_ZN14ArchiveBackendE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN14ArchiveBackendE_t,
        // property 'formats'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'formatIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'formatName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'formatColor'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'formatSupported'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'supportsEncryption'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'methodIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'codecIndex'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'splitValue'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'password'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'passwordRepeat'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'encryptFilenames'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'solidArchive'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'selfExtracting'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'verifyIntegrity'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'deleteAfter'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'archiveSeparately'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'archiveOpen'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'archiveEncrypted'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'showViewer'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'archivePath'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'entries'
        QtPrivate::TypeAndForceComplete<QVariantList, std::true_type>,
        // property 'entryCount'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'totalSize'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'statusMessage'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'statusError'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ArchiveBackend, std::true_type>,
        // method 'extractionFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'formatIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'methodIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'codecIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'splitValueChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'passwordChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'passwordRepeatChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'encryptFilenamesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'solidArchiveChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selfExtractingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'verifyIntegrityChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'deleteAfterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'archiveSeparatelyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'archiveOpenChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showViewerChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'archivePathChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'entriesChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDroppedUrls'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QList<QUrl> &, std::false_type>,
        // method 'extractAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'formatSize'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint64, std::false_type>,
        // method 'openFileFromPath'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void ArchiveBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ArchiveBackend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->extractionFinished((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->formatIndexChanged(); break;
        case 2: _t->methodIndexChanged(); break;
        case 3: _t->codecIndexChanged(); break;
        case 4: _t->splitValueChanged(); break;
        case 5: _t->passwordChanged(); break;
        case 6: _t->passwordRepeatChanged(); break;
        case 7: _t->encryptFilenamesChanged(); break;
        case 8: _t->solidArchiveChanged(); break;
        case 9: _t->selfExtractingChanged(); break;
        case 10: _t->verifyIntegrityChanged(); break;
        case 11: _t->deleteAfterChanged(); break;
        case 12: _t->archiveSeparatelyChanged(); break;
        case 13: _t->archiveOpenChanged(); break;
        case 14: _t->showViewerChanged(); break;
        case 15: _t->archivePathChanged(); break;
        case 16: _t->entriesChanged(); break;
        case 17: _t->statusChanged(); break;
        case 18: _t->handleDroppedUrls((*reinterpret_cast< std::add_pointer_t<QList<QUrl>>>(_a[1]))); break;
        case 19: _t->extractAll((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 20: { QString _r = _t->formatSize((*reinterpret_cast< std::add_pointer_t<quint64>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->openFileFromPath((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<QUrl> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (ArchiveBackend::*)(bool , const QString & );
            if (_q_method_type _q_method = &ArchiveBackend::extractionFinished; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::formatIndexChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::methodIndexChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::codecIndexChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::splitValueChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::passwordChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::passwordRepeatChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::encryptFilenamesChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::solidArchiveChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::selfExtractingChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::verifyIntegrityChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::deleteAfterChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::archiveSeparatelyChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::archiveOpenChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::showViewerChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::archivePathChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::entriesChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _q_method_type = void (ArchiveBackend::*)();
            if (_q_method_type _q_method = &ArchiveBackend::statusChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = _t->formats(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->formatIndex(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->formatName(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->formatColor(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->formatSupported(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->supportsEncryption(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->methodIndex(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->codecIndex(); break;
        case 8: *reinterpret_cast< QString*>(_v) = _t->splitValue(); break;
        case 9: *reinterpret_cast< QString*>(_v) = _t->password(); break;
        case 10: *reinterpret_cast< QString*>(_v) = _t->passwordRepeat(); break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->encryptFilenames(); break;
        case 12: *reinterpret_cast< bool*>(_v) = _t->solidArchive(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->selfExtracting(); break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->verifyIntegrity(); break;
        case 15: *reinterpret_cast< bool*>(_v) = _t->deleteAfter(); break;
        case 16: *reinterpret_cast< bool*>(_v) = _t->archiveSeparately(); break;
        case 17: *reinterpret_cast< bool*>(_v) = _t->archiveOpen(); break;
        case 18: *reinterpret_cast< bool*>(_v) = _t->archiveEncrypted(); break;
        case 19: *reinterpret_cast< bool*>(_v) = _t->showViewer(); break;
        case 20: *reinterpret_cast< QString*>(_v) = _t->archivePath(); break;
        case 21: *reinterpret_cast< QVariantList*>(_v) = _t->entries(); break;
        case 22: *reinterpret_cast< int*>(_v) = _t->entryCount(); break;
        case 23: *reinterpret_cast< QString*>(_v) = _t->totalSize(); break;
        case 24: *reinterpret_cast< QString*>(_v) = _t->statusMessage(); break;
        case 25: *reinterpret_cast< bool*>(_v) = _t->statusError(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setFormatIndex(*reinterpret_cast< int*>(_v)); break;
        case 6: _t->setMethodIndex(*reinterpret_cast< int*>(_v)); break;
        case 7: _t->setCodecIndex(*reinterpret_cast< int*>(_v)); break;
        case 8: _t->setSplitValue(*reinterpret_cast< QString*>(_v)); break;
        case 9: _t->setPassword(*reinterpret_cast< QString*>(_v)); break;
        case 10: _t->setPasswordRepeat(*reinterpret_cast< QString*>(_v)); break;
        case 11: _t->setEncryptFilenames(*reinterpret_cast< bool*>(_v)); break;
        case 12: _t->setSolidArchive(*reinterpret_cast< bool*>(_v)); break;
        case 13: _t->setSelfExtracting(*reinterpret_cast< bool*>(_v)); break;
        case 14: _t->setVerifyIntegrity(*reinterpret_cast< bool*>(_v)); break;
        case 15: _t->setDeleteAfter(*reinterpret_cast< bool*>(_v)); break;
        case 16: _t->setArchiveSeparately(*reinterpret_cast< bool*>(_v)); break;
        case 19: _t->setShowViewer(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *ArchiveBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ArchiveBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN14ArchiveBackendE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ArchiveBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
    return _id;
}

// SIGNAL 0
void ArchiveBackend::extractionFinished(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ArchiveBackend::formatIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ArchiveBackend::methodIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ArchiveBackend::codecIndexChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ArchiveBackend::splitValueChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ArchiveBackend::passwordChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void ArchiveBackend::passwordRepeatChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void ArchiveBackend::encryptFilenamesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void ArchiveBackend::solidArchiveChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void ArchiveBackend::selfExtractingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void ArchiveBackend::verifyIntegrityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void ArchiveBackend::deleteAfterChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void ArchiveBackend::archiveSeparatelyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void ArchiveBackend::archiveOpenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void ArchiveBackend::showViewerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void ArchiveBackend::archivePathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void ArchiveBackend::entriesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void ArchiveBackend::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}
QT_WARNING_POP
