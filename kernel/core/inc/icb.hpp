/**@file
 * @brief
 */
#ifndef _ICB_HPP_
#define _ICB_HPP_

#include "defs.hpp"
#include "jvm_classloader.hpp"

namespace ICB
{
	/**
	 * Supported tags for an ICB entry.
	 */
	enum class Tag : u8
	{
		START = 0,
		CLASS = 1
	};

	/**
	 * An entry in an ICB bundle.
	 */
	struct Entry
	{
		const char* name = nullptr;
		u8* content = nullptr;
		size_t length = 0;
		Tag tag = Tag::START;
	};

	/**
	 * A wrapper around an ICB file.
	 */
	class Bundle
	{
	public:
		/**
		 * Creates a ICB bundle from a memory buffer.
		 *
		 * @param data A pointer to the ICB bundle.
		 * @param size The size of the ICB bundle.
		 */
		static Bundle from(u8* data, size_t size);

		/**
		 * Checks whether the data the bundle points to is a valid ICB1 file or
		 * not.
		 *
		 * @return `true` if the object points to a valid ICB1 file, `false` if
		 * it points to an invalid ICB1 file.
		 */
		bool isICB1();

		/**
		 * Goes to the next entry in the ICB bundle.
		 *
		 * @param entry The current entry.
		 *
		 * @return `true` if the next entry was read, `false` if the end of the
		 * ICB bundle was reached.
		 */
		bool next(Entry& entry);

	private:
		u8* m_data;
		size_t m_size;

		/**
		 * Reads an ICB entry at a memory address.
		 *
		 * @param entry The read ICB entry.
		 * @param data The raw ICB entry within the bundle.
		 */
		void readModule(Entry& entry, u8* data);

		/**
		 * Reads a 32-bit little-endian number.
		 *
		 * @param data The pointer to the number.
		 *
		 * @return The number.
		 */
		u32 readU32(u8* data);

		/**
		 * Reads a 16-bit little-endian number.
		 *
		 * @param data The pointer to the number.
		 *
		 * @return The number.
		 */
		u16 readU16(u8* data);
	};

	/**
	 * Loads classes from an ICB bundle.
	 */
	struct Loader
	{
		static const Java::NativeClassLoader vtable;

		void bundle(Bundle& bundle);

		Bundle m_bundle;
	};
//	class Loader final : public Java::NativeClassLoader
//	{
//	public:
//		void bundle(Bundle& bundle);
//
//		[[nodiscard]]
//		Java::ClassError loadClass(Java::VM& vm, GC::Root<Java::ClassFile>& root, const GC::Root<char>& name) override;
//
//	private:
//		Bundle m_bundle;
//	};
}

#endif /* _ICB_HPP_ */
