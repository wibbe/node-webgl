
#include <v8.h>
#include <node.h>
#include <node_events.h>
#include <FreeImage.h>
#include <GL/glfw.h>

#include "GLContext.h"
#include "Image.h"

using namespace v8;
using namespace node;

static bool isWindowClosed = true;

static int windowClosedCallback()
{
  isWindowClosed = true;
  return 1;
}

static Handle<Value> initialize(Arguments const& args)
{
  HandleScope scope;
  
  int width = args.Length() >= 1 && args[0]->IsInt32() ? args[0]->Int32Value() : 640;
  int height = args.Length() >= 2 && args[1]->IsInt32() ? args[1]->Int32Value() : 480;
  int fullscreen = args.Length() == 3 && args[2]->IsBoolean() && args[2]->BooleanValue() ? GLFW_FULLSCREEN : GLFW_WINDOW;
  
  FreeImage_Initialise(true);
  
  glfwInit();
  glfwOpenWindow(width, height, 8, 8, 8, 0, 24, 0, fullscreen);
  
  glfwSetWindowCloseCallback(windowClosedCallback);
  
  isWindowClosed = false;
  
  return scope.Close(Boolean::New(true));
}

static Handle<Value> destroy(Arguments const& args)
{
  HandleScope scope;
  
  glfwCloseWindow();
  glfwTerminate();
  
  FreeImage_DeInitialise();

  return scope.Close(Boolean::New(true));
}

static Handle<Value> getContext(Arguments const& args)
{
  HandleScope scope;

  Handle<Object> context = GLContext::newInstance();
  return scope.Close(context);
}

static Handle<Value> tick(Arguments const& args)
{
  HandleScope scope;
  
  glfwPollEvents();
  
  return scope.Close(Boolean::New(!isWindowClosed));
}

extern "C" void init(Handle<Object> target)
{
  HandleScope scope;
  
  NODE_SET_METHOD(target, "initialize", initialize);
  NODE_SET_METHOD(target, "destroy", destroy);
  NODE_SET_METHOD(target, "getContext", getContext);
  NODE_SET_METHOD(target, "tick", tick);

  GLContext::initialize(target);
  Image::initialize(target);
}
