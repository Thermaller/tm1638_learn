/*
Library examples for TM1638.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <TM1638.h>
#include <AcceleroMMA7361.h>

class Body {
    private:

    static const byte MAX_DIGIT = 100;
    static const word DOTS = 0x24;
    
    byte m_x, m_y, m_z;
    TM1638* m_ptm1638;
    AcceleroMMA7361* m_paccelero;

    void setDigit(char pos, byte digit) {
        byte lo = digit % 10;
        m_ptm1638->setDisplayDigit(lo, pos, false);
        m_ptm1638->setDisplayDigit((digit - lo) / 10, pos - 1, false);
    }

    void setDisplay() {
        setDigit(1, m_x);
        setDigit(4, m_y);
        setDigit(7, m_z);
    }

    public:
    
    Body() : m_x(0), m_y(0), m_z(0), m_ptm1638(NULL), m_paccelero(NULL)  {
    }

    void setup() {
        Serial.begin(9600);
        // define a module on data pin 8, clock pin 9 and strobe pin 7
        m_ptm1638 = new TM1638(8, 9, 7);
        m_ptm1638->setLEDs(0);
        m_paccelero = new AcceleroMMA7361();
        m_paccelero->begin(13, 12, 11, 10, A0, A1, A2);
        //m_paccelero->setARefVoltage(3.3);
        m_paccelero->setSensitivity(HIGH);                   //sets the sensitivity to +/-6G
        m_paccelero->calibrate();
    }

    ~Body() {
        delete m_ptm1638;
    }

    byte getValAndSetLed(int gForce, byte pos) {
        gForce /= 10;
        if (gForce < 0) {
            m_ptm1638->setLED(TM1638_COLOR_RED, pos);
            m_ptm1638->setLED(TM1638_COLOR_RED, pos - 1);
            gForce = -gForce;
        } else {
            m_ptm1638->setLED(TM1638_COLOR_GREEN, pos);
            m_ptm1638->setLED(TM1638_COLOR_GREEN, pos - 1);
        }
        return (byte)gForce;
    }

    void loopOnce() {
        delay(10);
        char rgch[100];
        
        int xRaw = m_paccelero->getXAccel();
        int yRaw = m_paccelero->getYAccel();
        int zRaw = m_paccelero->getZAccel();

        ::sprintf(rgch, "raw: (%d, %d, %d)", xRaw, yRaw, zRaw);
        Serial.println(rgch);
        
        m_x = getValAndSetLed(m_paccelero->getXAccel(), 7);
        m_y = getValAndSetLed(m_paccelero->getYAccel(), 4);
        m_z = getValAndSetLed(m_paccelero->getZAccel(), 1);


        ::sprintf(rgch, "(%u, %u, %u)", (unsigned)m_x, (unsigned)m_y, (unsigned)m_z);
        Serial.println(rgch);

        setDisplay();

    }
};

Body body;

void setup() {
    body.setup();
}

void loop() {
    body.loopOnce();
}
