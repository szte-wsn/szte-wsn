/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include "ConnectWidget.h"
#include "ui_ConnectWidget.h"
#include <qextserialenumerator.h>
#include <QRadioButton>
#include <QListWidget>
#include <QList>
#include <QtDebug>
#include <QFile>
#include "Application.h"

ConnectWidget::ConnectWidget(QWidget *parent, Application &app) :
        QWidget(parent),
        ui(new Ui::ConnectWidget),
        application(app)
{
        ui->setupUi(this);

        rescanPorts();

        ui->baudRate->setCurrentIndex(2);

        connect(this, SIGNAL(portChanged(QString, int)), &app.serialListener, SLOT(onPortChanged(QString, int)));
        connect(&app.serialListener, SIGNAL(receiveMessage(const ActiveMessage &)), this, SLOT(onReceiveMessage(const ActiveMessage &)));

}

void ConnectWidget::rescanPorts()
{
        QLayout *layout = ui->comPorts->layout();

        portButtons.clear();
        QLayoutItem *child;
        while( (child = layout->takeAt(0)) != NULL )
        {
                QWidget *widget = child->widget();
                delete child;
                delete widget;
        }

        QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
        for (int i = 0; i < ports.size(); i++) {

                QRadioButton *radio = new QRadioButton(ports.at(i).friendName);
#ifdef Q_OS_WIN32
                portButtons.insert(ports.at(i).portName, radio);
#else
                portButtons.insert(ports.at(i).physName, radio);
#endif
                layout->addWidget(radio);

        }

        QRadioButton *radio = new QRadioButton("Simulated Input Source");
        portButtons.insert("simulated", radio);
        layout->addWidget(radio);

        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        layout->addItem(spacer);
}

ConnectWidget::~ConnectWidget()
{
        delete ui;
}

void ConnectWidget::changeEvent(QEvent *e)
{
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
        default:
                break;
        }
}

void ConnectWidget::on_connectButton_clicked()
{
    connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)), Qt::DirectConnection);
    QString portName;

    foreach(QString port, portButtons.keys())
    {
            QRadioButton *button = portButtons.value(port);
            if( button && button->isChecked() )
                    portName = port;
    }



    int baudRate = ui->baudRate->currentText().toInt();

    emit portChanged(portName, baudRate);

    application.window.show();
    application.samplingThread.start();
    application.window.start();
}

void ConnectWidget::on_disconnectButton_clicked()
{
        moteItems.clear();
        ui->motes->clear();

        emit portChanged("", 0);
}

void ConnectWidget::onReceiveMessage(const ActiveMessage & msg)
{
        if( ! moteItems.contains(msg.source) )
        {
                QListWidgetItem * item = new QListWidgetItem(ui->motes);
                item->setIcon(moteIcon);
                item->setText("Mote " + QString::number(msg.source));

                moteItems.insert(msg.source, item);
        }
}

void ConnectWidget::on_refreshButton_clicked()
{
        rescanPorts();
        moteItems.clear();
        ui->motes->clear();
}
