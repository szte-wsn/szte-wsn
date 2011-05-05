/* Copyright (c) 2011, University of Szeged
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
*      Author: Ali Baharev
*/

#include <iostream>
#include "ActiveMessage.hpp"
#include "AccelMagMsgReceiver.hpp"
#include "MatrixVector.hpp"

using std::cout;
using std::endl;
using gyro::vector3;

const int ACCEL_SAMPLE_COUNT = 100;
const int TEMP_COUNT         = ACCEL_SAMPLE_COUNT;
const int MAG_SAMPLE_COUNT   = 20;

void AccelMagMsgReceiver::onReceiveMessage(const ActiveMessage& msg) {

    if (msg.type != 0x12) {

        return;
    }

    unsigned int t_mote = msg.getUInt(0);

    vector3 mag(msg.getSignedInt(4), msg.getSignedInt(8), msg.getSignedInt(12));

    mag /= MAG_SAMPLE_COUNT;

    vector3 accel(msg.getUInt(16), msg.getUInt(20), msg.getUInt(24));

    accel /= ACCEL_SAMPLE_COUNT;

    double temp = msg.getUInt(28);

    temp /= TEMP_COUNT;

    //cout << "Time: " << t_mote << endl;
    //cout << "Accel: " << accel << endl;
    //cout << "Magn:  " << mag   << endl;
    //cout << "Temp:  " << temp << endl;
    //cout << endl;

    emit newSample(AccelMagSample(msg.source, t_mote, accel, mag, temp));
}
