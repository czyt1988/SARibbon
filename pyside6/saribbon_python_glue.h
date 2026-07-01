#ifndef SARIBBON_PYTHON_GLUE_H
#define SARIBBON_PYTHON_GLUE_H

#include <QWidget>
#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QString>
#include <sbkpython.h>
#include <shiboken.h>
#include <sbkconverter.h>

// Stub for PySide6 internal function not exported to custom bindings.
// The addAction(callable) overloads from QWidget will not be fully functional,
// but standard addAction(QAction*) works normally.
inline PyObject* addActionWithPyObject(QWidget* self, const QString& text, PyObject*)
{
    QAction* act = new QAction(text, self);
    self->addAction(act);
    PyObject* pyResult = Shiboken::Conversions::pointerToPython(
        reinterpret_cast<SbkObjectType*>(Shiboken::SbkType<QAction>()), act);
    return pyResult;
}

inline PyObject* addActionWithPyObject(QWidget* self, const QIcon& icon, const QString& text, PyObject*)
{
    QAction* act = new QAction(icon, text, self);
    self->addAction(act);
    PyObject* pyResult = Shiboken::Conversions::pointerToPython(
        reinterpret_cast<SbkObjectType*>(Shiboken::SbkType<QAction>()), act);
    return pyResult;
}

inline PyObject* addActionWithPyObject(QWidget* self, const QString& text, const QKeySequence& shortcut, PyObject*)
{
    QAction* act = new QAction(text, self);
    act->setShortcut(shortcut);
    self->addAction(act);
    PyObject* pyResult = Shiboken::Conversions::pointerToPython(
        reinterpret_cast<SbkObjectType*>(Shiboken::SbkType<QAction>()), act);
    return pyResult;
}

inline PyObject* addActionWithPyObject(QWidget* self, const QIcon& icon, const QString& text, const QKeySequence& shortcut, PyObject*)
{
    QAction* act = new QAction(icon, text, self);
    act->setShortcut(shortcut);
    self->addAction(act);
    PyObject* pyResult = Shiboken::Conversions::pointerToPython(
        reinterpret_cast<SbkObjectType*>(Shiboken::SbkType<QAction>()), act);
    return pyResult;
}

#endif // SARIBBON_PYTHON_GLUE_H
