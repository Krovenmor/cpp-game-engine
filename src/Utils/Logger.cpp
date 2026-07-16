#include <filesystem>
#include <chrono>
#include <ctime>

#include "Utils/Logger.h"

#ifdef WITH_THREAD_SAFE
std::mutex Logger::mutex_error;
std::atomic<size_t> Logger::examples = 0;
std::mutex Logger::mutex_cout;
#else
size_t Logger::examples = 0;
#endif

#define RESET   "\033[0m"
#define RED     "\033[31m"

// Static types
std::ofstream* Logger::file_errors_ptr = nullptr;
const char* Logger::logs_filepath = LOGGER_PATH;
const char* Logger::filename_errors = ERROR_LOGGER_PATH;


std::string Logger::get_current_time() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm{};
#if defined(_MSC_VER)
	localtime_s(&local_tm, &now_c);
#else
	localtime_r(&now_c, &local_tm);
#endif
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);
	return buffer;
}

void Logger::static_init() {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_error);
#endif
	if (file_errors_ptr == nullptr) {
		std::string fullFilename = filename_errors;
		file_errors_ptr = new std::ofstream;
		if (file_errors_ptr) {
			file_errors_ptr->open(fullFilename, std::ios::out | std::ios::trunc);
			if (!file_errors_ptr->is_open()) {
				std::cerr << "Logger::static_init(): Trouble with opening file: '" << filename_errors << "'\n";
			}
		}
		else {
			std::cerr << "Trouble with creating pointer in Logger::static_init()";
		}
	}
}

void Logger::static_destruct() {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_error);
#endif
	if (examples <= 0) {
		if (file_errors_ptr != nullptr && file_errors_ptr->is_open()) {
			file_errors_ptr->close();
			delete file_errors_ptr;
			file_errors_ptr = nullptr;
			//file_errors_ptr->close();
		}
	}
}

Logger::~Logger() {
	if (file.is_open()) {
		file.close();
	}
	examples--;
	static_destruct();
}

Logger::Logger(const std::string& filename_) {
	examples++;
	static_init();
	std::string fullFilename = logs_filepath + filename_;
	file.open(fullFilename, std::ios::out | std::ios::trunc);
	if (!file.is_open()) {
		examples--;
		std::cerr << "Logger::Logger(const std::string& filename_): Trouble with opening file: '" << fullFilename << "'\n";
	}
	filename = filename_;
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&)) {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_file);
#endif
	if (file.is_open()) {
		file << manip;
	}
#ifdef WITH_STDCOUT
	{
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> cout_lock(mutex_cout);
#endif
		std::cout << manip;
	}
#endif
	return *this;
}

Logger& Logger::operator<<(const std::string& value) {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_file);
#endif
	if (file.is_open())
		file << value;
#ifdef WITH_STDCOUT
	{
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> cout_lock(mutex_cout);
#endif
		std::cout << value;
	}
#endif
	Output::entry(value);
	return *this;
}

Logger& Logger::start_entry() {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_file);
#endif
	if (file.is_open()) {
		file << "\n[" << get_current_time() << " Logger: " << filename << "] ";
	}
#ifdef WITH_STDCOUT
	{
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> cout_lock(mutex_cout);
#endif
		std::cout << "\n[" << get_current_time() << " Logger: " << filename << "] ";
	}
#endif
	std::string to_output = "\n[" + get_current_time() + " Logger: " + filename + "] ";
	Output::entry(to_output);
	return *this;
}

LogStream Logger::error() {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_error);
#endif
	if (file_errors_ptr != nullptr && file_errors_ptr->is_open()) {
		*file_errors_ptr << "\n[" << get_current_time() << " Logger: " << filename << "] ERROR: ";
		file_errors_ptr->flush();
	}
#ifdef WITH_STDCOUT
	{
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> cout_lock(mutex_cout);
#endif
		std::cout << "\n[" << get_current_time() << " Logger: " << filename << "]" << RED << " ERROR: " << RESET;
		std::cout.flush();
	}
#endif
	std::string to_output = "\n[" + get_current_time() + " Logger: " + filename + "] " + " ERROR: ";
	Output::error(to_output);
#ifdef WITH_THREAD_SAFE
	return LogStream(file_errors_ptr, mutex_file);
#else
	return LogStream(file_errors_ptr);
#endif
}

LogStream Logger::error(const std::string& s) {
#ifdef WITH_THREAD_SAFE
	std::lock_guard<std::mutex> lock(mutex_error);
#endif
	if (file_errors_ptr != nullptr && file_errors_ptr->is_open()) {
		*file_errors_ptr << "\n[" << get_current_time() << " Logger: " << filename << "]" << RED << " ERROR: " << s << RESET << std::endl;
	}
#ifdef WITH_STDCOUT
	{
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> cout_lock(mutex_cout);
#endif
		std::cout << "\n[" << get_current_time() << " Logger: " << filename << "] ERROR: " << s << std::endl;
	}
#endif
	std::string to_output = "\n[" + get_current_time() + " Logger: " + filename + "] " + " ERROR: ";
	Output::error(to_output);
#ifdef WITH_THREAD_SAFE
	return LogStream(file_errors_ptr, mutex_file);
#else
	return LogStream(file_errors_ptr);
#endif
}
