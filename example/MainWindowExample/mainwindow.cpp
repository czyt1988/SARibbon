#include "mainwindow.h"
#include <QTextEdit>
#include <QAbstractButton>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include <QPushButton>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QElapsedTimer>
#include "SARibbonMenu.h"
#include "SARibbonComboBox.h"
#include "SARibbonLineEdit.h"
#include "SARibbonGallery.h"
#include "SARibbonCheckBox.h"
#define PRINT_COST(ElapsedTimer,LastTime,STR) \
    do{\
    int ___TMP_INT = ElapsedTimer.elapsed();\
    qDebug() << STR << ___TMP_INT - LastTime << "(" << ___TMP_INT << ")";\
    LastTime = ___TMP_INT;\
    }while(0)
MainWindow::MainWindow(QWidget *par):SARibbonMainWindow(par)
{
#if 1
    QElapsedTimer cost;
    int lastTimes = 0;
    cost.start();
    setWindowTitle(tr("ribbon test"));
    m_edit = new QTextEdit(this);
    setCentralWidget(m_edit);
    PRINT_COST(cost,lastTimes,"setCentralWidget & setWindowTitle");
    SARibbonBar* ribbon = ribbonBar();
    QFont f = ribbon->font();
    f.setFamily("微软雅黑");
    ribbon->setFont(f);
    ribbon->applitionButton()->setText(tr("File"));
    SARibbonCategory* categoryMain = ribbon->addCategoryPage(tr("主页"));
    PRINT_COST(cost,lastTimes,"new main page");
    createCategoryMain(categoryMain);
    PRINT_COST(cost,lastTimes,"add main page element");
    SARibbonCategory* categoryOther = ribbon->addCategoryPage(tr("其他"));
    createCategoryOther(categoryOther);
    PRINT_COST(cost,lastTimes,"add other page");
    m_contextCategory = ribbon->addContextCategory(tr("context"),Qt::red,1);
    SARibbonCategory* contextCategoryPage1 = m_contextCategory->addCategoryPage(tr("页面1"));
    SARibbonCategory* contextCategoryPage2 = m_contextCategory->addCategoryPage(tr("页面2"));
    PRINT_COST(cost,lastTimes,"add contextCategory page");
#endif
    showMaximized();
}

void MainWindow::onShowContextCategory(bool on)
{
    ribbonBar()->setContextCategoryVisible(m_contextCategory,on);
}

void MainWindow::createCategoryMain(SARibbonCategory *page)
{
    QElapsedTimer cost;
    cost.start();
    SARibbonToolButton * btn;
    SARibbonMenu* menu = new SARibbonMenu(this);
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("11"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("11111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("11111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111111111111111111111111"));

    int lastTimes = 0;
    SARibbonPannel* pannel = page->addPannel(tr("面板1"));
    PRINT_COST(cost,lastTimes,"addPannel pannel 1");

    QAction* act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/save.png"));
    act->setText(tr("保存"));
    PRINT_COST(cost,lastTimes,"new Large Action");
    pannel->addLargeAction(act);
    connect(act,&QAction::triggered,this,[this](bool b){
        this->m_edit->append("action clicked");
    });
    PRINT_COST(cost,lastTimes,"add 1 Large Action");

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/save.png"));
    act->setText(tr("隐藏ribbon"));
    act->setCheckable(true);
    pannel->addSmallAction(act);
    connect(act,&QAction::triggered,this,[this](bool b){
        this->ribbonBar()->setHideMode(b);
    });

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/save.png"));
    act->setText(tr("显示隐藏按钮"));
    act->setCheckable(true);
    pannel->addSmallAction(act);
    connect(act,&QAction::triggered,this,[this](bool b){
        this->ribbonBar()->showHideModeButton(b);
    });

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/filter.png"));
    act->setText(tr("测试2"));
    act->setMenu(menu);
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(tr("测试3"));
    act->setMenu(menu);
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(tr("测试4"));
    act->setMenu(menu);
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    pannel->addSeparator();




    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(tr("DelayedPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act,&QAction::triggered,this,&MainWindow::onDelayedPopupCheckabletriggered);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(tr("MenuButtonPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act,&QAction::triggered,this,&MainWindow::onMenuButtonPopupCheckabletriggered);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(tr("InstantPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act,&QAction::triggered,this,&MainWindow::onInstantPopupCheckabletriggered);

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(tr("DelayedPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setCheckable(true);
    connect(act,&QAction::triggered,this,&MainWindow::onDelayedPopupCheckableTest);

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(tr("MenuButtonPopup\n checkable"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    btn->setCheckable(true);
    connect(act,&QAction::triggered,this,&MainWindow::onMenuButtonPopupCheckableTest);

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(tr("InstantPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setCheckable(true);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act,&QAction::triggered,this,&MainWindow::onInstantPopupCheckableTest);

    SARibbonPannel* panne2 = page->addPannel(tr("pannel 2"));
    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(tr("显示\n上下文标签"));
    btn = panne2->addLargeAction(act);
    btn->setCheckable(true);
    //btn->setPopupMode(QToolButton::InstantPopup);
    connect(act,&QAction::triggered,this,[this](bool on){
        if(on)
            this->ribbonBar()->showContextCategory(this->m_contextCategory);
        else
            this->ribbonBar()->hideContextCategory(this->m_contextCategory);
    });

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(tr("非激活"));
    act->setMenu(menu);
    btn = panne2->addLargeAction(act);
    btn->setCheckable(true);
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setEnabled(false);


    SARibbonComboBox* com = new SARibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Fixed);
    com->setWindowIcon(QIcon(":/icon/icon/Graph-add.png"));
    com->setWindowTitle("ComboBox");
    for (int i=0;i<40;++i)
        com->comboBox()->addItem("SARibbonComboBox测试");
    com->comboBox()->setEditable(true);
    pannel->addWidget(com);

    com = new SARibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Fixed);
    com->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    com->setWindowTitle("ComboBox Editable");
    for (int i=0;i<40;++i)
        com->comboBox()->addItem("111111111111");
    pannel->addWidget(com);

    SARibbonLineEdit* lineEdit = new SARibbonLineEdit(this);
    lineEdit->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Fixed);
    lineEdit->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    lineEdit->setWindowTitle("Line Edit");
    lineEdit->lineEdit()->setText("SARibbonLineEdit");
    pannel->addWidget(lineEdit);

    SARibbonCheckBox* checkBox = new SARibbonCheckBox(this);
    checkBox->setSizePolicy(QSizePolicy::Expanding,
                       QSizePolicy::Fixed);
    checkBox->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    checkBox->checkBox()->setText("checkBox");
    pannel->addWidget(checkBox);

    pannel->setExpanding();
    QAction* optAct = new QAction(this);
    pannel->addOptionAction(optAct);
}

void MainWindow::createCategoryOther(SARibbonCategory *page)
{
    SARibbonMenu* menu = new SARibbonMenu(this);
    QAction * item = menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"),tr("1111111"));

    SARibbonPannel* pannel = page->addPannel(tr("pannel 1"));
    SARibbonToolButton* btn;
    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(tr("非标准\n图标"));
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addWidget(btn);

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(tr("换页\n测试"));
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addWidget(btn);

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(tr("大按钮"));
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addWidget(btn);

    SARibbonGallery* gallery = pannel->addGallery();
    SARibbonGalleryGroup*group = gallery->addGalleryGroup();
    for(int i=0;i<100;++i)
    {
        group->addItem(QIcon(":/icon/icon/folder.png"));
    }
    QAction* optAct = new QAction(this);
    pannel->addOptionAction(optAct);
    pannel->setObjectName("debug");

}

void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    m_edit->append(tr("MenuButtonPopupCheckableTest : %1").arg(b));
}

void MainWindow::onInstantPopupCheckableTest(bool b)
{
    m_edit->append(tr("InstantPopupCheckableTest : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    m_edit->append(tr("DelayedPopupCheckableTest : %1").arg(b));
}

void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    m_edit->append(tr("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    m_edit->append(tr("InstantPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    m_edit->append(tr("DelayedPopupCheckabletriggered : %1").arg(b));
}

