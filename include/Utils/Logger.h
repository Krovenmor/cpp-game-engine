#pragma once
#include <fstream>

#include "UI/Output.hpp"

// LOGGER_PATH
#define LOGGER_PATH "assets/logs/"
#define ERROR_LOGGER_PATH "assets/logs/error.log"
//

#ifndef WITHOUT_STDCOUT_LOGGER
#define WITH_STDCOUT
#include <iostream>
#endif

#ifndef WITHOUT_THREAD_SAFE_LOGGER
#define WITH_THREAD_SAFE
#include <mutex>
#endif

struct LogStream {
	std::ostream* file;
#ifdef WITH_THREAD_SAFE_LOGGER
	std::lock_guard<std::mutex> lock;
	LogStream(std::ostream* f, std::mutex& m) : file(f), lock(m) {}
#endif
	LogStream(std::ostream* f) : file(f) {}

	typedef std::ostream& (*StreamManipulator)(std::ostream&);

	LogStream& operator<<(StreamManipulator manip) {
		if (file && *file) {
			manip(*file);
		}
#ifdef WITH_STDCOUT
		manip(std::cout);
#endif
		return *this;
	}

	template<typename T>
	LogStream& operator<<(const T& msg) {
		if (file && *file) {
			*file << msg;
			file->flush();
		}
#ifdef WITH_STDCOUT
		std::cout << msg;
		std::cout.flush();
#endif
		Output::error(msg);
		return *this;
	}
};

class Logger {
private:

	// Filepath for all logs
	static const char* logs_filepath;
	// For special error output
	static const char* filename_errors;
	// For error output
	static std::ofstream* file_errors_ptr;
	// Function to close static output to error file
	static void static_destruct();
	// Filename to output
	std::string filename;
	// For simple file output
	std::ofstream file;

#ifdef WITH_THREAD_SAFE
	// Mutex for console output only
	static std::mutex mutex_cout;
	// Mutex for error output only
	static std::mutex mutex_error;
	// Mutex for log_file
	std::mutex mutex_file;
	// For correct closing 'static std::unique_ptr<std::wofstream> file_errors_ptr'
	static std::atomic<size_t> examples;
#else
	static size_t examples;
#endif

	// Function to init static output to error file
	static void static_init();

	// For all
	static std::string get_current_time();

public:

	Logger(const std::string& filename);

	~Logger();

	template<typename T>
	Logger& operator<<(const T& value) {
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> lock(mutex_file);
#endif
		if (file.is_open()) {
			file << value;
		}
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

	Logger& operator<<(std::ostream& (*manip)(std::ostream&));

	Logger& operator<<(const std::string& value);

	Logger& start_entry();

	template <class A>
	void start_entry(const A& s) {
#ifdef WITH_THREAD_SAFE
		std::lock_guard<std::mutex> lock(mutex_file);
#endif
		if (file.is_open()) {
			file << "\n[" << get_current_time() << " Logger: " << filename << "] " << s << std::endl;
		}
#ifdef WITH_STDCOUT
		{
#ifdef WITH_THREAD_SAFE
			std::lock_guard<std::mutex> cout_lock(mutex_cout);
#endif
			std::cout << "\n[" << get_current_time() << " Logger: " << filename << "] " << s << std::endl;
		}
#endif
		std::string to_output = "\n[" + get_current_time() + " Logger: " + filename + "] " + s;
		Output::entry(to_output);
	}

	LogStream error();

	LogStream error(const std::string& s);

};