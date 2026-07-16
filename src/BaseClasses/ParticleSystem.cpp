#include <cmath>

#include "BaseClasses/ParticleSystem.h"
#include "Utils/UsefulFunctions.h"


void ParticleSystem::on_draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	states.texture = NULL;

	target.draw(va, states);
}

void ParticleSystem::resetParticle(std::size_t index)
{
	float angle = (Random(0, 360)) * 3.14f / 180.f;
	float speed = (Random(0, 50)) + 50.f;
	vParticles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
	vParticles[index].lifetime = sf::milliseconds((Random(0, 2000) + 1000));

	va[index].position = emmiter;
}

ParticleSystem::ParticleSystem(size_t count) :
vParticles(count), va(sf::Points, count), lifetime(sf::seconds(3.f)), emmiter(0.f, 0.f),
Actor(DEFAULT_PARTICLESYSTEM_NAME, priority::HIGH_PRIOR) {}

ParticleSystem::ParticleSystem(const char* ps_name, size_t count) :
vParticles(count), va(sf::Points, count), lifetime(sf::seconds(3.f)), emmiter(0.f, 0.f),
Actor(ps_name, priority::HIGH_PRIOR) {}

void ParticleSystem::setEmitter(sf::Vector2f position) { emmiter = position; }

void ParticleSystem::update(sf::Time elapsed) {
	for (size_t i = 0; i < vParticles.size(); ++i) {

		Particle& p = vParticles[i];
		p.lifetime -= elapsed;

		if (p.lifetime <= sf::Time::Zero) {
			resetParticle(i);
		}

		va[i].position += p.velocity * elapsed.asSeconds();

		float ratio = p.lifetime.asSeconds() / lifetime.asSeconds();
		va[i].color.a = static_cast<sf::Uint8>(ratio * 255);

	}
}