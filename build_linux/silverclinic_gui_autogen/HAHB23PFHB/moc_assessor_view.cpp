/****************************************************************************
** Meta object code from reading C++ file 'assessor_view.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/gui/assessor/assessor_view.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'assessor_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_AssessorView_t {
    uint offsetsAndSizes[24];
    char stringdata0[13];
    char stringdata1[6];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[9];
    char stringdata5[9];
    char stringdata6[19];
    char stringdata7[12];
    char stringdata8[6];
    char stringdata9[16];
    char stringdata10[20];
    char stringdata11[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_AssessorView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_AssessorView_t qt_meta_stringdata_AssessorView = {
    {
        QT_MOC_LITERAL(0, 12),  // "AssessorView"
        QT_MOC_LITERAL(13, 5),  // "onAdd"
        QT_MOC_LITERAL(19, 0),  // ""
        QT_MOC_LITERAL(20, 6),  // "onEdit"
        QT_MOC_LITERAL(27, 8),  // "onDelete"
        QT_MOC_LITERAL(36, 8),  // "onImport"
        QT_MOC_LITERAL(45, 18),  // "onRowDoubleClicked"
        QT_MOC_LITERAL(64, 11),  // "QModelIndex"
        QT_MOC_LITERAL(76, 5),  // "index"
        QT_MOC_LITERAL(82, 15),  // "onFilterChanged"
        QT_MOC_LITERAL(98, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(118, 4)   // "text"
    },
    "AssessorView",
    "onAdd",
    "",
    "onEdit",
    "onDelete",
    "onImport",
    "onRowDoubleClicked",
    "QModelIndex",
    "index",
    "onFilterChanged",
    "onSearchTextChanged",
    "text"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_AssessorView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    0,   57,    2, 0x08,    2 /* Private */,
       4,    0,   58,    2, 0x08,    3 /* Private */,
       5,    0,   59,    2, 0x08,    4 /* Private */,
       6,    1,   60,    2, 0x08,    5 /* Private */,
       9,    1,   63,    2, 0x08,    7 /* Private */,
      10,    1,   66,    2, 0x08,    9 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::QString,   11,

       0        // eod
};

Q_CONSTINIT const QMetaObject AssessorView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_AssessorView.offsetsAndSizes,
    qt_meta_data_AssessorView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_AssessorView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<AssessorView, std::true_type>,
        // method 'onAdd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEdit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDelete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImport'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRowDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'onFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void AssessorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AssessorView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAdd(); break;
        case 1: _t->onEdit(); break;
        case 2: _t->onDelete(); break;
        case 3: _t->onImport(); break;
        case 4: _t->onRowDoubleClicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 5: _t->onFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *AssessorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AssessorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AssessorView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AssessorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
