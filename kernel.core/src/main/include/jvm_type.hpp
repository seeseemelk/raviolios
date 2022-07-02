/**@file
 * @brief
 */
#ifndef _JVM_TYPE_HPP_
#define _JVM_TYPE_HPP_

#include "gc.hpp"

namespace Java
{
	enum class Type
	{
		VOID,
		BYTE,
		CHAR,
		DOUBLE,
		FLOAT,
		INTEGER,
		LONG,
		REFERENCE,
		SHORT,
		BOOLEAN,
	};

	/**
	 * Describes a type.
	 */
	struct TypeDescriptor
	{
		union
		{
			/// The number of dimensions the array has, if it is an array.
			unsigned int dimensions = 0;
			/// The number of arguments the method has, if it is an argument.
			unsigned int arguments;
		};

		/// The type of the thing.
		Type type = Type::VOID;

		/// Set if the type describes an array.
		bool array = false;

		/// Set if the type describes a method.
		bool method = false;

		/**
		 * Gets the size of a type.
		 * Not defined for methods.
		 *
		 * @return The size of a type, in bytes.
		 */
		size_t size();

		inline bool isReferenceType() const
		{
			return array || type == Type::REFERENCE;
		}

		static TypeDescriptor parse(GC::Root<char>& type);
		static TypeDescriptor parseParameter(GC::Root<char>& type);
	};
}

#endif /* _JVM_TYPE_HPP_ */
