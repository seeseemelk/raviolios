/**@file
 * @brief
 */
#ifndef _GC_HPP_
#define _GC_HPP_

#include "defs.hpp"

#include <cstddef>

namespace GC
{
	class Context;
	struct Meta;
//	template<typename T> struct Ref;
	template<typename T> struct Object;
	template<typename T> struct Array;

	/**
	 * Visits each meta in a object.
	 */
	class MetaVisitor
	{
	public:
		virtual ~MetaVisitor() = default;
		virtual void visit(Meta** object) = 0;

//		template<typename T>
//		void visit(Ref<T>** ref)
//		{
//			visit(reinterpret_cast<Meta**>(ref));
//		}

		template<typename T>
		void visit(Array<T>** ref)
		{
			visit(reinterpret_cast<Meta**>(ref));
		}

		template<typename T>
		void visit(Object<T>** ref)
		{
			visit(reinterpret_cast<Meta**>(ref));
		}
	};

	/**
	 * A function which visits all reference field in an object.
	 *
	 * @param meta The object itself.
	 * @param context The garbage collection context.
	 */
	typedef void (MetaDescriber)(Meta* object, MetaVisitor& visitor);

	/**
	 * Describes an object as something that contains no pointers.
	 *
	 * @param object
	 * @param visitor
	 */
	void emptyDescriber(Meta* object, MetaVisitor& visitor);

	/**
	 * Describes an object as something that contains only pointers.
	 *
	 * @param object
	 * @param visitor
	 */
	void arrayDescriber(Meta* object, MetaVisitor& visitor);

	/**
	 * Meta information about an allocated object.
	 */
	struct Meta
	{
		/// A function that will call a visitor on each reference to another
		/// object within this object.
		MetaDescriber* describer = &emptyDescriber;

		/// The size of the object in bytes.
		u32 size : 31;

		/// Set by the mark step if the object is reachable.
		u32 reachable : 1;

		/**
		 * Gets a void pointer to the actual object owned by this meta.
		 *
		 * Note that this pointer might change whenever a garbage collection
		 * cycle occurs.
		 *
		 * @returns A pointer to the owned object.
		 */
		void* getRaw();
	};

//	/**
//	 * A typed wrapper around @ref Meta.
//	 *
//	 * @tparam T The type stored by the meta.
//	 */
//	template<typename T>
//	struct Ref : Meta
//	{
//		Ref()
//		{
//			size = sizeof(T);
//		}
//
//		/**
//		 * Gets a pointer to the object owned by this meta.
//		 *
//		 * The same warnings as for @ref getRaw() apply.
//		 *
//		 * @tparam T The type of object owned.
//		 *
//		 * @return A reference to the object.
//		 */
//		T& get()
//		{
//			return *static_cast<T*>(getRaw());
//		}
//	};

	template<typename T>
	struct Allocator : Meta
	{
		Allocator(size_t count, MetaDescriber* describer)
		{
			this->size = count * sizeof(T);
			this->describer = describer;
		}

		Allocator(size_t count)
			: Allocator(count, &emptyDescriber)
		{
		}

		Allocator(MetaDescriber* describer)
			: Allocator(1, describer)
		{
		}

		Allocator()
			: Allocator(1, &emptyDescriber)
		{
		}
	};

	template<typename T>
	struct Object : Meta
	{
		T object;
	};

	template<typename T>
	struct Array : Meta
	{
		T& get(size_t index)
		{
			return asPtr()[index];
		}

		T& operator[](size_t index)
		{
			return get(index);
		}

		T* asPtr()
		{
			return reinterpret_cast<T*>(getRaw());
		}
	};

	/**
	 * A root to an object.
	 */
	struct RawRoot
	{
		Meta* object = nullptr;
		RawRoot* previous = nullptr;
		RawRoot* next = nullptr;

		~RawRoot();

		void storeMeta(Meta** meta)
		{
			*meta = object;
		}

		void set(Meta* object, RawRoot* previous, RawRoot* next);
		void set(const RawRoot& root);
		void clear();
	};

	/**
	 * A typed root to an object.
	 */
	template<typename T>
	struct Root : RawRoot
	{
		size_t count()
		{
			return object->size / sizeof(T);
		}

		T& get()
		{
			return *static_cast<T*>(object->getRaw());
		}

//		void storeRef(Ref<T>** ref)
//		{
//			storeMeta(reinterpret_cast<Meta**>(ref));
//		}

		void store(Object<T>** object)
		{
			storeMeta(reinterpret_cast<Meta**>(object));
		}

		void store(Array<T>** array)
		{
			storeMeta(reinterpret_cast<Meta**>(array));
		}

		T& operator[](size_t index)
		{
			return (&get())[index];
		}
	};

	enum class AllocResult
	{
		SUCCESS,
		OUT_OF_MEMORY
	};

	/**
	 * Garbage collection context.
	 */
	class Context
	{
	public:
		/**
		 * Creates a new garbage collection context.
		 */
		void init(void* memoryStart, size_t size);

		/**
		 * Cleans up the context.
		 */
		void deinit();

		/**
		 * Get the number of bytes of memory that are being used.
		 *
		 * @return The number of bytes of memory that are being used.
		 */
		size_t getUsed();

		/**
		 * Get the number of bytes of memory that are free.
		 *
		 * @return The number of bytes of memory that are free.
		 */
		size_t getFree();

		/**
		 * Performs a garbage collection cycle.
		 */
		void collect();

		/**
		 * Allocates data on the context.
		 *
		 * @param meta Information about the object to allocate.
		 * @param root The allocated object.
		 */
		AllocResult allocateRaw(Meta& meta, RawRoot& root);

//		/**
//		 * Allocates data on the context.
//		 *
//		 * @tparam T The type of object to allocate.
//		 *
//		 * @param meta Information about the object to allocate.
//		 * @param root The allocated object.
//		 */
		template<typename T>
		AllocResult allocate(Allocator<T>& allocator, Root<T>& root)
		{
			return allocateRaw(allocator, root);
		}
//		template<typename T>
//		AllocResult allocate(Object<T>& meta, Root<T>& root)
//		{
//			return allocateRaw(meta, root);
//		}
//
//		template<typename T>
//		AllocResult allocate(Array<T>& meta, Root<T>& root)
//		{
//			return allocateRaw(meta, root);
//		}
//		template<typename T>
//		AllocResult allocate(Ref<T>& meta, Root<T>& root)
//		{
//			return allocateRaw(meta, root);
//		}

		/**
		 * Creates a root to an object.
		 *
		 * @param meta The object to create a root to.
		 * @param root The root to create.
		 */
		void makeRootRaw(Meta& meta, RawRoot& root);

		/**
		 * Creates a root to an object.
		 *
		 * @param object The object to create a root to.
		 * @param root The root to create.
		 */
		template<typename T>
		void makeRoot(Object<T>& object, Root<T>& root)
		{
			makeRootRaw(object, root);
		}

		/**
		 * Creates a root to an object.
		 *
		 * @param object The object to create a root to.
		 * @param root The root to create.
		 */
		template<typename T>
		void makeRoot(Array<T>& object, Root<T>& root)
		{
			makeRootRaw(object, root);
		}

		/**
		 * Allocates permanent data.
		 *
		 * This can only be done _before_ any normal objects have been allocated.
		 *
		 * @param size The number of bytes to allocate.
		 *
		 * @return The allocated object.
		 */
		void* permAlloc(size_t size);

		/**
		 * Templated version of @ref permAlloc.
		 *
		 * @tparam T The type to allocate.
		 * @param count The number of instances of T to allocate.
		 *
		 * @return The allocated object.
		 */
		template<typename T>
		T* permAlloc(size_t count = 1)
		{
			return static_cast<T*>(permAlloc(sizeof(T) * count));
		}

	private:
		/// Address at which allocations start.
		u8* m_memStart = nullptr;
		/// Address at which new allocations are made.
		u8* m_memCurrent = nullptr;
		/// The capacity of the memory region.
		size_t m_memCapacity = 0;
		/// The number of bytes of permanent storage being used.
		size_t m_permUsed = 0;
		/// The number of objects that exist.
		size_t m_objects = 0;

		/// The start of all roots.
		RawRoot m_root;

		/**
		 * Gets the first object.
		 *
		 * @return The first object.
		 */
		Meta* firstObject();

		/**
		 * Marks all objects that can be found via the roots.
		 */
		void mark();

		/**
		 * Sweeps over all objects and update any references to moved objects.
		 */
		void sweepUpdate();

		/**
		 * Updates the reference of an object from something to something else.
		 *
		 * @param from The old address.
		 * @param to The new address.
		 */
		void updateAddress(Meta* from, Meta* to);

		/**
		 * Updates the reference of an object from something to something else.
		 *
		 * Only operates on the heap.
		 *
		 * @param from The old address.
		 * @param to The new address.
		 */
		void updateAddressHeap(Meta* from, Meta* to);

		/**
		 * Updates the reference of an object from something to something else.
		 *
		 * Only operates on the roots.
		 *
		 * @param from The old address.
		 * @param to The new address.
		 */
		void updateAddressRoots(Meta* from, Meta* to);

		/**
		 * Sweeps over all objects and moves any to eliminate gaps.
		 */
		void sweepMove();

		/**
		 * Creates an object.
		 *
		 * @param meta The object to allocate.
		 *
		 * @return A root to the allocated object.
		 */
		void createObject(Meta& meta, RawRoot& root);
	};
}

#endif /* _GC_HPP_ */
