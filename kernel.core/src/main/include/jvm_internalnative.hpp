/*
 * jvm_internalnative.hpp
 *
 *  Created on: Sep 17, 2022
 *      Author: seeseemelk
 */

#ifndef _JVM_INTERNALNATIVE_HPP_
#define _JVM_INTERNALNATIVE_HPP_

#include "jvm_context.hpp"

namespace Java
{
	extern const Java::NativeMethod g_internalNativeMethods[];
	extern const size_t g_internalNativeMethodsCount;
}

#endif /* _JVM_INTERNALNATIVE_HPP_ */
