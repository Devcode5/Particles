
// src/Particle.cpp

//Line 105 - End of the program provide by Professor
#include "Particle.h"
#include <cstdlib>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace Matrices;
using namespace sf;
using namespace std;

Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
: m_A(2, numPoints), m_numPoints(numPoints)
{
    m_ttl = TTL;
    // random angular speed in [0, PI)
    m_radiansPerSec = ((float)rand() / RAND_MAX) * M_PI;

    // cartesian view: center at (0,0), invert y
    m_cartesianPlane.setCenter(0.f, 0.f);
    m_cartesianPlane.setSize((float)target.getSize().x, - (float)target.getSize().y);

    // map mouse pixel to cartesian coords
    Vector2f center = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);
    m_centerCoordinate = center;

    // random initial velocities (pixels/sec)
    m_vx = 100 + (rand() % 401); // 100 .. 500
    if (rand() % 2) m_vx *= -1;
    m_vy = 150 + (rand() % 151); // 150 .. 300 (tweakable)

    // colors
    m_color1 = Color::White;
    m_color2 = Color(rand()%256, rand()%256, rand()%256);

    // build randomized shape: sweep around full circle with random radii
    double theta = ((double)rand() / RAND_MAX) * (M_PI / 2.0); // start between 0 and PI/2
    double dTheta = 2.0 * M_PI / (numPoints - 1);
    for (int j = 0; j < numPoints; ++j) {
        double r = 20 + (rand() % 61); // 20..80
        double dx = r * cos(theta);
        double dy = r * sin(theta);
        m_A(0,j) = m_centerCoordinate.x + (float)dx;
        m_A(1,j) = m_centerCoordinate.y + (float)dy;
        theta += dTheta;
    }
}

void Particle::draw(RenderTarget& target, RenderStates states) const {
    VertexArray lines(TriangleFan, m_numPoints + 1);
    // center pixel coordinate
    Vector2i centerPixelI = target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane);
    Vector2f centerPixel((float)centerPixelI.x, (float)centerPixelI.y);
    lines[0].position = centerPixel;
    lines[0].color = m_color1;

    for (int j = 1; j <= m_numPoints; ++j) {
        Vector2f worldPt((float)m_A(0, j-1), (float)m_A(1, j-1));
        Vector2i pix = target.mapCoordsToPixel(worldPt, m_cartesianPlane);
        lines[j].position = Vector2f((float)pix.x, (float)pix.y);
        lines[j].color = m_color2;
    }
    target.draw(lines);
}

void Particle::update(float dt) {
    m_ttl -= dt;
    // rotate
    rotate(dt * m_radiansPerSec);
    // scale (shrink)
    scale(SCALE);
    // translate based on velocities and gravity
    float dx = m_vx * dt;
    m_vy -= G * dt;
    float dy = m_vy * dt;
    translate(dx, dy);
}

void Particle::translate(double xShift, double yShift) {
    TranslationMatrix T(xShift, yShift, m_A.getCols());
    m_A = T + m_A;
    m_centerCoordinate.x = m_centerCoordinate.x + (float)xShift;
    m_centerCoordinate.y = m_centerCoordinate.y + (float)yShift;
}

void Particle::rotate(double theta) {
    // translate to origin, rotate, translate back
    Vector2f temp = m_centerCoordinate;
    translate(-temp.x, -temp.y);
    RotationMatrix R(theta);
    m_A = R * m_A; // left-multiply
    translate(temp.x, temp.y);
}

void Particle::scale(double c) {
    Vector2f temp = m_centerCoordinate;
    translate(-temp.x, -temp.y);
    ScalingMatrix S(c);
    m_A = S * m_A;
    translate(temp.x, temp.y);
}

// almostEqual and unitTests (copied from starter's Particle-1.cpp)
bool Particle::almostEqual(double a, double b, double eps) {
    return fabs(a - b) < eps;
}
void Particle::unitTests() {
    int score = 0;
    cout << "Testing RotationMatrix constructor...";
    double theta = M_PI / 4.0;
    RotationMatrix r(M_PI / 4);
    if (r.getRows() == 2 && r.getCols() == 2 && almostEqual(r(0, 0), cos(theta))
        && almostEqual(r(0, 1), -sin(theta))
        && almostEqual(r(1, 0), sin(theta))
        && almostEqual(r(1, 1), cos(theta))) {
       cout << "Passed.\n\n+1" << std::endl;
        score++;
    } else {
        cout << "Failed." << endl;
    }

    cout << "Testing ScalingMatrix constructor...";
    ScalingMatrix s(1.5);
    if (s.getRows() == 2 && s.getCols() == 2
        && almostEqual(s(0, 0), 1.5)
        && almostEqual(s(0, 1), 0)
        && almostEqual(s(1, 0), 0)
        && almostEqual(s(1, 1), 1.5)) {
        cout << "Passed.\n\n+1" << endl;
        score++;
    } else {
       cout << "Failed." << endl;
    }
    cout << "Testing TranslationMatrix constructor...";
    TranslationMatrix t(5, -5, 3);
    if (t.getRows() == 2 && t.getCols() == 3
        && almostEqual(t(0, 0), 5)
        && almostEqual(t(1, 0), -5)
        && almostEqual(t(0, 1), 5)
        && almostEqual(t(1, 1), -5)
        && almostEqual(t(0, 2), 5)
        && almostEqual(t(1, 2), -5)) {
       cout << "Passed.\n\n+1" << ;
        score++;
    } else {
        cout << "Failed." << endl;
    }

    cout << "Testing Particles..." << endl;
    cout << "Testing Particle mapping to Cartesian origin..." << std::endl;
    if (m_centerCoordinate.x != 0 || m_centerCoordinate.y != 0) {
       cout << "Failed.\n\nExpected (0,0).\n\nReceived: (" << m_centerCoordinate.x
                  << "," << m_centerCoordinate.y << ")" << endl;
    } else {
       cout << "Passed.\n\n+1" << sendl;
        score++;
    }

    cout << "Applying one rotation of 90 degrees about the origin..." << std::endl;
    Matrix initialCoords = m_A;
    rotate(M_PI / 2.0);
    bool rotationPassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++) {
        if (!almostEqual(m_A(0, j), -initialCoords(1, j)) || !almostEqual(m_A(1, j), initialCoords(0, j))) {
            cout << "Failed mapping: ";
           cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ")"
                      << " ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << std::endl;
            rotationPassed = false;
        }
    }
    if (rotationPassed) { cout << "Passed.\n\n+1" << endl; score++; }
    else { :cout << "Failed." << endl; }

   cout << "Applying a scale of 0.5..." << endl;
    initialCoords = m_A;
    scale(0.5);
    bool scalePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++) {
        if (!almostEqual(m_A(0, j), 0.5 * initialCoords(0,j)) || !almostEqual(m_A(1, j), 0.5 * initialCoords(1, j))) {
           cout << "Failed mapping: ";
          cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ")"
                      << " ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << std::endl;
            scalePassed = false;
        }
    }
    if (scalePassed) { cout << "Passed.\n\n+1" << endl; score++; }
    else { cout << "Failed." <endl; }

    cout << "Applying a translation of (10, 5)..." << endl;
    initialCoords = A;
    translate(10, 5);
    bool translatePassed = true;
    for (int j = 0; j < initialCoords.getCols(); j++) {
        if (!almostEqual(m_A(0, j), 10 + initialCoords(0, j)) || !almostEqual(m_A(1, j), 5 + initialCoords(1, j))) {
          cout << "Failed mapping: ";
           cout << "(" << initialCoords(0, j) << ", " << initialCoords(1, j) << ")"
                      << " ==> (" << m_A(0, j) << ", " << m_A(1, j) << ")" << std::endl;
            translatePassed = false;
        }
    }
    if (translatePassed) { scout << "Passed.\n\n+1" << endl; score++; }
    else { cout << "Failed." << endl; }

   cout << "Score: " << score << " / 7" << endl;
}

