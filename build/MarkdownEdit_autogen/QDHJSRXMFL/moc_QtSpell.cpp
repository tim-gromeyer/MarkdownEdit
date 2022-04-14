/****************************************************************************
** Meta object code from reading C++ file 'QtSpell.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../3rdparty/qtspell/src/QtSpell.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtSpell.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtSpell__Checker_t {
    QByteArrayData data[11];
    char stringdata0[136];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtSpell__Checker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtSpell__Checker_t qt_meta_stringdata_QtSpell__Checker = {
    {
QT_MOC_LITERAL(0, 0, 16), // "QtSpell::Checker"
QT_MOC_LITERAL(1, 17, 15), // "languageChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 7), // "newLang"
QT_MOC_LITERAL(4, 42, 18), // "setSpellingEnabled"
QT_MOC_LITERAL(5, 61, 7), // "enabled"
QT_MOC_LITERAL(6, 69, 11), // "slotAddWord"
QT_MOC_LITERAL(7, 81, 14), // "slotIgnoreWord"
QT_MOC_LITERAL(8, 96, 15), // "slotReplaceWord"
QT_MOC_LITERAL(9, 112, 15), // "slotSetLanguage"
QT_MOC_LITERAL(10, 128, 7) // "checked"

    },
    "QtSpell::Checker\0languageChanged\0\0"
    "newLang\0setSpellingEnabled\0enabled\0"
    "slotAddWord\0slotIgnoreWord\0slotReplaceWord\0"
    "slotSetLanguage\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtSpell__Checker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   47,    2, 0x0a /* Public */,
       6,    0,   50,    2, 0x08 /* Private */,
       7,    0,   51,    2, 0x08 /* Private */,
       8,    0,   52,    2, 0x08 /* Private */,
       9,    1,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,

       0        // eod
};

void QtSpell::Checker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Checker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->languageChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setSpellingEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->slotAddWord(); break;
        case 3: _t->slotIgnoreWord(); break;
        case 4: _t->slotReplaceWord(); break;
        case 5: _t->slotSetLanguage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Checker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Checker::languageChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtSpell::Checker::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_QtSpell__Checker.data,
    qt_meta_data_QtSpell__Checker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtSpell::Checker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtSpell::Checker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtSpell__Checker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QtSpell::Checker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QtSpell::Checker::languageChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_QtSpell__TextEditChecker_t {
    QByteArrayData data[15];
    char stringdata0[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtSpell__TextEditChecker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtSpell__TextEditChecker_t qt_meta_stringdata_QtSpell__TextEditChecker = {
    {
QT_MOC_LITERAL(0, 0, 24), // "QtSpell::TextEditChecker"
QT_MOC_LITERAL(1, 25, 13), // "undoAvailable"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 9), // "available"
QT_MOC_LITERAL(4, 50, 13), // "redoAvailable"
QT_MOC_LITERAL(5, 64, 4), // "undo"
QT_MOC_LITERAL(6, 69, 4), // "redo"
QT_MOC_LITERAL(7, 74, 13), // "clearUndoRedo"
QT_MOC_LITERAL(8, 88, 19), // "slotShowContextMenu"
QT_MOC_LITERAL(9, 108, 3), // "pos"
QT_MOC_LITERAL(10, 112, 24), // "slotCheckDocumentChanged"
QT_MOC_LITERAL(11, 137, 18), // "slotDetachTextEdit"
QT_MOC_LITERAL(12, 156, 14), // "slotCheckRange"
QT_MOC_LITERAL(13, 171, 7), // "removed"
QT_MOC_LITERAL(14, 179, 5) // "added"

    },
    "QtSpell::TextEditChecker\0undoAvailable\0"
    "\0available\0redoAvailable\0undo\0redo\0"
    "clearUndoRedo\0slotShowContextMenu\0pos\0"
    "slotCheckDocumentChanged\0slotDetachTextEdit\0"
    "slotCheckRange\0removed\0added"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtSpell__TextEditChecker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   65,    2, 0x0a /* Public */,
       6,    0,   66,    2, 0x0a /* Public */,
       7,    0,   67,    2, 0x0a /* Public */,
       8,    1,   68,    2, 0x08 /* Private */,
      10,    0,   71,    2, 0x08 /* Private */,
      11,    0,   72,    2, 0x08 /* Private */,
      12,    3,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    9,   13,   14,

       0        // eod
};

void QtSpell::TextEditChecker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TextEditChecker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->undoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->redoAvailable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->undo(); break;
        case 3: _t->redo(); break;
        case 4: _t->clearUndoRedo(); break;
        case 5: _t->slotShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 6: _t->slotCheckDocumentChanged(); break;
        case 7: _t->slotDetachTextEdit(); break;
        case 8: _t->slotCheckRange((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TextEditChecker::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextEditChecker::undoAvailable)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TextEditChecker::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextEditChecker::redoAvailable)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QtSpell::TextEditChecker::staticMetaObject = { {
    &Checker::staticMetaObject,
    qt_meta_stringdata_QtSpell__TextEditChecker.data,
    qt_meta_data_QtSpell__TextEditChecker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QtSpell::TextEditChecker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtSpell::TextEditChecker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QtSpell__TextEditChecker.stringdata0))
        return static_cast<void*>(this);
    return Checker::qt_metacast(_clname);
}

int QtSpell::TextEditChecker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Checker::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QtSpell::TextEditChecker::undoAvailable(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtSpell::TextEditChecker::redoAvailable(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
