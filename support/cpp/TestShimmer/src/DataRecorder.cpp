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
* Author: Mikl�s Mar�ti
* Author: P�ter Ruzicska
*/

#include <stdexcept>
#include "DataRecorder.h"
#include "Application.h"
#include <QtDebug>
#include <qfile.h>
#include <QStringList>
#include <QMessageBox>
#include "DataPlot.h"
#include "constants.h"
#include <math.h>
#include "Results.hpp"
#include "EulerAngles.hpp"

DataRecorder::DataRecorder(Application &app) : application(app)
{
    //loadCalibrationData();
    for(int i=0; i<6; i++){
        accelIdleWindowStart[i] = -1;
    }
    for(int i=0; i<7; i++){
        gyroIdleWindowStart[i] = -1;
    }

    from = to = 0;
    copySamples.clear();
}

DataRecorder::~DataRecorder() {
}

int DataRecorder::getFirstTime() const {

	if( samples.isEmpty() )
                return 0;

	return samples[0].time;
}

int DataRecorder::getLastTime() const {

	int size = samples.size();

	if( size == 0 )
		return 0;

	return samples[size-1].time;
}

void DataRecorder::onReceiveMessage(const ActiveMessage & msg)
{
    // first type of messages
    if( msg.type == 0x37 && msg.payload.size() == 100 )
    {
            for(int start = 0; start < 100; start += 20)
            {
                    Sample sample;

                    sample.time = msg.getInt(start);
                    sample.xAccel = msg.getShort(start + 4);
                    sample.yAccel = msg.getShort(start + 6);
                    sample.zAccel = msg.getShort(start + 8);
                    sample.xGyro = msg.getShort(start + 10);
                    sample.yGyro = msg.getShort(start + 12);
                    sample.zGyro = msg.getShort(start + 14);
                    sample.voltage = msg.getShort(start + 16);
                    sample.temp = msg.getShort(start + 18);

    //			qDebug() << "sample " + sample.toString();
                    samples.append(sample);
            }
            emit sampleAdded();
    }
}

void DataRecorder::clearSamples()
{
	samples.clear();
        for(int i=0; i<6; i++){
            accelIdleWindowStart[i] = -1;
        }
        for(int i=0; i<7; i++){
            gyroIdleWindowStart[i] = -1;
        }
	emit samplesCleared();
}

Sample::Sample()
{
	time = -1;
	xAccel = -1;
	yAccel = -1;
	zAccel = -1;
	xGyro = -1;
	yGyro = -1;
	zGyro = -1;
	voltage = -1;
	temp = -1;
        XYangle = 0.0;
        YZangle = 0.0;
        ZXangle = 0.0;
        for (int i=1;i<=7;++i)
            rotmat[i] = 0.0;
        rotmat[0] = 1.0;
        rotmat[4] = 1.0;
        rotmat[8] = 1.0;
}

QString Sample::toString() const
{
	QString s = "(";

	s += "time:" + QString::number(time);

	if( xAccel >= 0 || yAccel >= 0 || zAccel >= 0 )
		s += " accel: " + QString::number(xAccel)
			+ " " + QString::number(yAccel)
			+ " " + QString::number(zAccel);

	if( xGyro >= 0 || yGyro >= 0 || zGyro >= 0 )
		s += " qyro: " + QString::number(xGyro)
			+ " " + QString::number(yGyro)
			+ " " + QString::number(zGyro);

	if( voltage >= 0 )
		s += " volt:" + QString::number(voltage);

	if( temp >= 0 )
		s += " temp:" + QString::number(temp);

	return s + ')';
}

QString Sample::toCsvString() const
{
        QString s = "";

        s += QString::number(time) + ",";

        if( xAccel >= 0 || yAccel >= 0 || zAccel >= 0 )
                s += QString::number(xAccel)
                        + "," + QString::number(yAccel)
                        + "," + QString::number(zAccel)
                        + ",";

        if( xGyro >= 0 || yGyro >= 0 || zGyro >= 0 )
                s += QString::number(xGyro)
                        + "," + QString::number(yGyro)
                        + "," + QString::number(zGyro)
                        + ",";;

        if( voltage >= 0 )
                s += QString::number(voltage) + ",";

        if( temp >= 0 )
                s += QString::number(temp);

        return s;
}

void DataRecorder::csvToSample(const QString& str)
{    
        QStringList list = str.split(",");
        QStringListIterator csvIterator(list);
        Sample sample;

        sample.time = csvIterator.next().toInt();
        sample.xAccel = csvIterator.next().toInt();
        sample.yAccel = csvIterator.next().toInt();
        sample.zAccel = csvIterator.next().toInt();
        sample.xGyro = csvIterator.next().toInt();
        sample.yGyro = csvIterator.next().toInt();
        sample.zGyro = csvIterator.next().toInt();
        sample.voltage = csvIterator.next().toInt();
        sample.temp = csvIterator.next().toInt();

        samples.append(sample);

}

void DataRecorder::saveSamples(const QString& filename) const {

    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "#Time,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp" << endl;
    for (int i=0; i<samples.size(); i++){
      ts << samples[i].toCsvString() << endl;
    }

    double xtemp, ytemp, ztemp, avgtemp, xyAngle, yzAngle, zxAngle, xGyro, yGyro, zGyro;

    ts << "#Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" << endl;
    for (int i = 0; i < application.dataRecorder.size(); i++) {
        xtemp = samples[i].xAccel * accelCalibrationData[0] + samples[i].yAccel * accelCalibrationData[1] + samples[i].zAccel * accelCalibrationData[2] + accelCalibrationData[9];
        ytemp = samples[i].xAccel * accelCalibrationData[3] + samples[i].yAccel * accelCalibrationData[4] + samples[i].zAccel * accelCalibrationData[5] + accelCalibrationData[10];
        ztemp = samples[i].xAccel * accelCalibrationData[6] + samples[i].yAccel * accelCalibrationData[7] + samples[i].zAccel * accelCalibrationData[8] + accelCalibrationData[11];
        avgtemp = sqrt( pow(xtemp, 2.0) + pow(ytemp, 2.0) + pow(ztemp, 2.0) );
        xyAngle = application.dataRecorder.calculateAngle(xtemp,ytemp);
        yzAngle = application.dataRecorder.calculateAngle(ytemp,ztemp);
        zxAngle = application.dataRecorder.calculateAngle(ztemp,xtemp);
        xGyro = (samples[i].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[0] + (samples[i].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[1] + (samples[i].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[2];
        yGyro = (samples[i].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[3] + (samples[i].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[4] + (samples[i].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[5];
        zGyro = (samples[i].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[6] + (samples[i].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[7] + (samples[i].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[8];

        ts << xtemp << "," << ytemp << "," << ztemp << "," << avgtemp << "," << xyAngle << "," << yzAngle << "," << zxAngle << "," << xGyro << "," << yGyro << "," << zGyro << endl;
    }

    ts << "#Static Calibration Data" << endl;
    for (int i = 0; i < 12; ++i) {
        ts << QString::number(accelCalibrationData[i]) + "\n" ;
    }

    ts << "#Gyro Calibration Data" << endl;
    for (int i = 0; i < 12; ++i) {
        ts << QString::number(gyroCalibrationData[i]) + "\n" ;
    }

    ts << "#Gyro Minimum Averages" << endl;
    for (int i = 0; i < 3; ++i) {
        ts << QString::number(gyroMinAvgs[i]) + "\n" ;
    }

    ts.flush();
    f.close();

}

void DataRecorder::loadSamples(const QString& filename )
{
    clearSamples();
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine(); //skip first line of csv

        if(line[0] != QChar('#')){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format!");
            msgBox.exec();
        } else {
            line = ts.readLine();
            while ( !line.isEmpty() && line != "#Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" )
            {
                csvToSample(line);            //convert line string to sample
                line = ts.readLine();         // line of text excluding '\n'
            }
            f.close();
        }
    }
    emit sampleAdded();
}

void DataRecorder::loadCalibFromFile(const QString& filename )
{
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );

        line = ts.readLine();
        while ( !line.isEmpty() && line != "#Static Calibration Data" )
        {
            line = ts.readLine();         // line of text excluding '\n'
        }
        if(line != "#Static Calibration Data"){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format!");
            msgBox.exec();
        } else {
            for(int i = 0; i < 12; i++){
                line = ts.readLine();         // line of text excluding '\n'
                accelCalibrationData[i] = line.toDouble();            //convert line string to int
            }

            line = ts.readLine();
            if(line != "#Gyro Calibration Data"){
                QMessageBox msgBox;
                msgBox.setText("Wrong file format!");
                msgBox.exec();
            } else {
                for(int i = 0; i < 12; i++){
                    line = ts.readLine();         // line of text excluding '\n'
                    gyroCalibrationData[i] = line.toDouble();            //convert line string to int
                }

                line = ts.readLine();
                if(line != "#Gyro Minimum Averages"){
                    QMessageBox msgBox;
                    msgBox.setText("Wrong file format!");
                    msgBox.exec();
                } else {
                    for(int i = 0; i < 3; i++){
                        line = ts.readLine();         // line of text excluding '\n'
                        gyroMinAvgs[i] = line.toDouble();            //convert line string to int
                    }
                }
            }
        }
        f.close();
    }
    saveCalibrationData();
}

void DataRecorder::saveCalibToFile(const QString& filename ) const
{
    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "#Static Calibration Data" << endl;
    for (int i = 0; i < 12; ++i) {
        ts << QString::number(accelCalibrationData[i]) + "\n" ;
    }

    ts << "#Gyro Calibration Data" << endl;
    for (int i = 0; i < 12; ++i) {
        ts << QString::number(gyroCalibrationData[i]) + "\n" ;
    }

    ts << "#Gyro Minimum Averages" << endl;
    for (int i = 0; i < 3; ++i) {
        ts << QString::number(gyroMinAvgs[i]) + "\n" ;
    }

    ts.flush();
    f.close();
}

void DataRecorder::loadCalibrationData()
{
    int size = application.settings.beginReadArray("stationaryCalibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        accelCalibrationData[i] = application.settings.value("stationaryCalibrationData").toDouble();
    }
    application.settings.endArray();

    size = application.settings.beginReadArray("gyroCalibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        gyroCalibrationData[i] = application.settings.value("gyroCalibrationData").toDouble();
    }
    application.settings.endArray();

    size = application.settings.beginReadArray("gyroAvgsData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        gyroMinAvgs[i] = application.settings.value("gyroAvgsData").toDouble();
    }
    application.settings.endArray();
}

void DataRecorder::saveCalibrationData() const
{
    application.settings.beginWriteArray("stationaryCalibrationData");
    for (unsigned int i = 0; i < 12; i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("stationaryCalibrationData", accelCalibrationData[i]);
    }
    application.settings.endArray();

    application.settings.beginWriteArray("gyroCalibrationData");
    for (unsigned int i = 0; i < 12; i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("gyroCalibrationData", gyroCalibrationData[i]);
    }
    application.settings.endArray();

    application.settings.beginWriteArray("gyroAvgsData");
    for (unsigned int i = 0; i < 3; i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("gyroAvgsData", gyroMinAvgs[i]);
    }
    application.settings.endArray();

}

void DataRecorder::at(int i, double data[ipo::SIZE]) const {

    using namespace ipo;

    if (i<0 || i>=application.dataRecorder.size()) {

        throw std::out_of_range("Index out of range!");
    }

    const Sample& s = samples[i];

    data[TIME_STAMP] = s.time;

    const double ax = s.xAccel;
    const double ay = s.yAccel;
    const double az = s.zAccel;

    data[ACCEL_X] = ax*accelCalibrationData[0] + ay*accelCalibrationData[1] + az*accelCalibrationData[2] + accelCalibrationData[9];
    data[ACCEL_Y] = ax*accelCalibrationData[3] + ay*accelCalibrationData[4] + az*accelCalibrationData[5] + accelCalibrationData[10];
    data[ACCEL_Z] = ax*accelCalibrationData[6] + ay*accelCalibrationData[7] + az*accelCalibrationData[8] + accelCalibrationData[11];

    const double wx = s.xGyro;
    const double wy = s.yGyro;
    const double wz = s.zGyro;

    data[GYRO_X] = (wx - gyroMinAvgs[0]) * gyroCalibrationData[0] + (wy - gyroMinAvgs[1]) * gyroCalibrationData[1] + (wz - gyroMinAvgs[2]) * gyroCalibrationData[2];
    data[GYRO_Y] = (wx - gyroMinAvgs[0]) * gyroCalibrationData[3] + (wy - gyroMinAvgs[1]) * gyroCalibrationData[4] + (wz - gyroMinAvgs[2]) * gyroCalibrationData[5];
    data[GYRO_Z] = (wx - gyroMinAvgs[0]) * gyroCalibrationData[6] + (wy - gyroMinAvgs[1]) * gyroCalibrationData[7] + (wz - gyroMinAvgs[2]) * gyroCalibrationData[8];
}

void DataRecorder::loadRotationMatrices(const ipo::Results* res) {

    const int n = samples.size();

    if (res->number_of_samples()!=n) {
        throw std::logic_error("Error: the number of samples has changed since starting the solver!");
    }

    for (int i=0; i<n; ++i) {

        const double* const m = res->matrix_at(i);

        Sample& s = samples[i];

        for (int k=0; k<9; ++k) {

            s.rotmat[k] = m[k];
        }
    }
}

bool DataRecorder::euler_angle(int i, int XYZ, double& angle_deg) const {

    using namespace gyro;

    if (XYZ<0 || XYZ>=3) {
        throw std::out_of_range("Coordinate should be either X, Y or Z!");
    }

    double euler[3];

    const bool degenerate = rotmat_to_angles(samples[i].rotmat, euler);

    angle_deg = euler[XYZ];

    return degenerate;
}

int DataRecorder::getTime(int i)
{
    int returnTime = 0;

    if(i < samples.size()){
        int startTime = getFirstTime();
        if(startTime != 0){
            if(samples[i].time-startTime > LAG_THRESHOLD){
                returnTime =  int((samples[i].time -startTime) / 160);
            } else {
                returnTime = i;
            }
        }
    } else {
        returnTime = i;
    }

    return returnTime;
}

double DataRecorder::calculateAngle(double accel1, double accel2) {
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

double DataRecorder::calculateCalibratedValue(QString axis, int time)
{
    if( axis == "xAcc"){
        return (samples[time].xAccel * accelCalibrationData[0] + samples[time].yAccel * accelCalibrationData[1] + samples[time].zAccel * accelCalibrationData[2] + accelCalibrationData[9]);
    } else if( axis == "yAcc"){
        return (samples[time].xAccel * accelCalibrationData[3] + samples[time].yAccel * accelCalibrationData[4] + samples[time].zAccel * accelCalibrationData[5] + accelCalibrationData[10]);
    } else if( axis == "zAcc" ){
        return (samples[time].xAccel * accelCalibrationData[6] + samples[time].yAccel * accelCalibrationData[7] + samples[time].zAccel * accelCalibrationData[8] + accelCalibrationData[11]);
    } else if( axis == "xGyro" ){
        //return ( (samples[time].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[0] + (samples[time].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[1] + (samples[time].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[2] );
        return ( (samples[time].xGyro) * gyroCalibrationData[0] + (samples[time].yGyro) * gyroCalibrationData[1] + (samples[time].zGyro) * gyroCalibrationData[2] + gyroCalibrationData[9] );
    } else if( axis == "yGyro"){
        //return ( (samples[time].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[3] + (samples[time].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[4] + (samples[time].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[5] );
        return ( (samples[time].xGyro) * gyroCalibrationData[3] + (samples[time].yGyro) * gyroCalibrationData[4] + (samples[time].zGyro) * gyroCalibrationData[5] + gyroCalibrationData[10] );
    } else if( axis == "zGyro"){
        //return ( (samples[time].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[6] + (samples[time].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[7] + (samples[time].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[8] );
        return ( (samples[time].xGyro) * gyroCalibrationData[6] + (samples[time].yGyro) * gyroCalibrationData[7] + (samples[time].zGyro) * gyroCalibrationData[8] + gyroCalibrationData[11] );
    } else return -1;
}

double DataRecorder::calculateAbsAcc(int time)
{
    double avg;
    double x, y ,z;

    x = calculateCalibratedValue("xAcc", time);
    y = calculateCalibratedValue("yAcc", time);
    z = calculateCalibratedValue("zAcc", time);

    avg = sqrt( pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) );

    return avg;
}

void DataRecorder::edit(const QString& option)
{
    if(samples.size()< to){
        to = samples.size()-1;
    }

    if(option == "trim"){
        for(int i=from; i<=to; i++){
            copySamples.append(samples[i]);
        }
        samples.clear();
        emit samplesCleared();
        samples = copySamples;
        emit sampleAdded();
        copySamples.clear();
    } else
    if(option == "copy"){
        for(int i=from; i<=to; i++){            
            samples.append(samples[i]);
        }       
        emit sampleAdded();
    } else
    if(option == "cut"){
        for(int i=0; i<from; i++){
            copySamples.append(samples[i]);
        }
        for(int i=to; i<samples.size(); i++){
            copySamples.append(samples[i]);
        }
        samples.clear();
        emit samplesCleared();
        samples = copySamples;
        emit sampleAdded();
        copySamples.clear();
    }
}
