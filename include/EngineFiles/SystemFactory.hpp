#pragma once
#include <map>
#include <string>
#include <functional>
#include <memory>
class Actor;

class ActorFactory {
public:
	using Creator = std::function<std::unique_ptr<Actor>(const char* name)>;

	static std::map<std::string, Creator>& getRegistry();
	static std::vector<std::string>& getRegisteredNames();

	static void registerActor(const std::string& name, Creator creator);
};

#define REGISTER_ACTOR(ClassName) \
namespace { \
	static bool _registered_##ClassName = []() { \
			ActorFactory::registerActor(#ClassName, [](const char* name) { \
			return std::make_unique<ClassName>(name); \
			}); \
		return true; \
		}(); \
}
