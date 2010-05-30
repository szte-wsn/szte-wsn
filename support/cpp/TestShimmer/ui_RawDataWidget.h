/********************************************************************************
** Form generated from reading UI file 'RawDataWidget.ui'
**
** Created: Tue May 25 16:54:15 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAWDATAWIDGET_H
#define UI_RAWDATAWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "src/PlotScrollArea.h"

QT_BEGIN_NAMESPACE

class Ui_RawDataWidget
{
public:
    QVBoxLayout *verticalLayout;
    PlotScrollArea *scrollArea;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *xAccel;
    QCheckBox *yAccel;
    QCheckBox *zAccel;
    QCheckBox *xGyro;
    QCheckBox *yGyro;
    QCheckBox *zGyro;
    QCheckBox *voltage;
    QCheckBox *temp;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *loadButton;
    QPushButton *saveButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *clearButton;
    QPushButton *recordButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *RawDataWidget)
    {
        if (RawDataWidget->objectName().isEmpty())
            RawDataWidget->setObjectName(QString::fromUtf8("RawDataWidget"));
        RawDataWidget->resize(500, 389);
        verticalLayout = new QVBoxLayout(RawDataWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        scrollArea = new PlotScrollArea(RawDataWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setCursor(QCursor(Qt::CrossCursor));

        verticalLayout->addWidget(scrollArea);

        widget_2 = new QWidget(RawDataWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        xAccel = new QCheckBox(widget_2);
        xAccel->setObjectName(QString::fromUtf8("xAccel"));
        xAccel->setChecked(true);

        horizontalLayout_2->addWidget(xAccel);

        yAccel = new QCheckBox(widget_2);
        yAccel->setObjectName(QString::fromUtf8("yAccel"));
        QPalette palette;
        QBrush brush(QColor(0, 0, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(118, 116, 108, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        yAccel->setPalette(palette);
        yAccel->setChecked(true);

        horizontalLayout_2->addWidget(yAccel);

        zAccel = new QCheckBox(widget_2);
        zAccel->setObjectName(QString::fromUtf8("zAccel"));
        QPalette palette1;
        QBrush brush2(QColor(255, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        zAccel->setPalette(palette1);
        zAccel->setChecked(true);

        horizontalLayout_2->addWidget(zAccel);

        xGyro = new QCheckBox(widget_2);
        xGyro->setObjectName(QString::fromUtf8("xGyro"));
        QPalette palette2;
        QBrush brush3(QColor(0, 170, 0, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        xGyro->setPalette(palette2);
        xGyro->setChecked(true);

        horizontalLayout_2->addWidget(xGyro);

        yGyro = new QCheckBox(widget_2);
        yGyro->setObjectName(QString::fromUtf8("yGyro"));
        QPalette palette3;
        QBrush brush4(QColor(0, 255, 255, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush4);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush4);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        yGyro->setPalette(palette3);
        yGyro->setChecked(true);

        horizontalLayout_2->addWidget(yGyro);

        zGyro = new QCheckBox(widget_2);
        zGyro->setObjectName(QString::fromUtf8("zGyro"));
        QPalette palette4;
        QBrush brush5(QColor(170, 0, 127, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush5);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush5);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        zGyro->setPalette(palette4);
        zGyro->setChecked(true);

        horizontalLayout_2->addWidget(zGyro);

        voltage = new QCheckBox(widget_2);
        voltage->setObjectName(QString::fromUtf8("voltage"));
        QPalette palette5;
        QBrush brush6(QColor(255, 255, 0, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush6);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        voltage->setPalette(palette5);
        voltage->setChecked(false);

        horizontalLayout_2->addWidget(voltage);

        temp = new QCheckBox(widget_2);
        temp->setObjectName(QString::fromUtf8("temp"));

        horizontalLayout_2->addWidget(temp);


        verticalLayout->addWidget(widget_2);

        widget = new QWidget(RawDataWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        loadButton = new QPushButton(widget);
        loadButton->setObjectName(QString::fromUtf8("loadButton"));

        horizontalLayout->addWidget(loadButton);

        saveButton = new QPushButton(widget);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        horizontalLayout->addWidget(saveButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        clearButton = new QPushButton(widget);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));

        horizontalLayout->addWidget(clearButton);

        recordButton = new QPushButton(widget);
        recordButton->setObjectName(QString::fromUtf8("recordButton"));

        horizontalLayout->addWidget(recordButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);

        verticalLayout->setStretch(0, 1);

        retranslateUi(RawDataWidget);

        QMetaObject::connectSlotsByName(RawDataWidget);
    } // setupUi

    void retranslateUi(QWidget *RawDataWidget)
    {
        RawDataWidget->setWindowTitle(QApplication::translate("RawDataWidget", "Form", 0, QApplication::UnicodeUTF8));
        xAccel->setText(QApplication::translate("RawDataWidget", "x-accel", 0, QApplication::UnicodeUTF8));
        yAccel->setText(QApplication::translate("RawDataWidget", "y-accel", 0, QApplication::UnicodeUTF8));
        zAccel->setText(QApplication::translate("RawDataWidget", "z-accel", 0, QApplication::UnicodeUTF8));
        xGyro->setText(QApplication::translate("RawDataWidget", "x-gyro", 0, QApplication::UnicodeUTF8));
        yGyro->setText(QApplication::translate("RawDataWidget", "y-gyro", 0, QApplication::UnicodeUTF8));
        zGyro->setText(QApplication::translate("RawDataWidget", "z-gyro", 0, QApplication::UnicodeUTF8));
        voltage->setText(QApplication::translate("RawDataWidget", "voltage", 0, QApplication::UnicodeUTF8));
        temp->setText(QApplication::translate("RawDataWidget", "temp", 0, QApplication::UnicodeUTF8));
        loadButton->setText(QApplication::translate("RawDataWidget", "(L)oad", 0, QApplication::UnicodeUTF8));
        loadButton->setShortcut(QApplication::translate("RawDataWidget", "L", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("RawDataWidget", "(S)ave", 0, QApplication::UnicodeUTF8));
        saveButton->setShortcut(QApplication::translate("RawDataWidget", "S", 0, QApplication::UnicodeUTF8));
        clearButton->setText(QApplication::translate("RawDataWidget", "(C)lear", 0, QApplication::UnicodeUTF8));
        clearButton->setShortcut(QApplication::translate("RawDataWidget", "C", 0, QApplication::UnicodeUTF8));
        recordButton->setText(QApplication::translate("RawDataWidget", "(R)ecord", 0, QApplication::UnicodeUTF8));
        recordButton->setShortcut(QApplication::translate("RawDataWidget", "R", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RawDataWidget: public Ui_RawDataWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAWDATAWIDGET_H
