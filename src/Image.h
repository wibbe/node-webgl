#ifndef IMAGE_H_
#define IMAGE_H_

#include <v8.h>
#include <node.h>
#include <string>
#include <FreeImage.h>

using namespace v8;
using namespace node;

class Image : public EventEmitter
{
  public:
    Image()
      : EventEmitter()
    { }

    virtual ~Image()
    {
	    if (m_image)
	      FreeImage_Unload(m_image);
    }

    static void initialize(Handle<Object> target)
    {
      HandleScope scope;

      Local<FunctionTemplate> t = FunctionTemplate::New(constructor);

      t->Inherit(EventEmitter::constructor_template);
      t->InstanceTemplate()->SetInternalFieldCount(1);

    	t->PrototypeTemplate()->SetAccessor(String::New("width"), WidthGetter);
    	t->PrototypeTemplate()->SetAccessor(String::New("height"), HeightGetter);
    	t->PrototypeTemplate()->SetAccessor(String::New("src"), SrcGetter, SrcSetter);

      NODE_SET_PROTOTYPE_METHOD(t, "addEventListener", AddEventListener);

    	target->Set(String::NewSymbol("Image"), t->GetFunction());
    }

    int getWidth()
    {
      return FreeImage_GetWidth(m_image);
    }

    int getHeight()
    {
	    return FreeImage_GetHeight(m_image);
    }

    void * getData()
    {
      BYTE * pixels = FreeImage_GetBits(m_image);

      // FreeImage stores data in BGR
      // Convert from BGR to RGB
      for(int i = 0; i < FreeImage_GetWidth(m_image) * FreeImage_GetHeight(m_image); i++)
      {
        BYTE temp = pixels[i * 4 + 0];
        pixels[i * 4 + 0] = pixels[i * 4 + 2];
        pixels[i * 4 + 2] = temp;
      }

	    return pixels;
    }

    void load(std::string const& filename)
    {
    	m_filename = filename;

    	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(m_filename.c_str(), 0);
    	m_image = FreeImage_Load(format, m_filename.c_str(), 0);
    	m_image = FreeImage_ConvertTo32Bits(m_image);
    }

  protected:
    static Handle<Value> constructor(Arguments const& args)
    {
    	HandleScope scope;

    	Image * image = new Image();
    	image->Wrap(args.This());

    	return args.This();
    }

    static Handle<Value>
    WidthGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());

	return scope.Close(Integer::New(image->getWidth()));
    }


    static Handle<Value>
    HeightGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());

	return scope.Close(Integer::New(image->getHeight()));
    }


    static Handle<Value>
    SrcGetter (Local<String> property, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());

	return scope.Close(String::New(image->m_filename.c_str()));
    }

    static void
    SrcSetter (Local<String> property, Local<Value> value, const AccessorInfo& info) {
	HandleScope scope;

	Image *image = ObjectWrap::Unwrap<Image>(info.This());
	String::Utf8Value filename_s(value->ToString());
	image->load(*filename_s);

	image->Emit(String::New("load"), 0, NULL);
    }

    static Handle<Value>
    AddEventListener (const Arguments& args) {
        HandleScope scope;

	// Fallback to addListener

	Local<Value> f_v = args.This()->Get(String::New("addListener"));
	if (!f_v->IsFunction()) return Undefined();

	Local<Function> f = Local<Function>::Cast(f_v);

	Handle<Value> values[args.Length()];
	for(int i = 0; i < args.Length(); i++) {
	    values[i] = args[i];
	}
	
	return f->Call(args.This(), args.Length(), values);
    }

  private:
    FIBITMAP * m_image;
    std::string m_filename;
    void * m_data;
};

#endif  // IMAGE_H_
