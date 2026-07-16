#pragma once
#include <vector>
#include "Utils/Logger.h"

inline Logger lt("FastVector.log");

template <class T>
class FastVector {
private:

	std::vector<T> data;

public:

	std::vector<T>& getData() {
		return data;
	}

	const std::vector<T>& getData() const {
		return data;
	}

	size_t size() const { return data.size(); }
	auto begin() { return data.begin(); }
	auto end() { return data.end(); }

	T& operator[](size_t index) {
		return data[index];
	}

	size_t push(T element) {
		data.push_back(element);
		return data.size() - 1;
	}

	void erase(size_t index) {
		if (data.empty()) {
			lt.error() << "erase: data is empty";
			return;
		}

		if (index >= data.size()) {
			lt.error() << "erase: index out of bounds, index: " << index << ", size: " << data.size();
			return;
		}

		if (index != data.size() - 1) {
			data[index] = std::move(data.back());
			data[index]->on_index_change(index);
		}
		data.pop_back();
	}

	void erase_anim(size_t index) {
		if (data.empty()) {
			lt.error() << "erase_anim: data is empty";
			return;
		}

		if (index >= data.size()) {
			lt.error() << "erase_anim: index out of bounds, index: " << index << ", size: " << data.size();
			return;
		}

		if (index != data.size() - 1) {
			data[index] = std::move(data.back());
			data[index]->on_animIndex_change(index);
		}
		data.pop_back();
	}

};