#pragma once

class SpawnableComponent {
private:

	unsigned long index = 0;

public:

	void set_index(const unsigned long index_) { this->index = index_; }
	unsigned long get_index_sp() const {return index;}
	void die();

	virtual ~SpawnableComponent() = default;

};