#include "ribbonbackend.h"

// The command half of the migration story: plain QAction wiring that a
// widgets application already has (icons land on the actions directly; the
// QML side renders them through the SARibbon image provider bridge).

RibbonBackend::RibbonBackend(QObject* parent) : QObject(parent)
{
    mSave.setObjectName("actionSave");
    mSave.setText(QObject::tr("Save"));
    mSave.setToolTip(QObject::tr("Save the document (Ctrl+S)"));
    mSave.setShortcut(QKeySequence("Ctrl+S"));
    mSave.setIcon(QIcon(":/icon/icon/save.svg"));
    QObject::connect(&mSave, &QAction::triggered, this, [this]() {
        ++mSaveCount;
        Q_EMIT saveCountChanged();
    });

    mUndo.setObjectName("actionUndo");
    mUndo.setText(QObject::tr("Undo"));
    mUndo.setToolTip(QObject::tr("Undo the last change (Ctrl+Z)"));
    mUndo.setShortcut(QKeySequence("Ctrl+Z"));
    mUndo.setIcon(QIcon(":/icon/icon/undo.svg"));

    mRedo.setObjectName("actionRedo");
    mRedo.setText(QObject::tr("Redo"));
    mRedo.setToolTip(QObject::tr("Redo the last undone change (Ctrl+Y)"));
    mRedo.setShortcut(QKeySequence("Ctrl+Y"));
    mRedo.setIcon(QIcon(":/icon/icon/redo.svg"));

    // checkable command: the SAME QAction is placed on the panel (large) and
    // on the quick access bar (small icon-only) — one state, two views
    mAutoWrap.setObjectName("actionAutoWrap");
    mAutoWrap.setText(QObject::tr("Auto Wrap"));
    mAutoWrap.setToolTip(QObject::tr("Toggle automatic word wrap (Ctrl+W)"));
    mAutoWrap.setShortcut(QKeySequence("Ctrl+W"));
    mAutoWrap.setCheckable(true);
    mAutoWrap.setIcon(QIcon(":/icon/icon/bold.svg"));
}
