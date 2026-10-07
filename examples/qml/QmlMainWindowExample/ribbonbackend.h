#ifndef RIBBONBACKEND_H
#define RIBBONBACKEND_H

// Command-layer backend of the QML example (plan-05 S6/S8): the widgets
// migration story — the QAction creation/connection/shortcut/state code a
// widgets application already owns, exposed to QML as plain QAction*
// properties. Zero QML knowledge on this side; the same actions could feed
// a widgets SARibbonMainWindow unchanged.

#include <QObject>
#include <QAction>
#include <QKeySequence>

class RibbonBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAction* actionSave READ actionSave CONSTANT)
    Q_PROPERTY(QAction* actionUndo READ actionUndo CONSTANT)
    Q_PROPERTY(QAction* actionRedo READ actionRedo CONSTANT)
    Q_PROPERTY(QAction* actionAutoWrap READ actionAutoWrap CONSTANT)
    Q_PROPERTY(int saveCount READ saveCount NOTIFY saveCountChanged)
public:
    explicit RibbonBackend(QObject* parent = nullptr);

    QAction* actionSave() { return &mSave; }
    QAction* actionUndo() { return &mUndo; }
    QAction* actionRedo() { return &mRedo; }
    QAction* actionAutoWrap() { return &mAutoWrap; }

    int saveCount() const { return mSaveCount; }

Q_SIGNALS:
    void saveCountChanged();

private:
    QAction mSave;
    QAction mUndo;
    QAction mRedo;
    QAction mAutoWrap;
    int mSaveCount = 0;
};

#endif  // RIBBONBACKEND_H
