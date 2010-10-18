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

#include <QVarLengthArray>
#include <QObject>
#include "Data.hpp"
#include "SerialListener.h"

#ifndef DATARECORDER_H
#define DATARECORDER_H

class Application;

namespace ipo {
    class Results;
}

struct Sample
{
        Sample(); // FIXME
	QString toString() const;
        QString toCsvString() const;

	int time;
	int xAccel;
	int yAccel;
	int zAccel;
	int xGyro;
	int yGyro;
	int zGyro;
	int voltage;
	int temp;
        double rotmat[9];
        double integrated_angle[3];
        double XYangle, YZangle, ZXangle; // FIXME
};

class DataRecorder : public QObject
{
	Q_OBJECT

public:
        DataRecorder(Application &application);
        ~DataRecorder();

	int size() const {
		return samples.size();
	}

        Sample & setAngle(int i) { // FIXME
            return samples[i];
        }

	const Sample & at(int i) const {
		return samples[i];
	}

        int getFirstTime() const;
        int getLastTime() const;

        void saveCalibrationData() const;
        void loadCalibrationData();
        void clearCalibrationData();
        void setCalibToZero();

        void dump_calibration_data() const;

        const double* getAccelCalibration() const { return accelCalibrationData; }
        const double* getGyroCalibration() const { return gyroCalibrationData; }
        const double* getGyroMinAvgs() const { return gyroMinAvgs; }
        const int* getAccelIdleWindowStart() const { return accelIdleWindowStart; }
        const int* getGyroIdleWindowStart() const { return gyroIdleWindowStart; }

        void setAccelIdleWindowStart(int index, int start);
        void setGyroMinAvgs(int index, double value);
        void setAccelCalibration(int index, double value);
        void setGyroCalibration(int index, double value);
        void setGyroIdleWindowStart(int index, int start);

        double calculateAngle( double acceleration1, double acceleration2 ) const;
        double calculateCalibratedValue( QString axis, int time ) const;
        double calculateAbsAcc(int time) const;
        int getLag(int i) const;
        int getTime(int i) const;

        void at(int i, double data[ipo::SIZE]) const;
        int from, to;

        void clearSamples();
        void saveSamples(const QString& filename) const;
        void loadSamples(const QString& filename);
        void loadCalibFromFile(const QString& filename);
        void saveCalibToFile(const QString& filename) const;
        void edit(const QString& option);

        void loadResults(const ipo::Results* res);
        bool euler_angle(int sample, int X_or_Y_or_Z, double& angle_in_rad) const;
        double integrated_angle(int sample, int X_or_Y_or_Z) const;

signals:

	void sampleAdded();
	void samplesCleared();
        void fileLoaded();
        void calibrationDataLoaded();

public slots:
	void onReceiveMessage(const ActiveMessage & msg); 


private:
        void csvToSample(const QString& line); // FIXME

        void integrate_angles();
        void update_gyro_calib(const double correction[12]);

	QVarLengthArray<Sample> samples;
        QVarLengthArray<Sample> copySamples;
        double accelCalibrationData[12];
        double gyroCalibrationData[12];
        double gyroMinAvgs[3];
        int accelIdleWindowStart[6];
        int gyroIdleWindowStart[7];

        Application &application; // FIXME

};

#endif // DATARECORDER_H
