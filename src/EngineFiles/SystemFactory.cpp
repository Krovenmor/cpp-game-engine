#include <vector>
#include "EngineFiles/SystemFactory.hpp"

std::map<std::string, ActorFactory::Creator>& ActorFactory::getRegistry() {
	static std::map<std::string, Creator> registry;
	return registry;
}

void ActorFactory::registerActor(const std::string& name, Creator creator) {
	printf("Registering system: %s\n", name.c_str());
	getRegistry()[name] = creator;
	getRegisteredNames().push_back(name);
}

std::vector<std::string>& ActorFactory::getRegisteredNames() {
	static std::vector<std::string> names;
	return names;
}