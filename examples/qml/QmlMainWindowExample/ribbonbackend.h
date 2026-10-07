#ifndef RIBBONBACKEND_H
#define RIBBONBACKEND_H

// Command-layer backend of the QML example (plan-05 S6/S8): the widgets
// migration story — the QAction creation/connection/shortcut/state code a
// widgets application already owns, exposed to QML as plain QAction*
// properties. Zero QML knowledge on this side; the same actions could feed
// a widgets SARibbonMainWindow unchanged.

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QVariantList>

class RibbonBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAction* actionSave READ actionSave CONSTANT)
    Q_PROPERTY(QAction* actionUndo READ actionUndo CONSTANT)
    Q_PROPERTY(QAction* actionRedo READ actionRedo CONSTANT)
    Q_PROPERTY(QAction* actionAutoWrap READ actionAutoWrap CONSTANT)
    // exclusive groups (widgets QActionGroup parity): the QML side only
    // declares views over these action lists — the exclusivity itself is
    // native QActionGroup behavior, no imperative re-assertion anywhere
    Q_PROPERTY(QVariantList coverageActions READ coverageActions CONSTANT)
    Q_PROPERTY(QVariantList animationActions READ animationActions CONSTANT)
    Q_PROPERTY(qreal currentCoverage READ currentCoverage NOTIFY coverageChanged)
    Q_PROPERTY(QString currentCoverageLabel READ currentCoverageLabel NOTIFY coverageChanged)
    Q_PROPERTY(int currentAnimation READ currentAnimation NOTIFY animationChanged)
    Q_PROPERTY(QString currentAnimationLabel READ currentAnimationLabel NOTIFY animationChanged)
    Q_PROPERTY(int saveCount READ saveCount NOTIFY saveCountChanged)
public:
    explicit RibbonBackend(QObject* parent = nullptr);

    QAction* actionSave() { return &mSave; }
    QAction* actionUndo() { return &mUndo; }
    QAction* actionRedo() { return &mRedo; }
    QAction* actionAutoWrap() { return &mAutoWrap; }

    QVariantList coverageActions() const;
    QVariantList animationActions() const;

    qreal currentCoverage() const;
    QString currentCoverageLabel() const;
    int currentAnimation() const;
    QString currentAnimationLabel() const;

    int saveCount() const { return mSaveCount; }

Q_SIGNALS:
    void saveCountChanged();
    void coverageChanged();
    void animationChanged();

private:
    QAction mSave;
    QAction mUndo;
    QAction mRedo;
    QAction mAutoWrap;
    QActionGroup mCoverageGroup;
    QActionGroup mAnimationGroup;
    int mSaveCount = 0;
};

#endif  // RIBBONBACKEND_H
