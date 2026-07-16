#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "Utils/Logger.h"


inline Logger lUF("UsefulFunctions.log");


int Random(int from, int to);
float Random(float from, float to);

std::string gimmik(const std::string& filePath);

void Rotate(sf::Sprite& sprite, float& x, float& y);

std::ostream& operator << (std::ostream& out, const sf::FloatRect& fr);

template <class A>
std::ostream& operator << (std::ostream& out, const sf::Vector2<A>& v) {
	out << "x: " << v.x << ", y: " << v.y;
	return out;
}

template <class A, class B>
std::ostream& operator << (std::ostream& out, const std::pair<A, B>& p) {
	out << "(" << p.first << ", " << p.second << ")";
	return out;
}

template <class A>
std::ostream& operator << (std::ostream& out, const std::vector<A>& v) {
	for (const auto& a : v) {
		out << a << " ";
	}
	return out;
}

template <class A, class B>
std::ostream& operator << (std::ostream& out, const std::map<A, B>& map) {
	for (const auto& [key, value] : map) {
		out << "key: '" << key << "', value: '" << value << "'\n";
	}
	return out;
}

template <class Key, class Value>
bool safe_search(std::map<Key, Value>& map, const Key& key_target) {
	auto it = map.find(key_target);
	if (it != map.end()) {
		return true;
	}
	return false;
}

template <class Key, class Value>
bool safe_search(std::unordered_map<Key, Value>& u_map, const Key& key_target) {
	auto it = u_map.find(key_target);
	if (it != u_map.end()) {
		return true;
	}
	return false;
}

template <class A>
void fast_remove(std::vector<A>& v, const A& target) {
    auto it = std::find(v.begin(), v.end(), target);
    if (it != v.end()) {
        *it = std::move(v.back());
        v.pop_back();
	} else {
		lUF.start_entry() << "fast_remove(std::vector<A>& v, const A& target) target not found\n";
	}

}

template <class A>
void fast_remove(std::vector<A>& v, typename std::vector<A>::iterator it) {
    if (it != v.end()) {
        *it = std::move(v.back());
        v.pop_back();
    } else {
		lUF.start_entry() << "fast_remove(std::vector<A>& v, typename std::vector<A>::iterator it) false iterator\n";
	}
}

sf::Vector2f getDirection(const sf::Vector2f& vec1, const sf::Vector2f& vec2);