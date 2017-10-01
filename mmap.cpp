#include <node.h>
#include <node_buffer.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

struct helper {
  size_t length;
};

static void Map_finalise(char *data, void*hint_void)
{
	struct helper *h = (struct helper *)hint_void;

	if(h->length > 0) {
		munmap(data, h->length);
	}
	delete h;
}

void munmap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto* isolate = args.GetIsolate();

    if (args.Length() != 2) {
        isolate->ThrowException(
            v8::Exception::Error(
                v8::String::NewFromUtf8(isolate, "mmap takes 2 arguments: name.")));
        return;
    }

    auto buffer = args[0];
    char* data = node::Buffer::Data(buffer);
    const size_t length = args[1]->ToInteger()->Value();

    int retVal = munmap(data, length);

    args.GetReturnValue().Set(retVal);
}


void mmap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    auto* isolate = args.GetIsolate();

    if (args.Length() != 5) {
        isolate->ThrowException(
            v8::Exception::Error(
                v8::String::NewFromUtf8(isolate, "mmap takes 5 arguments: name.")));
        return;
    }

    const size_t length = args[0]->ToInteger()->Value();
    const int protection = args[1]->ToInteger()->Value();
    const int flags = args[2]->ToInteger()->Value();
    const int fd = args[3]->ToInteger()->Value();
    const off_t offset = args[4]->ToInteger()->Value();

    char* data = (char*) mmap(0, length, protection, flags, fd, offset);

    if (data == MAP_FAILED) {
    
        isolate->ThrowException(node::ErrnoException(isolate, errno, "mmap", ""));
        return;

    }

    struct helper *h = new helper;
    h->length = length;

    auto buffer = node::Buffer::New(isolate, data, length, Map_finalise, (void*)h).ToLocalChecked();

    args.GetReturnValue().Set(buffer);
}


static void RegisterModule(v8::Local<v8::Object> exports)
{

    NODE_SET_METHOD(exports, "mmap", mmap);
    NODE_SET_METHOD(exports, "munmap", munmap);
    NODE_DEFINE_CONSTANT(exports, PROT_READ);
	  NODE_DEFINE_CONSTANT(exports, PROT_WRITE);
	  NODE_DEFINE_CONSTANT(exports, PROT_EXEC);
	  NODE_DEFINE_CONSTANT(exports, PROT_NONE);
	  NODE_DEFINE_CONSTANT(exports, MAP_SHARED);
	  NODE_DEFINE_CONSTANT(exports, MAP_PRIVATE);
	  NODE_DEFINE_CONSTANT(exports, MS_ASYNC);
	  NODE_DEFINE_CONSTANT(exports, MS_SYNC);
	  NODE_DEFINE_CONSTANT(exports, MS_INVALIDATE);

}

NODE_MODULE(mmap, RegisterModule);
