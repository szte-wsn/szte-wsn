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
* Author: P�ter Ruzicska
*
*/

#ifndef SDATAWIDGET_H
#define SDATAWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QVarLengthArray>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include "DownloadManager.hpp"
#include "RecordList.hpp"

class Application;

namespace Ui {
    class SDataWidget;
}

struct SData
{
    SData();
    int moteID;
    int recordID;
    int length;
    QString tor;
    QString tod;
};

void fillSData(QVarLengthArray<SData>& records);

class SDataWidget : public QWidget {
    Q_OBJECT
public:
    SDataWidget(QWidget *parent, Application &app);
    ~SDataWidget();

    const SData& getSDataAt(int i) const { return records[i]; }
    int getRecordsSize() const { return records.size(); }

private:
    Ui::SDataWidget *ui;
    Application &application;

    QMessageBox* blockingBox;
    sdc::DownloadManager manager;
    RecordList recordList;

    QVarLengthArray<SData> records;
    QVarLengthArray<SData> filteredRecords;

    volatile bool downloadFailed;
    QString errorMsg;

    enum ColumnIndex {
        MOTE_ID       = 0,
        RECORD_ID     = 1,
        LENGTH        = 2,
        DATE_OF_REC   = 3,
        DATE_DOWNLOAD = 4,
        COMMENT       = 5
    };

    void initLeft(bool filter);
    void initRight(const QVarLengthArray<int>& list);
    QVarLengthArray<int> getLinkingRecords(int moteId, int num);

    QTreeWidgetItem* createParentItem(int i, QTreeWidget* root);
    void createChildItem(int i, QTreeWidgetItem* parent);

    void init();

    void processBinaryFile(const QString& dialogCaption, const QString& startFromHere, const QString& blockTitle);
    void downloadFromDevice();
    void showBlockingBox(const QString& title, const QString& text);

    const QString selectWin32Device();
    const QString selectBinaryFile(const QString& caption, const QString& startFromHere);

    void printRecords();
    void filterRecords();

signals:

    void updateGUI();

private slots:

    void onItemSelectionChanged();
    void on_toPlotButton_clicked();
    void on_downloadButton_clicked();
    void onItemDoubleClicked(QTreeWidgetItem*,int);
    void on_fileButton_clicked();
    void onDownloadFinished(bool error, const QString& error_msg, const QVarLengthArray<SData>& data);
    void onUpdateGUI();
    void on_clearButton_clicked();
    void on_showLastTencBox_clicked();
};

#endif // SDATAWIDGET_H
