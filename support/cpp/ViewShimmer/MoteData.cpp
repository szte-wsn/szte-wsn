#include "MoteData.h"

#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>
#include <QtDebug>
#include <qwt_series_data.h>
#include "constants.h"

class MoteData::PrivateData
{
public:
    PrivateData():
        boundingRect(1.0, 1.0, -2.0, -2.0) // invalid
    {
        //values.reserve(RESERVED_SAMPLES);
    }

    inline void append(const QPointF &sample)
    {
        values.append(sample);

        // adjust the bounding rectangle

        if ( boundingRect.width() < 0 || boundingRect.height() < 0 )
        {
            boundingRect.setRect(sample.x(), sample.y(), 0.0, 0.0);
        }
        else
        {
            boundingRect.setRight(sample.x());

            if ( sample.y() > boundingRect.bottom() )
                boundingRect.setBottom(sample.y());

            if ( sample.y() < boundingRect.top() )
                boundingRect.setTop(sample.y());
        }
    }


    QVector<QPointF> values;
    QRectF boundingRect;

};

Sample::Sample()
{
        unix_time = 0;
        xAccel = 0;
        yAccel = 0;
        zAccel = 0;
        xGyro = 0;
        yGyro = 0;
        zGyro = 0;
        voltage = 0;
        temp = 0;
}

MoteData::MoteData()
{
    //samples.reserve(RESERVED_SAMPLES);
    d_data = new PrivateData();
}

MoteData::~MoteData()
{
    //samples.clear();
    delete d_data;
}

void MoteData::setParam(Params param, int value)
{
    if(param == MOTEID){
        moteID = value;
    } else if(param == REBOOTID){
        rebootID = value;
    }
}

void MoteData::setParam(Params param, double value)
{
    if(param == LENGTH){
        length = value;
    } else if(param == BOOT_UNIX_TIME){
        boot_unix_time = value;
    } else if(param == SKEW_1){
        skew_1 = value;
    } else if(param == OFFSET){
        offset = value;
    }
}

int MoteData::size() const
{
    return d_data->values.size();
}

QPointF MoteData::value(int index) const
{
    return d_data->values[index];
}

QRectF MoteData::boundingRect() const
{
    return d_data->boundingRect;
}


void MoteData::append(const QPointF &sample)
{

    d_data->append(sample);
}


MoteData &MoteData::instance()
{
    static MoteData valueVector;
    return valueVector;
}