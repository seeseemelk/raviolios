/**@file
 * @brief
 */
#ifndef _JVM_CLASSLOADER_HPP_
#define _JVM_CLASSLOADER_HPP_

#include "jvm_context.hpp"

namespace Java
{
	struct ClassListEntry
	{
		GC::Object<ClassFile>* classFile;
		GC::Array<char>* name;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);
	};

	struct ClassList
	{
		size_t size;
		size_t capacity;
		GC::Array<ClassListEntry>* entries;

		static void describer(GC::Meta* object, GC::MetaVisitor& visitor);

		static void create(GC::Context& gc, GC::Root<ClassList>& root);
		static void add(GC::Context& gc, GC::Root<ClassList>& root, ClassListEntry& object);
	};

	/**
	 * A class loader which will cache already-loaded classes. It will prevent
	 * classes from being loaded twice.
	 */
	class CachingClassLoader : public NativeClassLoader
	{
	public:
		void parent(NativeClassLoader& parent);

		[[nodiscard]]
		ClassError loadClass(VM& vm, GC::Root<ClassFile>& root, const GC::Root<char>& name) override;

	private:
		NativeClassLoader* m_parent;
		GC::Root<ClassList> m_classList;

		ClassList& classList(VM& vm);
	};
}

#endif /* _JVM_CLASSLOADER_HPP_ */
