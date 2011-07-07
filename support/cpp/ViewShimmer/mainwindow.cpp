#include "mainwindow.h"
#include "plot.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include "curvedata.h"
#include "Application.h"
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QClipboard>
#include <QDockWidget>

#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>

#include "MoteData.h"

MainWindow::MainWindow(QWidget *parent, Application &app):
    QMainWindow(parent),
    application(app)
{    
    d_plot = new Plot(this, application);

    setCentralWidget(d_plot);

    QToolBar *toolBar = new QToolBar(this);

    btnZoom = new QToolButton(toolBar);
    btnZoom->setText("Zoom");
    btnZoom->setCheckable(true);
    btnZoom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnZoom);    
    connect(btnZoom, SIGNAL(toggled(bool)), SLOT(enableZoomMode(bool)));

    btnLoad = new QToolButton(toolBar);
    btnLoad->setText("Load");
    btnLoad->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnLoad);
    connect(btnLoad, SIGNAL(clicked()), SLOT(onLoadButtonPressed()));

    btnClear = new QToolButton(toolBar);
    btnClear->setText("Clear");
    btnClear->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnClear);
    connect(btnClear, SIGNAL(clicked()), SLOT(onClearButtonPressed()));

    btnMarker = new QToolButton(this);
    btnMarker->setText("Marker");
    btnMarker->setCheckable(true);
    btnMarker->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(btnMarker);
    connect(btnMarker, SIGNAL(toggled(bool)), SLOT(enableMarkerMode(bool)));

    btnSave = new QToolButton(this);
    btnSave->setText("Save");
    toolBar->addWidget(btnSave);
    connect(btnSave, SIGNAL(clicked()), SLOT(onSaveButtonPressed()));

    btnCut = new QToolButton(this);
    btnCut->setText("Cut");
    btnCut->setCheckable(true);
    toolBar->addWidget(btnCut);    
    connect(btnCut, SIGNAL(toggled(bool)), SLOT(enableCutMode(bool)));


    btnCopy = new QToolButton(this);
    btnCopy->setText("Copy");
    btnCopy->setCheckable(true);
    toolBar->addWidget(btnCopy);
    connect(btnCopy, SIGNAL(toggled(bool)), SLOT(enableCopyMode(bool)));

    btnPaste = new QToolButton(this);
    btnPaste->setText("Paste");    
    btnPaste->setCheckable(true);
    toolBar->addWidget(btnPaste);
    connect(btnPaste, SIGNAL(toggled(bool)), SLOT(enablePasteMode(bool)));

    btnSData = new QToolButton(this);
    btnSData->setText("SData Downloader");
    toolBar->addWidget(btnSData);
    connect(btnSData, SIGNAL(clicked()), SLOT(enableSDownloader()));

    toolBar->addSeparator();

    this->addToolBar(toolBar);

    d_picker = new QwtPlotPicker(d_plot->canvas());
    d_picker->setTrackerMode(QwtPicker::AlwaysOn);
    d_picker->setTrackerPen(QColor(Qt::white));

    copyPositions.clear();

    markerText = new QLineEdit(this);
    markerText->hide();
    dockWidget = new QDockWidget(tr("Marker Notes"), this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
    dockWidget->setWidget(markerText);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    dockWidget->hide();

    //connect(d_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButtonPressed()));
    //connect(d_clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonPressed()));
}

void MainWindow::onLoadButtonPressed()
{
    if(application.moteDataHolder.motesCount() != 0){
        const int ret = QMessageBox::warning(this, "Warning", "Loading new data will erase earlier ones.\nAre you sure?",
                                             QMessageBox::Yes, QMessageBox::Cancel);

        if(ret==QMessageBox::Yes){
            onClearButtonPressed();
        } else {
            return;
        }
    }

    QString file = QFileDialog::getOpenFileName(this,"Select one or more files to open", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        application.moteDataHolder.loadCSVData( file );

        QFile f( file );
        QString line;

        if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
        {
            QTextStream ts( &f );
            line = ts.readLine();

            while(line != "#marker_id,marker_text,marker_x_pos"){
                line = ts.readLine();
            }

            line = ts.readLine();

            while ( !line.isEmpty() )
            {
                QStringList list = line.split(",");
                QStringListIterator csvIterator(list);

                QString text;
                QPointF pos;
                if(csvIterator.hasNext()){
                    int id = csvIterator.next().toInt();
                    text = csvIterator.next();
                    double xPos = csvIterator.next().toDouble();
                    double yPos = csvIterator.next().toDouble();

                    pos.setX(xPos);
                    pos.setY(yPos);
                }

                createMarker(pos, text);
                line = ts.readLine();
            }
        }


        btnZoom->setEnabled(true);
        btnMarker->setEnabled(true);
        btnClear->setEnabled(true);
    }
}

void MainWindow::onSaveButtonPressed()
{
    QString file = QFileDialog::getSaveFileName(this,"Select a file to save to!", "c:/", "CSV (*.csv);;Any File (*.*)");
    if ( !file.isEmpty() ) {
        application.moteDataHolder.saveData( file );

        QFile f( file );

        if( !f.open( QIODevice::Append) )
          {
              return;
          }

        QTextStream ts( &f );

        ts << "#marker_id,marker_text,marker_x_pos" << endl;
        for (int i = 0; i < markers.size(); i++){
          ts << i << "," << markers[i]->label().text() << "," << markers[i]->xValue() << "," << markers[i]->yValue() << endl;
        }

        ts.flush();
        f.close();

        btnZoom->setEnabled(true);
        btnMarker->setEnabled(true);
        btnClear->setEnabled(true);
    }

}

void MainWindow::onClearButtonPressed()
{
    clearCurveDatas();

    for(int i=0; i<application.moteDataHolder.motesCount(); i++){
        delete application.moteDataHolder.mote(i);
    }

    application.moteDataHolder.clearMotes();

    btnZoom->setEnabled(false);
    btnMarker->setEnabled(false);
    btnClear->setEnabled(false);
}

void MainWindow::clearCurveDatas()
{
    d_plot->clearCurves();

    curve_datas.clear();
}

void MainWindow::clearCopyDatas()
{
    copyPositions.clear();

}



void MainWindow::onLoadFinished()
{

    calculateCurveDatas(1.0);
    d_plot->createZoomer();

}

void MainWindow::enableZoomMode(bool on)
{
    d_plot->enableZoomMode(on);
    if(on == true){
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);
    }
    d_picker->setEnabled(!on);
}

void MainWindow::enableMarkerMode(bool on)
{
    //d_plot->enableZoomMode(false);

    if(on == true){

        dockWidget->show();

        btnZoom->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);        

        d_picker->setRubberBand(QwtPlotPicker::VLineRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragPointMachine());

        connect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(createMarker(QPointF)));
    } else {
        dockWidget->hide();
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(createMarker(QPointF)));
    }
}

void MainWindow::enableCopyMode(bool on)
{

    //d_plot->enableZoomMode(false);

    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCut->setChecked(false);
        btnPaste->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::RectRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragRectMachine());

        connect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(copy(QRectF)));
    } else {
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(copy(QRectF)));
    }
}

void MainWindow::enableCutMode(bool on)
{

    //d_plot->enableZoomMode(false);

    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnPaste->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::RectRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragRectMachine());

        connect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(cut(QRectF)));
    } else {
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QRectF)), this, SLOT(cut(QRectF)));
    }
}

void MainWindow::enablePasteMode(bool on)
{
    if(on == true){
        btnZoom->setChecked(false);
        btnMarker->setChecked(false);
        btnCopy->setChecked(false);
        btnCut->setChecked(false);

        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);
        d_picker->setRubberBandPen(QColor(Qt::green));
        d_picker->setStateMachine(new QwtPickerDragPointMachine());

        connect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(paste(QPointF)));
    } else {
        d_picker->setRubberBand(QwtPlotPicker::NoRubberBand);

        disconnect(d_picker, SIGNAL(selected(QPointF)), this, SLOT(paste(QPointF)));
    }
}

void MainWindow::enableSDownloader()
{
    application.sdataWidget.show();
}

void MainWindow::calculateCurveDatas(double zoomRatio)
{
    clearCurveDatas();

    qDebug() << "PLOT DEBUG:";
    qDebug() << "Zoom ratio: " << zoomRatio;

    long int longestID = -1;
    int maxLength = 0;
    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
        if(application.moteDataHolder.mote(i)->getLength() > maxLength){
            maxLength = application.moteDataHolder.mote(i)->getLength();
            longestID = i;
        }
    }
    double time = application.moteDataHolder.mote(longestID)->getLength();

    int minValue, maxValue;
    maxValue = 0; minValue = 20000;

    for(int i=0; i<application.moteDataHolder.motesCount(); i++){

        int samplesSize = application.moteDataHolder.mote(i)->samplesSize();

        int interval = (samplesSize/d_plot->canvas()->width()*zoomRatio) + 1;
        qDebug() << "Point interval: " << interval;

        CurveData* curve_data = new CurveData;

        for(long int j=0; j < samplesSize-interval; j+=interval)
        {

            double value;
            value = application.moteDataHolder.mote(i)->sampleAt(j).xAccel + application.moteDataHolder.mote(i)->sampleAt(j).yAccel + application.moteDataHolder.mote(i)->sampleAt(j).zAccel;

            if(value > maxValue) maxValue = value;
            if(value < minValue) minValue = value;

            double time;
            time = application.moteDataHolder.mote(i)->sampleAt(j).unix_time;
            QPointF point(time, value);

            curve_data->append(point);
        }

        curve_datas.append(curve_data);

        d_plot->setMoteCurve(i);
    }

    d_plot->setAxisScale(QwtPlot::xBottom, -10, time+10);
    d_plot->setAxisScale(QwtPlot::yLeft, minValue, maxValue);
    d_plot->replot();

    //d_plot->createZoomer();


    qDebug() << "Canvas width: " << d_plot->canvas()->width() << ";";
    qDebug() << "Number of curve points: " << curve_datas[0]->size();
}

void MainWindow::createMarker(const QPointF &pos)
{
    QString label = markerText->text()+"; ";
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setValue(pos);
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    marker->setLinePen(QPen(Qt::green, 0, Qt::DashDotLine));

    label.append(QString::number(pos.x()));
    marker->setLabel(label);
    marker->attach(d_plot);

    d_plot->replot();

    markers.append(marker);
    markerText->clear();
}

void MainWindow::createMarker(const QPointF &pos, QString text)
{
    QString label = text;
    QwtPlotMarker *marker = new QwtPlotMarker();
    marker->setValue(pos);
    marker->setLineStyle(QwtPlotMarker::VLine);
    marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
    marker->setLinePen(QPen(Qt::green, 0, Qt::DashDotLine));

    marker->setLabel(label);
    marker->attach(d_plot);

    d_plot->replot();

    markers.append(marker);
    markerText->clear();
}


void MainWindow::copy(QRectF rect)
{
    double from = rect.bottomLeft().x();
    double to = rect.bottomRight().x();

    clearCopyDatas();

    qDebug() << "================";
    qDebug() << "Copy Positions";

    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){

        int begining = application.moteDataHolder.findNearestSample(from, i);
        qDebug() << begining << " , " << application.moteDataHolder.mote(i)->sampleAt(begining).unix_time << "; real time: " << from;
        copyPositions.append(begining);

        int end = application.moteDataHolder.findNearestSample(to, i);
        qDebug() << end << " , " << application.moteDataHolder.mote(i)->sampleAt(end).unix_time << "; real time: " << to;
        copyPositions.append(end);
    }

    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText;
    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        for(int i = copyPositions.at(2*j); i < copyPositions.at(2*j+1); i++ ){
            QString row = QString::number(j)+","+application.moteDataHolder.mote(j)->sampleAt(i).toCsvString()+"\n";
            clipboardText.append(row);
        }
    }
    clipboard->setText(clipboardText);

}

void MainWindow::cut(QRectF rect)
{
    double from = rect.bottomLeft().x();
    double to = rect.bottomRight().x();

    int begining, end;
    clearCopyDatas();

    qDebug() << "================";
    qDebug() << "Copy Positions";
    for(int i = 0; i < application.moteDataHolder.motesCount(); i++){
        begining = application.moteDataHolder.findNearestSample(from, i);
        qDebug() << begining << " , " << application.moteDataHolder.mote(i)->sampleAt(begining).unix_time << "; real time: " << from;
        copyPositions.append(application.moteDataHolder.findNearestSample(from, i));


        end = application.moteDataHolder.findNearestSample(to, i);
        qDebug() << end << " , " << application.moteDataHolder.mote(i)->sampleAt(end).unix_time << "; real time: " << to;
        copyPositions.append(application.moteDataHolder.findNearestSample(to, i));
    }

    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText;
    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        for(int i = copyPositions.at(2*j); i < copyPositions.at(2*j+1); i++ ){
            QString row = QString::number(j)+","+application.moteDataHolder.mote(j)->sampleAt(i).toCsvString()+"\n";
            clipboardText.append(row);
        }

        application.moteDataHolder.mote(j)->deleteSamplesFrom(copyPositions.at(2*j), copyPositions.at(2*j+1)-copyPositions.at(2*j));
    }
    clipboard->setText(clipboardText);
    calculateCurveDatas(1.0);

}

void MainWindow::paste(QPointF pos)
{
    int from = pos.x();
    int begining;

    for(int j = 0; j < application.moteDataHolder.motesCount(); j++){
        QVector<Sample> samples;
        for(int i = copyPositions.at(2*j); i < copyPositions.at(2*j+1); i++ ){            
            samples.append(application.moteDataHolder.mote(j)->sampleAt(i));
        }
        begining = application.moteDataHolder.findNearestSample(from, j);

        for(int k = 0; k < samples.size(); k++){
            application.moteDataHolder.mote(j)->insertSampleAt(begining++, samples.at(k));
        }
    }    

    calculateCurveDatas(1.0);
}
