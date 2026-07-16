#pragma once
#include <SFML/Graphics.hpp>

#include "BaseClasses/Actor.h"

#define DEFAULT_PARTICLESYSTEM_NAME "ParticleSystem"

class ParticleSystem : public Actor {
private:

	struct Particle {
		sf::Vector2f velocity;
		sf::Time lifetime;
	};

	std::vector<Particle> vParticles;
	sf::VertexArray va;
	sf::Time lifetime;
	sf::Vector2f emmiter;

	void on_draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void resetParticle(std::size_t index);

public:

	ParticleSystem(size_t count);
	ParticleSystem(const char* ps_name, size_t count);

	void setEmitter(sf::Vector2f position);

	void update(sf::Time elapsed);

};