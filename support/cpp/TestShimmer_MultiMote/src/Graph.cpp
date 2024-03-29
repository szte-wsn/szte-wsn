#include "Graph.h"

#include <QPainter>
#include <QtDebug>
#include <QPaintEvent>
#include <QScrollArea>
#include "Application.h"
#include "DataRecorder.h"
#include "math.h"
#include "PlotScrollArea.h"
#include "GLWidget.h"
#include "constants.h"

Graph::Graph(PlotScrollArea *parent, Application &app) : QWidget(parent),
        application(app)
{
        scrollArea = parent;
        graphs = XANG | YANG | TIME | GRID;

        //connect(&app.dataRecorder, SIGNAL(sampleAdded()), this, SLOT(onSampleAdded()));
        connect(&app.dataRecorder, SIGNAL(samplesCleared()), this, SLOT(onSamplesCleared()));

        loadSettingsData();

        resize(QWIDGETSIZE_MAX, 1000);
}

QPoint Graph::getSample(int x, int y)
{
        if( parentHeight <= 1 )
                parentHeight = 2;

        y = 4095 - (y * 4095 / (parentHeight - 1));
        return QPoint(x, y);
}

QPoint Graph::getPoint(int x, int y)
{
        y = (parentHeight - 1) * (4095 - y) / 4095;
        return QPoint(x, y);
}

void Graph::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);     //painter = new QPainter(this); //painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        parentHeight = parentWidget()->height();
        QRect rect = event->rect();

        // to see which area is beeing refreshed
                //painter.drawLine(rect.left(), rect.top(), rect.right(), rect.bottom());

        const DataRecorder &dataRecorder = application.dataRecorder;

        int x0 = rect.left() - 2;
        int x1 = rect.right() + 2;

        if( x0 < application.dataRecorder.size() )
        {
            if( x0 < 0 )
                    x0 = 0;
            if( x1 > application.dataRecorder.size() )
                    x1 = dataRecorder.size();

            if( (graphs & XRAWACC) != 0 )
            {
                    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).xAccel), getPoint(i, dataRecorder.at(i).xAccel));
            }

            if( (graphs & YRAWACC) != 0 )
            {
                    painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).yAccel), getPoint(i, dataRecorder.at(i).yAccel));
            }

            if( (graphs & ZRAWACC) != 0 )
            {
                    painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).zAccel), getPoint(i, dataRecorder.at(i).zAccel));
            }

            if( (graphs & XRAWGYRO) != 0 )
            {
                    painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).xGyro), getPoint(i, dataRecorder.at(i).xGyro));
            }

            if( (graphs & YRAWGYRO) != 0 )
            {
                    painter.setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).yGyro), getPoint(i, dataRecorder.at(i).yGyro));
            }

            if( (graphs & ZRAWGYRO) != 0 )
            {
                    painter.setPen(QPen(Qt::magenta, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).zGyro), getPoint(i, dataRecorder.at(i).zGyro));
            }

            if( (graphs & VOLTAGE) != 0 )
            {
                    painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).voltage), getPoint(i, dataRecorder.at(i).voltage));
            }

            if( (graphs & TEMP) != 0 )
            {
                    painter.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
                    for(int i = x0 + 1; i < x1; ++i)
                            painter.drawLine(getPoint(i-1, dataRecorder.at(i-1).temp), getPoint(i, dataRecorder.at(i).temp));
            }

            if( (graphs & XACC) != 0 )
            {
                painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = calculateCalibratedValue("xAcc", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * 512/GRAV + 2048), getPoint(i, xtemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & YACC) != 0 )
            {
                painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
                double ytemp1, ytemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = calculateCalibratedValue("yAcc", i);
                    painter.drawLine(getPoint(i-1, ytemp1 * 512/GRAV + 2048), getPoint(i, ytemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & ZACC) != 0 )
            {
                painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
                double ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ztemp1 = calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = calculateCalibratedValue("zAcc", i);
                    painter.drawLine(getPoint(i-1, ztemp1 * 512/GRAV + 2048), getPoint(i, ztemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & ABSACC) != 0 )
            {
                painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine));
                double avgtemp1, avgtemp2;
                double xtemp1, xtemp2, ytemp1, ytemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = calculateCalibratedValue("xAcc", i);
                    ytemp1 = calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = calculateCalibratedValue("yAcc", i);
                    ztemp1 = calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = calculateCalibratedValue("zAcc", i);

                    avgtemp1 = sqrt( pow(xtemp1, 2.0) + pow(ytemp1, 2.0) + pow(ztemp1, 2.0) );
                    avgtemp2 = sqrt( pow(xtemp2, 2.0) + pow(ytemp2, 2.0) + pow(ztemp2, 2.0) );
                    painter.drawLine(getPoint(i-1, avgtemp1 * 512/GRAV + 2048), getPoint(i, avgtemp2 * 512/GRAV + 2048));
                }
            }

            if( (graphs & XYANG) != 0 )
            {
                painter.setPen(QPen(Qt::darkBlue, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double xtemp1, xtemp2, ytemp1, ytemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = calculateCalibratedValue("xAcc", i);
                    ytemp1 = calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = calculateCalibratedValue("yAcc", i);

                    alfa1 = calculateAngle(xtemp1, ytemp1);
                    alfa2 = calculateAngle(xtemp2, ytemp2);

                    //emit angleChanged(alfa1);
                    //HACK
                    application.dataRecorder.setAngle(i).XYangle = alfa2;

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & YZANG) != 0 )
            {
                painter.setPen(QPen(Qt::darkGreen, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double ytemp1, ytemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = calculateCalibratedValue("yAcc", i);
                    ztemp1 = calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = calculateCalibratedValue("zAcc", i);

                    alfa1 = calculateAngle(ytemp1, ztemp1);
                    alfa2 = calculateAngle(ytemp2, ztemp2);

                    //HACK
                    application.dataRecorder.setAngle(i).YZangle = alfa2;

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & ZXANG) != 0 )
            {
                painter.setPen(QPen(Qt::darkRed, 2, Qt::SolidLine));
                double alfa1, alfa2;
                double xtemp1, xtemp2, ztemp1, ztemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = calculateCalibratedValue("xAcc", i);
                    ztemp1 = calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = calculateCalibratedValue("zAcc", i);

                    alfa1 = calculateAngle(ztemp1, xtemp1);
                    alfa2 = calculateAngle(ztemp2, xtemp2);

                    //HACK
                    application.dataRecorder.setAngle(i).ZXangle = alfa2;

                    if ( alfa1 < 9.0 && alfa2 < 9.0 ) {
                        if ( alfa1 > (15*M_PI)/16 || alfa1 < -(15*M_PI)/16 ) {
                            painter.drawPoint( getPoint(i-1, alfa1 * 2048/M_PI + 2048) );
                        } else {
                            painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                        }
                    }
                }
            }

            if( (graphs & XGYRO) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("xGyro", i-1);
                    xtemp2 = calculateCalibratedValue("xGyro", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * (2048/(4*M_PI)) + 2048), getPoint(i, xtemp2 * (2048/(4*M_PI)) + 2048));
                }
            }

            if( (graphs & YGYRO) != 0 )
            {
                painter.setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("yGyro", i-1);
                    xtemp2 = calculateCalibratedValue("yGyro", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * (2048/(4*M_PI)) + 2048), getPoint(i, xtemp2 * (2048/(4*M_PI)) + 2048));
                }
            }

            if( (graphs & ZGYRO) != 0 )
            {
                painter.setPen(QPen(Qt::magenta, 2, Qt::SolidLine));
                double xtemp1, xtemp2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("zGyro", i-1);
                    xtemp2 = calculateCalibratedValue("zGyro", i);
                    painter.drawLine(getPoint(i-1, xtemp1 * (2048/(4*M_PI)) + 2048), getPoint(i, xtemp2 * (2048/(4*M_PI)) + 2048));
                }
            }

            if( (graphs & XANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double xtemp1, xtemp2, alfa1, alfa2;
                for(int i = x0 + 1; i < x1; ++i) {
                    xtemp1 = calculateCalibratedValue("xAcc", i-1);
                    xtemp2 = calculateCalibratedValue("xAcc", i);

                    double param1, param2;
                    param1 = xtemp1 / GRAV;
                    param2 = xtemp2 / GRAV;
                    //if(param1 > 1) param1 = 1; if(param1 < -1) param1 = -1;
                    //if(param2 > 1) param2 = 1; if(param2 < -1) param2 = -1;
                    alfa1 = asin(param1);
                    alfa2 = asin(param2);
                    painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                }
            }

            if( (graphs & YANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double ytemp1, ytemp2, alfa1, alfa2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ytemp1 = calculateCalibratedValue("yAcc", i-1);
                    ytemp2 = calculateCalibratedValue("yAcc", i);

                    alfa1 = asin(ytemp1/GRAV);
                    alfa2 = asin(ytemp2/GRAV);
                    painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                }
            }

            if( (graphs & ZANG) != 0 )
            {
                painter.setPen(QPen(Qt::green, 2, Qt::SolidLine));
                double ztemp1, ztemp2, alfa1, alfa2;
                for(int i = x0 + 1; i < x1; ++i) {
                    ztemp1 = calculateCalibratedValue("zAcc", i-1);
                    ztemp2 = calculateCalibratedValue("zAcc", i);

                    alfa1 = asin(ztemp1/GRAV);
                    alfa2 = asin(ztemp2/GRAV);
                    painter.drawLine(getPoint(i-1, alfa1 * 2048/M_PI + 2048), getPoint(i, alfa2 * 2048/M_PI + 2048));
                }
            }
        }

        if( (graphs & GRID) != 0 )
        {
            painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
            painter.drawLine(0, parentHeight/2, width(), parentHeight/2);
            painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));

            int start = floor((rect.left() - 20) / 100);
            int end = ceil((rect.right() + 20) / 100);

            int startt = -(end - start) /2;
            int endd = (end - start) /2;

            for(int i = startt; i <= endd; ++i) {
                    QPoint xp = getPoint(i * 100, 2048);
                    painter.drawLine(xp.x(), xp.y() - 5, xp.x(), xp.y() + 5);

                    double sec = (double)i / 2;

                    painter.drawText(xp.x() - 20, xp.y() - 15, 41, 10, Qt::AlignCenter, QString::number(sec, 'f', 1));
            }

           //painter.drawLine(0, parentHeight/4, 10, parentHeight/8);
           // painter.drawLine(0, 3*parentHeight/4, width(), 3*parentHeight/4);
           // painter.drawLine(0, parentHeight/8, width(), parentHeight/8);
           // painter.drawLine(0, 3*parentHeight/8, width(), 3*parentHeight/8);
           // painter.drawLine(0, 5*parentHeight/8, width(), 5*parentHeight/8);
           // painter.drawLine(0, 7*parentHeight/8, width(), 7*parentHeight/8);
        }

}

double Graph::calculateAngle(double accel1, double accel2) {
    double alfa;
    if( (pow(accel1, 2.0) + pow(accel2, 2.0)) < pow(GRAV/2, 2.0) )
        return 10.0;

    if( fabs(accel1) > fabs(accel2) ) {
        alfa = atan(accel2/accel1);
        if( accel1 < 0 )
            alfa += M_PI;
    }
    else {
        alfa = M_PI/2 - atan(accel1/accel2);
        if( accel2 < 0 )
            alfa += M_PI;
    }

    if ( alfa >= M_PI) alfa -= 2*M_PI;
    if ( alfa < -M_PI) alfa += 2*M_PI;

    return alfa;
}

// FIXME Can this go to DataRecoreder?
double Graph::calculateCalibratedValue(QString axis, int time)
{
    if( axis == "xAcc"){
        return (application.dataRecorder.at(time).xAccel * calibrationData[0] + application.dataRecorder.at(time).yAccel * calibrationData[1] + application.dataRecorder.at(time).zAccel * calibrationData[2] + calibrationData[9]);
    } else if( axis == "yAcc"){
        return (application.dataRecorder.at(time).xAccel * calibrationData[3] + application.dataRecorder.at(time).yAccel * calibrationData[4] + application.dataRecorder.at(time).zAccel * calibrationData[5] + calibrationData[10]);
    } else if( axis == "zAcc" ){
        return (application.dataRecorder.at(time).xAccel * calibrationData[6] + application.dataRecorder.at(time).yAccel * calibrationData[7] + application.dataRecorder.at(time).zAccel * calibrationData[8] + calibrationData[11]);
    } else if( axis == "xGyro" ){
        //return ( (application.dataRecorder.at(time).xGyro - gyroMinAvgs[0]) * gyroCalibrationData[0] + (application.dataRecorder.at(time).yGyro - gyroMinAvgs[1]) * gyroCalibrationData[1] + (application.dataRecorder.at(time).zGyro - gyroMinAvgs[2]) * gyroCalibrationData[2] );
        return ( (application.dataRecorder.at(time).xGyro) * gyroCalibrationData[0] + (application.dataRecorder.at(time).yGyro) * gyroCalibrationData[1] + (application.dataRecorder.at(time).zGyro) * gyroCalibrationData[2] + gyroCalibrationData[9] );
    } else if( axis == "yGyro"){
        //return ( (application.dataRecorder.at(time).xGyro - gyroMinAvgs[0]) * gyroCalibrationData[3] + (application.dataRecorder.at(time).yGyro - gyroMinAvgs[1]) * gyroCalibrationData[4] + (application.dataRecorder.at(time).zGyro - gyroMinAvgs[2]) * gyroCalibrationData[5] );
        return ( (application.dataRecorder.at(time).xGyro) * gyroCalibrationData[3] + (application.dataRecorder.at(time).yGyro) * gyroCalibrationData[4] + (application.dataRecorder.at(time).zGyro) * gyroCalibrationData[5] + gyroCalibrationData[10] );
    } else if( axis == "zGyro"){
        //return ( (application.dataRecorder.at(time).xGyro - gyroMinAvgs[0]) * gyroCalibrationData[6] + (application.dataRecorder.at(time).yGyro - gyroMinAvgs[1]) * gyroCalibrationData[7] + (application.dataRecorder.at(time).zGyro - gyroMinAvgs[2]) * gyroCalibrationData[8] );
        return ( (application.dataRecorder.at(time).xGyro) * gyroCalibrationData[6] + (application.dataRecorder.at(time).yGyro) * gyroCalibrationData[7] + (application.dataRecorder.at(time).zGyro) * gyroCalibrationData[8] + gyroCalibrationData[11] );
    } else return -1;
}

void Graph::setGraphs(int graphs, bool on)
{
        if( on )
                this->graphs |= graphs;
        else
                this->graphs &= ~graphs;

        QWidget::update();
}

void Graph::onSampleAdded()
{
        if( plotWidth != application.dataRecorder.size() )
        {
                int oldWidth = plotWidth;
                plotWidth = application.dataRecorder.size();

                scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
                scrollArea->ensureVisible(plotWidth,0,1,1);

                QWidget::update(oldWidth-2, 0, plotWidth-oldWidth+2, parentWidget()->height());
        }
}

void Graph::onSamplesCleared()
{
        plotWidth = application.dataRecorder.size();
        scrollArea->setWidgetRect(QRect(0, 0, plotWidth, 1000));
        scrollArea->ensureVisible(plotWidth,0,1,1);
        QWidget::update(0, 0, parentWidget()->width(), parentWidget()->height());
}

void Graph::onNewCalibration()
{
    loadSettingsData();
    QWidget::update();
}

void Graph::mousePressEvent(QMouseEvent * event)
{
    startPos = event->pos();

    if (event->buttons() & Qt::LeftButton) {
        QPoint sample = getSample(event->pos().x(), event->pos().y());
        QString message = "Time: " + QString::number(sample.x()/C_HZ, 'f', 1) + " sec  ";
        if( (graphs & XRAWACC) != 0 || (graphs & YRAWACC) != 0 || (graphs & ZRAWACC) != 0 || (graphs & XRAWGYRO) != 0 || (graphs & YRAWGYRO) != 0 || (graphs & ZRAWGYRO) != 0 ){
            message.append("Sample: " + QString::number(sample.y()) + " ");
        }
        if( (graphs & XACC) != 0 || (graphs & YACC) != 0 || (graphs & ZACC) != 0 || (graphs & ABSACC) != 0 ){
            message.append(" , Acceleration: " + QString::number((double)((sample.y()-2048)/(512/GRAV)), 'f', 2) + " m/s^2");
        }
        if( (graphs & XYANG) != 0 || (graphs & YZANG) != 0 || (graphs & ZXANG) != 0 || (graphs & XANG) != 0 || (graphs & YANG) != 0 || (graphs & ZANG) != 0 ){
            message.append(",  Angle: " + QString::number((sample.y()-2048)/(2048/M_PI), 'f', 1) + "rad; "  + QString::number( (sample.y()-2048)/(2048/M_PI)*57.296, 'f', 2 ) + "°.");
        }
        if( (graphs & XGYRO) != 0 || (graphs & YGYRO) != 0 || (graphs & ZGYRO) != 0 ){
            message.append(",  Gyroscope: " + QString::number((sample.y()-2048)/(2048/(4*M_PI)),'f',1) + "rad/sec; " + QString::number( ((sample.y()-2048)/(2048/(4*M_PI)))*RADIAN, 'f', 2 ) + "°/sec; " + QString::number( (((sample.y()-2048)/(2048/(4*M_PI)))*60)/(2*M_PI), 'f', 1 ) + "rpm." );
        }
        application.showMessage( message );
    } else if (event->buttons() & Qt::RightButton) {
        //painter->backgroundMode(Qt::TransparentMode);

    }

    lastPos = event->pos();
}

void Graph::mouseMoveEvent(QMouseEvent *event)
{
    //int dx = event->x() - lastPos.x();
    //int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {

    } else if (event->buttons() & Qt::RightButton) {

    }
    lastPos = event->pos();
}

void Graph::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

// FIXME Can this go to the DataRecorder?
void Graph::loadSettingsData()
{
    int size = application.settings.beginReadArray("calibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        calibrationData[i] = application.settings.value("calibrationData").toDouble();
    }
    application.settings.endArray();

    size = application.settings.beginReadArray("gyroCalibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        gyroCalibrationData[i] = application.settings.value("gyroCalibrationData").toDouble();
    }
    application.settings.endArray();

}
