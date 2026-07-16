#include <random>
#include <chrono>
#include <filesystem>
#include "Utils/UsefulFunctions.h"

std::mt19937& get_rng() {
	static std::random_device rd;
	static std::uint32_t s = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	static std::seed_seq ss{rd(), s, (std::uint32_t)s >> 16, (std::uint32_t)reinterpret_cast<std::uintptr_t>(&rd)};
	static std::mt19937 gen(ss);
	return gen;
}

int Random(int from, int to) {
	if (from > to) {
		std::swap(from, to);
		lUF.error() << "Random from > to, from: " << from << ", to: " << to << std::endl;
	}
	return std::uniform_int_distribution<>(from, to)(get_rng());
}

float Random(float from, float to) {
	if (from > to) {
		std::swap(from, to);
		lUF.error() << "Random from > to, from: " << from << ", to: " << to << std::endl;
	}
	return std::uniform_real_distribution<float>(from, to)(get_rng());
}

std::string gimmik(const std::string& filePath) 
{
	std::string s;
	int count = 0;

	for (size_t st = filePath.size() - 1; st != 0; --st) {

		count++;

		if (filePath[st] == '/') {

			s = filePath.substr(st + 1, count);
			break;

		}
	}
	return s;
}

void Rotate(sf::Sprite& sprite, float& x, float& y) {

	sprite.rotate(0.1f);

	x += 0.001f, y += 0.001f;

	sprite.setScale(x, y);

	if (sprite.getScale().x > 5) {
		x = 0.1f, y = 0.1f;
	}

}

std::ostream& operator<<(std::ostream& out, const sf::FloatRect& fr)
{
	out << "Position: " << fr.getPosition();
	out << "\nSize: " << fr.getSize();
	out << "\nHeight:" << fr.height << ", left: " << fr.left << ", top: " << fr.top << ", width: " << fr.width;
	return out;
}

sf::Vector2f getDirection(const sf::Vector2f& vec1, const sf::Vector2f& vec2) {
	auto v_dir = vec2 - vec1;
	float length = std::sqrt(v_dir.x * v_dir.x + v_dir.y * v_dir.y);
	if (length > 0.f) {
		v_dir /= length;
		return v_dir;
	}
	return {0, 0};
}