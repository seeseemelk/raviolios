/**@file
 * @brief
 */
#ifndef _TEST_H_
#define _TEST_H_

namespace Test
{
	typedef void (test_func_t)();

	/**
	 * @brief Succeeds a test.
	 *
	 * Prints out a test succeeded message and increments the succeeded test count.
	 *
	 * @param message The message to print out.
	 */
	void succeed(const char* message);

	/**
	 * @brief Fails a test.
	 *
	 * Prints out a test failure message and increments the failed test count.
	 *
	 * @param message The message to print out.
	 */
	void fail(const char* message);

	struct Handle
	{
		Handle* m_next;
		const char* m_name;
		test_func_t* m_function;

		Handle(const char* name, test_func_t* function);
		void run();
	};
}

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)
#define TEST(name) CREATE_TEST(name, CAT(test_func_,__COUNTER__))

#define CREATE_TEST(name, func) \
	static void func(); \
	static Test::Handle CAT(handle_,func)(name, &func); \
	void func()

#endif /* _TEST_H_ */
