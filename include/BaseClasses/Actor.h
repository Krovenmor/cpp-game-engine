#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "EngineFiles/Saving/TransformableSaved.h"


enum class priority {
	HIGH_PRIOR,
	MED_PRIOR,
	LOW_PRIOR
};

class Animation;
class ActorSavingClass;

// Base of a base , Actor class
class Actor : public sf::Drawable, public TransformableSaved<Actor> {
private:

	enum class DrawableType {
		Sprite,
		Text,
		VertexArray,
		Animation
	};

	// Structure to store sf::Drawable objects
	struct DrawableObj {
		std::unique_ptr<sf::Drawable> to_draw;
		bool is_visible;
		DrawableType type;

		DrawableObj(std::unique_ptr<sf::Drawable>&& to_draw, const bool is_visible, const DrawableType type) :
			to_draw(std::move(to_draw)), is_visible(is_visible), type(type) {};
	};

	using storage = std::vector<DrawableObj>;

	// For saving all paths and data for Actors
	ActorSavingClass* actor_saving = nullptr;
	void init_actor_saving();
	int get_id() const;

	// Settings of an Actor
	bool is_ui = false;
	size_t ActorIndex;
	priority prior = priority::LOW_PRIOR;

	// Name of an Actor
	std::string name;

	// Main objects to draw
	storage actor_drawable_storage;

	template <class Item>
	void on_gui_item(const DrawableObj& obj, const std::string& type, size_t obj_id);
	void on_gui_vertex(const DrawableObj& obj, size_t obj_id);
	void on_gui_components();
	void on_gui() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

protected:

	//Can be very useful in children classes
	storage& get_drawable_objects() {
		return actor_drawable_storage;
	}

	// If you want to draw smth along main draw() func, you can do this by overriding this func
	virtual void on_draw(sf::RenderTarget& target, sf::RenderStates states) const {}

	// Special constructor for widgets only
	Actor(const char* ui_name, bool is_ui);

public:

	explicit Actor(const char* actor_name);
	explicit Actor(const std::string& actor_name);
	explicit Actor(const char* actor_name, priority prior);
	explicit Actor(const std::string& actor_name, priority prior);

	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;

	// Make a declaration of sprite, that will be created in Actor class,
	// and can be savable/changeable in redactor.
	// Returns obj_id, -1 on trouble
	int AnnounceSprite(const char* sprite_name);

	// Make a declaration of text, that will be created in Actor class,
	// and can be savable/changeable in redactor.
	// Returns obj_id, -1 on trouble
	int AnnounceText(const char *text_name);

	// Add sprite for drawing, returns obj_id on success, -1 on error
	int AddSprite(sf::Sprite& sprite);
	// This func creates sf::Sprite based on texture and adds it for drawing,
	// returns obj_id on success, -1 on error
	int AddSprite(const std::string& texture_path);

	// Add Text for drawing, returns obj_id on success, -1 on error
	int AddText(sf::Text& text);
	// This func creates sf::Text and adds it for drawing, returns obj_id on success, -1 on error
	int AddText(const std::string& text, const std::string& font_path);
	// This func creates sf::Text ( with default constructor )
	// and adds it for drawing, returns obj_id on success, -1 on error
	int AddText();
	// Add VertexArray for drawing, returns obj_id on success, -1 on error
	int AddVertexArray(sf::VertexArray& va);

	// Adds Animation for drawing in Actor class, returns obj_id, on error -1
	int AddCustomAnimation(std::unique_ptr<Animation>&& animation);

	// Useful function to get your Added sf::Sprite, returns nullptr on error
	sf::Sprite* get_sprite(int obj_id);
	// Useful function to get your Added sf::Text, returns nullptr on error
	sf::Text* get_text(int obj_id);
	// Useful function to get your Added sf::VertexArray, returns nullptr on error
	sf::VertexArray* get_va(int obj_id);
	// Useful function to get your Added Animation, returns nullptr on error
	Animation* get_anim(int obj_id);

	// Returns max_size of sf::Sprite obj in drawable storage
	sf::Vector2f getMaxSize();
	// Returns Bounds of obj, by obj_id
	sf::FloatRect getBounds(int obj_id);
	// Returns name of an Actor
	const std::string& get_name() const { return name; }
	// Returns elapsed from last frame time
	sf::Time get_elapsed_time() const;

	bool is_drawable_object_exists(int obj_id);

	// Set visibility of drawable object with obj_id, returns true if successful, false if not found
	bool set_visibility(bool visibility, int obj_id);
	// Set visibility of all drawable objects, returns true if successful, false if no drawable objects
	bool set_visibility(bool visibility);

	//
	virtual void on_tick(sf::Time elapsed) {};

	// Game Engine func!
	// It's only used by MapHandler to delete all Actor data, and better not use it in game code
	void on_delete();

	// Game Engine func!
	// It's only used by FastVector to update Actor's index, and better not use it in game code
	void on_index_change(const size_t new_index) {
		ActorIndex = new_index;
	}

	~Actor() override;
};