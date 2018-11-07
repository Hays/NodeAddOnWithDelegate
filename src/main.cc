#include <node/node.h>
#include <node/uv.h>
#include <node/v8.h>
#include <node/node_buffer.h>
#include <iostream>
#import <Foundation/Foundation.h>
#import <Fake/Fake.h>
#import <Cocoa/Cocoa.h>

using namespace v8;
@class FakeServiceCallback;

struct ShareData
{
    Isolate *isolate;
    Persistent<Function> js_callback;
};

struct AsyncRequest
{
    uv_work_t request;
    char *key;
    int argc = 1;
    NSArray *argv;
};
// Local<Value> argv[];

static ShareData *persistData;
static FakeService *service;
static FakeServiceCallback *delegate;

void worker_cb(uv_work_t *req) {
    // printf("test by Hays in worker cb.....\n");
}

void after_worker_cb(uv_work_t *req, int status) {
    AsyncRequest *aq = static_cast<AsyncRequest *>(req->data);
    ShareData *share = persistData;
    Isolate * isolate = share->isolate;
    HandleScope scope(isolate);
    
    Local<Function> js_callback = Local<Function>::New(isolate, share->js_callback);
    Local<Value> argv[aq->argc];
    argv[0] = String::NewFromUtf8(isolate, aq->key);
    
    for(int i = 0; i < aq->argc - 1; i++)
    {
        id arg = aq->argv[i];
        if ([arg isKindOfClass:[NSNumber class]]) {
            argv[i+1] = Integer::New(isolate, [arg integerValue]);
        } 
        else if ([arg isKindOfClass:[NSString class]]) {
            NSString *str = (NSString *)arg;
            argv[i+1] = String::NewFromUtf8(isolate, [str cStringUsingEncoding:NSUTF8StringEncoding]);
        }
    }
    
    js_callback->Call(isolate->GetCurrentContext()->Global(), aq->argc, argv);
}

@interface FakeServiceCallback : NSObject<FakeServiceDelegate>

@end

@implementation FakeServiceCallback

- (void)onFrontConnected
{
    AsyncRequest *asyncReq = new AsyncRequest;
    asyncReq->request.data = asyncReq;
    asyncReq->key = "onFrontConnected";
    uv_queue_work(uv_default_loop(), &(asyncReq->request), worker_cb, after_worker_cb);
}

- (void)onCountUpdate:(NSInteger)count
{
    printf("test by Hays in on count update: %ld \n", count);

    AsyncRequest *asyncReq = new AsyncRequest;
    asyncReq->request.data = asyncReq;
    asyncReq->key = "onCountUpdate";
    asyncReq->argc = 2;
    asyncReq->argv = @[@(count)];
    uv_queue_work(uv_default_loop(), &(asyncReq->request), worker_cb, after_worker_cb);
}

- (void)onFrontDisConnected
{
    AsyncRequest *asyncReq = new AsyncRequest;
    asyncReq->request.data = asyncReq;
    asyncReq->key = "onFrontDisConnected";
    uv_queue_work(uv_default_loop(), &(asyncReq->request), worker_cb, after_worker_cb);
}

@end

void Register(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    persistData = new ShareData;
    persistData->isolate = isolate;
    persistData->js_callback.Reset(isolate, Local<Function>::Cast(args[0]));

    delegate = [[FakeServiceCallback alloc] init];
    service = [[FakeService alloc] initWithDelegate: delegate];
}

void Test1(const FunctionCallbackInfo<Value> &args) {
    [service test1];
}

void Test2(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    NSInteger ret = [service test2];
    args.GetReturnValue().Set(Integer::New(isolate, ret));
}

void Test3(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    if (args.Length() > 0 && args[0]->IsString()) {
        Local<String> param = args[0]->ToString(context).ToLocalChecked();
        String::Utf8Value str(isolate, param);
        std::string cstr = std::string(*str);
        [service test3:[NSString stringWithCString:cstr.c_str() encoding:NSUTF8StringEncoding]];
    } else {
        printf("arg is not string");
    }
}

void Test4(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    
    if (args.Length() == 3) {
        int arg1 = args[0].As<Integer>()->Value();
        String::Utf8Value str(isolate, args[1]->ToString(context).ToLocalChecked());
        std::string cstr = std::string(*str);
        NSString *arg2 = [NSString stringWithCString:cstr.c_str() encoding:NSUTF8StringEncoding];
        Local<v8::Boolean> localArg3 = Local<v8::Boolean>::Cast(args[2]);
        bool arg3 = localArg3->Value();
        [service test4:arg1 text:arg2 isTrue:arg3];
    } else {
        printf("test4 need 3 args");
    }
}

void  AddSubView(const FunctionCallbackInfo<Value> &args) {
    v8::Local<v8::Object> handleBuffer = args[0].As<v8::Object>();
    char* bufferData = (char*)node::Buffer::Data(handleBuffer);
    NSView* view = *reinterpret_cast<NSView**>(bufferData);
    [service addSubView:view];
}

void Init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "register", Register);
    NODE_SET_METHOD(exports, "test1", Test1);
    NODE_SET_METHOD(exports, "test2", Test2);
    NODE_SET_METHOD(exports, "test3", Test3);
    NODE_SET_METHOD(exports, "test4", Test4);
    NODE_SET_METHOD(exports, "addSubView", AddSubView);
}

NODE_MODULE(demoaddon, Init)