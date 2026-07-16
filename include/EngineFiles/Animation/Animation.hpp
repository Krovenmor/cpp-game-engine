#pragma once
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Time.hpp>

#include "EngineFiles/Saving/TransformableSaved.h"

class Logger;

struct TimeStruct {
	sf::Time duration;
	sf::Time dTimeMin;
	sf::Time dTimeMax;
};

class Animation: public sf::Drawable, public TransformableSaved<Animation> {
private:

	struct AnimFrame
	{
		sf::Texture* texture;
		sf::VertexArray va;
		sf::Time dTime;
		AnimFrame(sf::Time deltaTime, sf::VertexArray va_, sf::Texture* texture_) : texture(texture_), va(std::move(va_)),
			dTime(deltaTime){}
	};

	std::vector<AnimFrame> vAnimFrames;
	size_t stCurrentObj = 0;
	sf::Time elapsedTime;
	float playbackSpeed = 1.f;

	size_t obj_index = 0;
	bool is_updating = false;
	bool is_playing = true;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void DrawFramePreview();
	// Returns true when done
	bool on_frame_gui(AnimFrame& af);
	void DrawTimeline(TimeStruct& ts);
	void DrawAnimPreview() const;
	void on_gui() override;

protected:

	static Logger lA;
	std::vector<AnimFrame>& get_anim_frames() { return vAnimFrames; }

	virtual void onGui() = 0;

public:

	Animation();
	Animation(bool turn_automatic_update);

	Animation(Animation& other) = delete;
	Animation& operator=(const Animation&) = delete;

	Animation(Animation&& other) = default;

	void check_and_update(const sf::Time& elapsed);

	void turn_update(bool value);

	void on_index_change(const size_t index) {
		obj_index = index;
	}

	// By default, playback_speed = 1.f, min = 0.001f
	void set_playback_speed(float speed);

	//
	void pause_playback();
	void resume_playback();

	void from_start() { elapsedTime = sf::Time::Zero; stCurrentObj = 0; }
	sf::Time get_duration() const;
	TimeStruct get_time_info() const;
	float get_playback_speed() const;

	~Animation() override;

};
