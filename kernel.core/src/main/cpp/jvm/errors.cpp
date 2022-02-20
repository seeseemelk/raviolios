#include "jvm_context.hpp"

using namespace Java;

const char* Java::toString(ClassError error)
{
	switch (error)
	{
	case ClassError::GOOD:
		return "good";
	case ClassError::NOT_FOUND:
		return "not_found";
	case ClassError::BAD_CONSTANT:
		return "bad_constant";
	}
	return "unknown_error";
}

const char* Java::toString(ThreadCreateResult error)
{
	switch (error)
	{
	case ThreadCreateResult::CREATED:
		return "created";
	case ThreadCreateResult::NO_METHOD:
		return "no_method";
	}
	return "unknown_error";
}
