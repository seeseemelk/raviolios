/**@file
 * @brief
 */
#ifndef _FILELOADER_HPP_
#define _FILELOADER_HPP_

#include "defs.hpp"

#include <cstdlib>
#include <string>

struct Buffer
{
	~Buffer();

	u8* data = nullptr;
	size_t length = 0;
};

[[nodiscard]]
bool loadFile(Buffer& buf, std::string& file);

[[nodiscard]]
bool loadTestClass(Buffer& buf, std::string file);

#endif /* _FILELOADER_HPP_ */
