#include <filesystem>
#include <string>
#include "EngineFiles/Saving/JSONhandler.h"


#define SAVED_PROJECTS_PATH "assets/config/saved_projects.json"


struct SavedProjects {
	// .first = project_name, .second = project_path
	std::vector<std::pair<std::string, std::string>> projects;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SavedProjects,
		projects);
};

void print_menu() {
	std::cout << "Type:"
		<< "\n1: create new project"
		<< "\n2: open project"
		<< "\n3: close\n";
}

void create_project(SavedProjects& sp) {
	std::cout << "Creating of a new proj, enter project name:\n";
	std::string name;
	std::cin >> name;
	std::cout << "Enter full path:\n";
	std::string dest;
	std::cin >> dest;

	std::string full_cmd = "(echo " + dest + "; echo " + name + ") | ./scripts/create_default_proj.sh";
	int result = std::system(full_cmd.c_str());

	if (result == 0) {
		std::pair p_new(std::move(name), std::move(dest));
		sp.projects.emplace_back(std::move(p_new));
		jsonhandler::save<SavedProjects>(SAVED_PROJECTS_PATH, sp);
	}
	std::cout << std::endl;
}

void open_project(SavedProjects& sp) {
	if (!sp.projects.empty()) {
		for (unsigned i = 0; i < sp.projects.size(); ++i) {
			std::cout << "Project " << i << ": " << sp.projects[i].first << "\n";
		}
		unsigned proj;
		std::cin >> proj;
		if (proj < sp.projects.size()) {
			std::cout << "\nSelected project: " << sp.projects[proj].first << std::endl;
			std::string full_cmd = "(echo " + sp.projects[proj].second + "; echo "
				+ sp.projects[proj].first + ") | ./scripts/make_and_run.sh";

			int result = std::system(full_cmd.c_str());
			std::cout << "Result: " << result << std::endl;
		} else {
			std::cout << "\nInvalid project choice!\n";
		}
	}else {
		std::cout << "Nothing..." << std::endl;
	}
}

int main() {
	auto projects = jsonhandler::get<SavedProjects>(SAVED_PROJECTS_PATH);
	if (jsonhandler::error) {
		jsonhandler::save<SavedProjects>(SAVED_PROJECTS_PATH, projects);
	}

	std::cout << "path: " << std::filesystem::current_path() << std::endl;

	int consoleInput;
	print_menu();
	while (std::cin >> consoleInput) {
		switch (consoleInput) {
			case 1:
				create_project(projects);
				break;
			case 2:
				open_project(projects);
				break;
			case 3:
				return 0;
			default:
				std::cout << "Invalid input" << std::endl;
				break;
		}
		print_menu();
	}
}
