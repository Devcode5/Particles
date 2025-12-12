
// src/Engine.cpp
#include "Engine.h"
#include "Particle.h"
#include <SFML/Window.hpp>
#include <ctime>
#include <iostream>

using namespace std;

Engine::Engine() {
    srand((unsigned)time(nullptr));
    // create window (1280x720)
    m_Window.create(VideoMode(1280, 720), "Particles");
}

void Engine::run() {
    cout << "Starting Particle unit tests..." << endl;
    // unit tests particle in center
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete.  Starting engine..." << endl;

    sf::Clock clock;
    while (m_Window.isOpen()) {
        float dt = clock.restart().asSeconds();
        input();
        update(dt);
        draw();
    }
}

void Engine::input() {
    sf::Event event;
    while (m_Window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) m_Window.close();
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) m_Window.close();
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            // spawn several particles
            Vector2i pos = sf::Mouse::getPosition(m_Window);
            for (int i = 0; i < 5; ++i) {
                int numPoints = 25 + (rand() % 26); // 25..50
                m_particles.emplace_back(m_Window, numPoints, pos);
            }
        }
    }
}

void Engine::update(float dt) {
    for (auto it = m_particles.begin(); it != m_particles.end(); ) {
        if (it->getTTL() > 0.0f) {
            it->update(dt);
            ++it;
        } else {
            it = m_particles.erase(it);
        }
    }
}

void Engine::draw() {
    m_Window.clear(sf::Color::Black);
    for (auto &p : m_particles) m_Window.draw(p);
    m_Window.display();
}

