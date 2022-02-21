#include "runner.hpp"

#include "test.hpp"

#include <array>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

static bool s_in_test = false;
static bool s_test_failed = false;
static int s_success = 0;
static int s_fails = 0;
static bool s_quiet = false;
static Test::Handle* s_next_test = nullptr;

#define ANSI_RED "\x1B[91m"
#define ANSI_GREEN "\x1B[92m"
#define ANSI_BLUE "\x1B[94m"
#define ANSI_RESET "\x1B[0m"

/**
 * Converts an address to a line number.
 * @param addr Address to convert.
 * @return The line number of the address.
 */
/*static std::string addr2line(void* addr)
{
	std::string result;
	auto pid = getpid();

	char command[256];
	sprintf(command, "$PWD/kernel/test/convert_test_address.py %p %d", addr, pid);
	FILE* pipe = popen(command, "r");
	if (!pipe)
		abort();
	std::array<char, 128> buffer;
	while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
	{
		result += buffer.data();
	}
	pclose(pipe);

	result.pop_back();
	return result;
}*/

void Test::fail(const char* message)
{
	/*fputs("Stacktrace:\n", stderr);

	void* buffer[10];
	size_t size = backtrace(buffer, 10);
	char** trace = backtrace_symbols(buffer, size);
	for (size_t i = 1; i < size; i++)
	{
		std::string line = addr2line(buffer[i]);
		char toLog[512];
		snprintf(toLog, sizeof(toLog), "    %s", line.c_str());
		fprintf(stderr, "%s\n", toLog);
	}
	free(trace);*/

	fprintf(stderr, "[" ANSI_RED "FAIL" ANSI_RESET "] %s\n", message);
	s_test_failed = true;
}

void Test::succeed(const char* message)
{
	if (!s_quiet)
	{
		fprintf(stdout, "[" ANSI_GREEN " OK " ANSI_RESET "] %s\n", message);
	}
}

void Runner::runTests()
{
	Test::Handle* test = s_next_test;
	while (test != nullptr)
	{
		test->run();
		test = test->m_next;
	}
}

static void finishTest()
{
	if (s_in_test)
	{
		if (s_test_failed)
		{
			s_fails++;
		}
		else
		{
			s_success++;
		}
	}
	s_in_test = true;
	s_test_failed = false;
}

int Runner::printTestResults()
{
	finishTest();
	printf("%d succeeded, %d failed\n", s_success, s_fails);
	if (s_fails == 0)
	{
		printf("All tests succeeded\n");
		return 0;
	}
	else
	{
		return 1;
	}
}

Test::Handle::Handle(const char* name, test_func_t* function)
	: m_next(s_next_test)
	, m_name(name)
	, m_function(function)
{
	s_next_test = this;
}

void Test::Handle::run()
{
	finishTest();
	if (!s_quiet)
	{
		puts("");
	}
	fprintf(stdout, "[" ANSI_BLUE "TEST" ANSI_RESET "] %s...\n", m_name);
	m_function();
}

