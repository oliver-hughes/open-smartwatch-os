#include "./apps/_experiments/3d_test.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

using std::vector;

struct vec3d {
    float x, y, z;
};

struct triangle {
    vec3d p[3];
};

struct mesh {
    vector<triangle> tris;
};

struct mat4x4 {
    //[row][col]
    float m[4][4] = { { 0 } };
};

class engine3D {
    public:
        engine3D() {};

    private:
        mesh meshCube;
        mat4x4 matProj;

        float fTheta;

        void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) {
            o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
            o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
            o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
            float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

            if (w != 0.0f)
            {
                o.x /= w; o.y /= w; o.z /= w;
            }
        }

        void DrawTriangle(Graphics2D* g2d, int x1, int y1, int x2, int y2, int x3, int y3) {
            g2d->drawLine(x1, y1, x2, y2, 16777214);
            g2d->drawLine(x2, y2, x3, y3, 16777214);
            g2d->drawLine(x3, y3, x1, y1, 16777214);
        };

    public:
        bool onCreate() {

            meshCube.tris = {
                // SOUTH
                { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

                // EAST                                                      
                { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
                { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

                // NORTH                                                     
                { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
                { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

                // WEST                                                      
                { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

                // TOP                                                       
                { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
                { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

                // BOTTOM                                                    
                { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
                { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
            };

            // proj matrix
            float fNear = 0.1f;
            float fFar = 1000.0f;
            float fFov = 90.0f;
            float fAspectRatio = 1;
            float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

            matProj.m[0][0] = fAspectRatio * fFovRad;
            matProj.m[1][1] = fFovRad;
            matProj.m[2][2] = fFar / (fFar - fNear);
            matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
            matProj.m[2][3] = 1.0f;
            matProj.m[3][3] = 0.0f;

            return true;
        };

        bool onUpdate(OswHal* hal, float fElapsedTime) {
            // hal->gfx()->fill(0);
            Graphics2D* gfx2d = hal->gfx();

            gfx2d->fill(0);

            mat4x4 matRotZ, matRotX, matRotY;
            fTheta += 1.0f * fElapsedTime;
            float accY = 1 - hal->getAccelerationY();
            float accX =  hal->getAccelerationX();
            float accZ = hal->getAccelerationZ();

            // fTheta = 0;
            // accY = 0.3;
            // accX = 0.3;

            // Rotation X
            matRotX.m[0][0] = 1;
            matRotX.m[1][1] = cosf(accY * 0.5f);
            matRotX.m[1][2] = sinf(accY * 0.5f);
            matRotX.m[2][1] = -sinf(accY * 0.5f);
            matRotX.m[2][2] = cosf(accY * 0.5f);
            matRotX.m[3][3] = 1;

            // Rotation Z
            matRotZ.m[0][0] = cosf(accX);
            matRotZ.m[0][1] = sinf(accX);
            matRotZ.m[1][0] = -sinf(accX);
            matRotZ.m[1][1] = cosf(accX);
            matRotZ.m[2][2] = 1;
            matRotZ.m[3][3] = 1;

            // Rot Y

            matRotY.m[0][0] = 1;
            matRotY.m[1][1] = 1;
            matRotY.m[1][2] = cosf(accZ * 0.5f);
            matRotY.m[2][1] = sinf(accZ * 0.5f);
            matRotY.m[2][2] = -sinf(accZ * 0.5f);
            matRotY.m[3][3] = cosf(accZ * 0.5f);

            // Draw Tris
            for (auto tri : meshCube.tris) {
                triangle triProjected, triTranslated, triRotatedZ, triRotatedZX, triRotatedZXY;

                // Rotate in Z-Axis
                MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
                MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
                MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

                // Rotate in X-Axis
                MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
                MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
                MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

                // Rotate in Y-Axis

                MultiplyMatrixVector(triRotatedZX.p[0], triRotatedZXY.p[0], matRotY);
                MultiplyMatrixVector(triRotatedZX.p[1], triRotatedZXY.p[1], matRotY);
                MultiplyMatrixVector(triRotatedZX.p[2], triRotatedZXY.p[2], matRotY);

                triTranslated = triRotatedZXY;
                triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
                triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
                triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

                MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
                MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
                MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

                triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
                triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

                float width =  240.0f; //hal->gfx()->getWidth();
                float height = 240.0f;

                triProjected.p[0].x *= 0.5f * width; //width
                triProjected.p[0].y *= 0.5f * height; //height
                triProjected.p[1].x *= 0.5f * width;
                triProjected.p[1].y *= 0.5f * height;
                triProjected.p[2].x *= 0.5f * width;
                triProjected.p[2].y *= 0.5f * height;

                DrawTriangle(gfx2d, triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y);
            }

            return true;
        };


};

engine3D demo;

long fr_ms = 0;

void OswApp3DTest::setup() {
  // this is where you initialise code, gets called when this app is shown
//   engine3D demo;
    demo.onCreate();

    fr_ms = millis();
  
}

void OswApp3DTest::loop() {
  // This section of the code is where you can write code that will loop.
    OswHal* hal = OswHal::getInstance();
    // Graphics2D* gfx2d = hal->gfx();

    long a = millis();

    demo.onUpdate(hal, 0.03f);

    long b = millis();
    fr_ms = 1000/ (b-a);
    
    
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextColor(rgb565(255, 0, 0), rgb565(0, 0, 0));  // colour changes: foreground, background
    hal->gfx()->setTextCursor(50, 120);                            // set (X,Y) coordinates for the 240x204px display
    hal->gfx()->print(fr_ms);

    debug();
    

  hal->requestFlush();  // request the screen to be updated. refreshrate is handled by main loop
}

void OswApp3DTest::debug() {
  const int defaultXHint = 40;
  const int defaultYHint = 40;
  const int fontHeight = 1;
  const int lineHeight = fontHeight * 10 + 2;
  OswHal* hal = OswHal::getInstance();

  hal->gfx()->setTextSize(fontHeight);

//   ui->resetTextColors();
  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 0 * lineHeight);
  hal->gfx()->print("Accelerometer Data:");

  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 1 * lineHeight);
  hal->gfx()->print("X: ");
  hal->gfx()->print(hal->getAccelerationX());

  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 2 * lineHeight);
  hal->gfx()->print("Y: ");
  hal->gfx()->print(hal->getAccelerationY());

  hal->gfx()->setTextCursor(defaultXHint, defaultYHint + 3 * lineHeight);
  hal->gfx()->print("Z: ");
  hal->gfx()->print(hal->getAccelerationZ());
}

void OswApp3DTest::stop() {
  // this is where you de-initialize stuff, gets called when another app is shown
  // htpc->~HTTPClient();

}
