#include "jvm_thread.hpp"

using namespace Java;

void Frame::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Frame* frame = object->as<Frame>();
	visitor.visit(&frame->previous);
}

void Thread::describer(GC::Meta* object, GC::MetaVisitor& visitor)
{
	Thread* thread = object->as<Thread>();
	visitor.visit(&thread->top);
}


//Thread::Thread(VM& vm, Method& method)
//	: m_vm(vm)
//{
//}
