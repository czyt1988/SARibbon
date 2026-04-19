#include <QtTest>
#include <QApplication>
#include "SARibbonGallery.h"

class SARibbonGalleryRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testGalleryButtonsPosition();
    void testSizeGripPosition();
};

void SARibbonGalleryRTLTest::testGalleryButtonsPosition()
{
    SARibbonGallery gallery;
    gallery.resize(300, 200);
    gallery.show();
    QApplication::processEvents();

    // Test LTR: buttons should be on the right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QList<QAbstractButton*> buttons = gallery.findChildren<QAbstractButton*>();
    for (QAbstractButton* btn : buttons) {
        if (btn->objectName().contains("scroll") || btn->objectName().contains("more")) {
            QVERIFY(btn->geometry().left() > gallery.width() - 50); // Right edge
        }
    }

    // Test RTL: buttons should be on the left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    for (QAbstractButton* btn : buttons) {
        if (btn->objectName().contains("scroll") || btn->objectName().contains("more")) {
            QVERIFY(btn->geometry().right() < 50); // Left edge
        }
    }

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    gallery.hide();
}

void SARibbonGalleryRTLTest::testSizeGripPosition()
{
    SARibbonGallery gallery;
    gallery.setSizeAdjustPolicy(QComboBox::AdjustToContents);
    gallery.resize(300, 200);
    gallery.show();
    QApplication::processEvents();

    // Test LTR: size grip at bottom-right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QSizeGrip* sizeGrip = gallery.findChild<QSizeGrip*>();
    if (sizeGrip) {
        QRect gripRect = sizeGrip->geometry();
        QVERIFY(gripRect.left() > gallery.width() - 30);
        QVERIFY(gripRect.top() > gallery.height() - 30);
    }

    // Test RTL: size grip at bottom-left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    if (sizeGrip) {
        QRect gripRect = sizeGrip->geometry();
        QVERIFY(gripRect.right() < 30);
        QVERIFY(gripRect.top() > gallery.height() - 30);
    }

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    gallery.hide();
}

QTEST_MAIN(SARibbonGalleryRTLTest)

#include "SARibbonGalleryRTLTest.moc"
