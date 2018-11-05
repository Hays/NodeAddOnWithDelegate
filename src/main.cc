#include <node/node.h>
#include <node/uv.h>
#include <node/v8.h>
#import <Foundation/Foundation.h>
#import <Fake/Fake.h>

using namespace v8;

struct ShareData
{
    uv_work_t request;
    Isolate *isolate;
    Persistent<Function> js_callback;
};

static ShareData *persistData;

void worker_cb(uv_work_t *req) {
    printf("test by Hays in worker cb.....\n");
}

void after_worker_cb(uv_work_t *req, int status) {
    ShareData * my_data = static_cast<ShareData *>(req->data);
    Isolate * isolate = my_data->isolate;
    HandleScope scope(isolate);
    Local<Function> js_callback = Local<Function>::New(isolate,my_data->js_callback);
    js_callback->Call(isolate->GetCurrentContext()->Global(), 0, NULL);
}

void Register(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    persistData = new ShareData;
    persistData->request.data = persistData;
    persistData->isolate = isolate;
    persistData->js_callback.Reset(isolate, Local<Function>::Cast(args[0]));
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        printf("test by Hays in cpp .... \n");
        uv_queue_work(uv_default_loop(), &(persistData->request), worker_cb, after_worker_cb);
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3 * NSEC_PER_SEC)), dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            printf("test by Hays in cpp .... 2 \n");
            uv_queue_work(uv_default_loop(), &(persistData->request), worker_cb, after_worker_cb);
        });
    });
}

void Init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "register", Register);
}

NODE_MODULE(demoaddon, Init)