#include "ribbonbackend.h"

// The command half of the migration story: plain QAction wiring that a
// widgets application already has (icons land on the actions directly; the
// QML side renders them through the SARibbon image provider bridge).

RibbonBackend::RibbonBackend(QObject* parent)
    : QObject(parent), mCoverageGroup(this), mAnimationGroup(this)
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

    // exclusive coverage group (plan-06 S5): one QAction per entry, values
    // ride on the actions as properties; the group delivers the radio
    // semantics natively — this is exactly the widgets code shape
    struct Entry { const char* key; qreal ratio; };
    const Entry coverage[] = { { "actionCoverageFull", 1.0 }, { "actionCoverageTwoThirds", 2.0 / 3 },
                               { "actionCoverageHalf", 0.5 } };
    for (const Entry& e : coverage) {
        QAction* a = new QAction(&mCoverageGroup);
        a->setObjectName(e.key);
        a->setText(QString::fromLatin1(e.key) == QStringLiteral("actionCoverageFull") ? QObject::tr("Full")
                     : (e.ratio == 2.0 / 3 ? QObject::tr("2/3") : QObject::tr("1/2")));
        a->setCheckable(true);
        a->setProperty("ratio", e.ratio);
        mCoverageGroup.addAction(a);
    }
    mCoverageGroup.setExclusive(true);
    mCoverageGroup.actions().first()->setChecked(true);
    connect(&mCoverageGroup, &QActionGroup::triggered, this, [this](QAction*) { Q_EMIT coverageChanged(); });

    struct Anim { const char* key; int effect; const char* label; };
    const Anim anims[] = { { "actionAnimSlideL", 0, QT_TRANSLATE_NOOP("RibbonBackend", "Slide L") },
                           { "actionAnimSlideR", 1, QT_TRANSLATE_NOOP("RibbonBackend", "Slide R") },
                           { "actionAnimFade", 2, QT_TRANSLATE_NOOP("RibbonBackend", "Fade") },
                           { "actionAnimNone", 3, QT_TRANSLATE_NOOP("RibbonBackend", "None") } };
    for (const Anim& e : anims) {
        QAction* a = new QAction(&mAnimationGroup);
        a->setObjectName(e.key);
        a->setText(QObject::tr(e.label));
        a->setCheckable(true);
        a->setProperty("effect", e.effect);
        mAnimationGroup.addAction(a);
    }
    mAnimationGroup.setExclusive(true);
    mAnimationGroup.actions().first()->setChecked(true);
    connect(&mAnimationGroup, &QActionGroup::triggered, this, [this](QAction*) { Q_EMIT animationChanged(); });
}

QVariantList RibbonBackend::coverageActions() const
{
    QVariantList res;
    const QList< QAction* > acts = mCoverageGroup.actions();
    for (QAction* a : acts) {
        res.append(QVariant::fromValue(a));
    }
    return res;
}

QVariantList RibbonBackend::animationActions() const
{
    QVariantList res;
    const QList< QAction* > acts = mAnimationGroup.actions();
    for (QAction* a : acts) {
        res.append(QVariant::fromValue(a));
    }
    return res;
}

qreal RibbonBackend::currentCoverage() const
{
    QAction* a = mCoverageGroup.checkedAction();
    return a ? a->property("ratio").toReal() : 1.0;
}

QString RibbonBackend::currentCoverageLabel() const
{
    QAction* a = mCoverageGroup.checkedAction();
    return a ? a->text() : QString();
}

int RibbonBackend::currentAnimation() const
{
    QAction* a = mAnimationGroup.checkedAction();
    return a ? a->property("effect").toInt() : 0;
}

QString RibbonBackend::currentAnimationLabel() const
{
    QAction* a = mAnimationGroup.checkedAction();
    return a ? a->text() : QString();
}
