#include "runner.hpp"

#include "test.hpp"

#include <array>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

struct TestCase
{
	const char* name;
	TestCase* next;
};

static bool s_in_test = false;
static bool s_test_failed = false;
static int s_success = 0;
static int s_fails = 0;
static bool s_quiet = false;
static Test::Handle* s_next_test = nullptr;
static TestCase* s_failedTests = nullptr;
static const char* s_currentTestName;

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
	if (s_failedTests == nullptr || s_failedTests->name != s_currentTestName)
	{
		TestCase* testCase = new TestCase();
		testCase->name = s_currentTestName;
		testCase->next = s_failedTests;
		s_failedTests = testCase;
	}

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
		puts("");
		puts("Failed tests:");
		while (s_failedTests != nullptr)
		{
			printf(" - %s\n", s_failedTests->name);
			TestCase* nextTest = s_failedTests->next;
			delete s_failedTests;
			s_failedTests = nextTest;
		}
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
	s_currentTestName = m_name;
	fprintf(stdout, "[" ANSI_BLUE "TEST" ANSI_RESET "] %s...\n", m_name);
	m_function();
}

