#include "st.h"
#include "ruby.h"
#include "intern.h"
#include <flixengine2/flixengine2.h>

/* Ruby Declartions */
static VALUE mOn2;
static VALUE cFlixEngine;
static VALUE eFlixEngineError;

/* Globals */
FLIX2HANDLE flix;
	
/* Helper functions */
#define CHECKSC(func) {\
    on2sc sc;\
    if((sc= func)) {\
        on2sc esc;\
        FE2_errno flixerr = ErrNone;\
        on2s32 syserr = 0;\
        /*retrieve information if possible*/\
        esc = Flix2_Errno(flix,&flixerr,&syserr);\
				char *message = malloc(500);\
				snprintf(message, 500, "errors calling %s, Flix2_Errno: sc:%d %s:%d syserrno:%d\n",#func, esc,(sc==ON2_NET_ERROR?"rpcerr":"flixerrno"),flixerr,syserr);\
        rb_raise(eFlixEngineError, message);\
				free(message);\
    }\
}


/* contact flixd on rpchost w/timeout.
   A port may be specified by giving rpchost in the form 'server:port'
   to avoid making a connection to portmap before contacting flixd.
   This is only useful if flixd is being run with the --port option.
   If timeout is 0 rpc's default timeout will be used (typically 25s) */
VALUE FlixEngine_init(VALUE self, VALUE _rpchost, VALUE _timeout )
{	
  CHECKSC( Flix2_CreateEx(&flix, StringValueCStr(_rpchost), NUM2INT(_timeout)) )
  return self;
}

VALUE FlixEngine_finalize(VALUE self)
{	
  CHECKSC( Flix2_Destroy(flix) );
  return Qnil;
}

VALUE FlixEngine_version(VALUE self)
{
  return rb_str_new2(Flix2_Version());
}

VALUE FlixEngine_copyright(VALUE self)
{
  return rb_str_new2(Flix2_Copyright());
}

VALUE FlixEngine_set_source_path(VALUE self, VALUE _source_path)
{
  rb_iv_set(self, "source_path", _source_path);
  CHECKSC( Flix2_SetInputFile(flix, StringValueCStr(_source_path)) );
  return Qnil;
}

VALUE FlixEngine_get_source_path(VALUE self)
{
  return rb_iv_get(self, "source_path");
}

VALUE FlixEngine_set_output_path(VALUE self, VALUE _output_path)
{
  rb_iv_set(self, "output_path", _output_path);
  CHECKSC( Flix2_SetOutputFile(flix, StringValueCStr(_output_path)) )
  return Qnil;
}

VALUE FlixEngine_get_output_path(VALUE self)
{
  return rb_iv_get(self, "output_path");
}

VALUE FlixEngine_encode(VALUE self)
{
  CHECKSC( Flix2_Encode(flix) )
  return Qnil;
}

VALUE FlixEngine_encoding(VALUE self)
{
  on2bool b;
  CHECKSC( Flix2_IsEncoderRunning(flix,&b) )
  return b == 1 ? Qtrue : Qfalse;
}

VALUE FlixEngine_percent_complete(VALUE self)
{
  int32_t p;
  CHECKSC( encoding_status_PercentComplete(flix,&p) )
  return INT2FIX(p); 
}

VALUE FlixEngine_get_duration(VALUE self)
{
  on2s32 p;
  CHECKSC( Flix2_GetSourceDuration(flix,&p) )
  return INT2FIX(p); 
}

VALUE FlixEngine_get_state(VALUE self)
{
  FE2_EncState state;

  char *status;
  CHECKSC( Flix2_GetEncoderState(flix,&state) )
  if(state == EncStateIdle)    { status = "idle"; }
  if(state == EncStateRunning) { status = "running"; }
  if(state == EncStateQueued)  { status = "queued"; }
  return rb_str_new2(status);
}

VALUE FlixEngine_get_success(VALUE self)
{
  FE2_errno flixerr;
  on2s32 syserr;

  CHECKSC( Flix2_Errno(flix,&flixerr,&syserr) )
  return (flixerr == 0 && syserr == 0) ? Qtrue : Qfalse;
}

VALUE FlixEngine_get_system_errors(VALUE self)
{
  FE2_errno flixerr;
  on2s32 syserr;

  CHECKSC( Flix2_Errno(flix,&flixerr,&syserr) )
  return INT2FIX(syserr);
}

VALUE FlixEngine_get_flix_errors(VALUE self)
{
  FE2_errno flixerr;
  on2s32 syserr;

  CHECKSC( Flix2_Errno(flix,&flixerr,&syserr) )
  return INT2FIX(flixerr);
}

VALUE FlixEngine_enable_flv(VALUE self)
{
  FLIX2PLGNHANDLE codec;
  FLIX2PLGNHANDLE muxer;

  CHECKSC( Flix2_AddCodec(&codec, flix, FE2_CODEC_VP6) )
  CHECKSC( Flix2_CodecSetParam(codec, FE2_VP6_BITRATE, 450.0) )
  CHECKSC( Flix2_AddMuxer(&muxer, flix, FE2_MUXER_FLV) )
}

VALUE FlixEngine_validate(VALUE self)
{
  CHECKSC( Flix2_Validate(flix) )
}


// filters
VALUE FlixEngine_set_scale_filter(VALUE self, VALUE filters)
{
  FLIX2PLGNHANDLE filter;
  CHECKSC( Flix2_AddFilter(&filter,flix,FE2_FILTER_SCALE) );
  
  VALUE width = rb_hash_aref(filters, ID2SYM(rb_intern("width")));
  if(TYPE(width) == T_FIXNUM) {
    CHECKSC( Flix2_FilterSetParam(filter,FE2_SCALE_WIDTH, NUM2INT(width)));
  }

  VALUE height = rb_hash_aref(filters, ID2SYM(rb_intern("height")));
  if(TYPE(height) == T_FIXNUM) {
    CHECKSC( Flix2_FilterSetParam(filter,FE2_SCALE_HEIGHT, NUM2INT(height)));
  }
  return Qnil;
}

VALUE FlixEngine_set_png_filter(VALUE self, VALUE filters)
{
  FLIX2PLGNHANDLE filter;
  CHECKSC( Flix2_AddFilter(&filter,flix,FE2_FILTER_PNGEX) );
  
  VALUE width = rb_hash_aref(filters, ID2SYM(rb_intern("width")));
  if(TYPE(width) == T_FIXNUM) {
    CHECKSC( Flix2_FilterSetParam(filter,FE2_PNGEX_WIDTH, NUM2INT(width)));
  }

  VALUE height = rb_hash_aref(filters, ID2SYM(rb_intern("height")));
  if(TYPE(height) == T_FIXNUM) {
    CHECKSC( Flix2_FilterSetParam(filter,FE2_PNGEX_HEIGHT, NUM2INT(height)));
  }

  VALUE export_time = rb_hash_aref(filters, ID2SYM(rb_intern("export_time")));
  if(TYPE(export_time) == T_STRING) {
    CHECKSC( Flix2_FilterSetParamAsStr(filter,FE2_PNGEX_EXPORT_TIME_STRING, StringValueCStr(export_time)));
  }

  VALUE prefix = rb_hash_aref(filters, ID2SYM(rb_intern("prefix")));
  if(TYPE(prefix) == T_STRING) {
    CHECKSC( Flix2_FilterSetParamAsStr(filter,FE2_PNGEX_FILENAME_PREFIX, StringValueCStr(prefix)));
  }
  
  VALUE output_directory = rb_hash_aref(filters, ID2SYM(rb_intern("output_directory")));
  if(TYPE(output_directory) == T_STRING) {
    CHECKSC( Flix2_FilterSetParamAsStr(filter,FE2_PNGEX_OUTPUT_DIRECTORY, StringValueCStr(output_directory)));
  }

  VALUE suffix = rb_hash_aref(filters, ID2SYM(rb_intern("suffix")));
  if(TYPE(suffix) == T_STRING) {
    CHECKSC( Flix2_FilterSetParamAsStr(filter,FE2_PNGEX_FILENAME_SUFFIX, StringValueCStr(suffix)));
  }
  return Qnil;
}

void Init_flix_engine()
{
  mOn2 = rb_define_module("On2");

  eFlixEngineError = rb_define_class_under(mOn2, "FlixEngineError", rb_eStandardError);

  cFlixEngine = rb_define_class_under(mOn2, "FlixEngine", rb_cObject);
  rb_define_method(cFlixEngine, "initialize", FlixEngine_init, 2);
  rb_define_method(cFlixEngine, "finalize", FlixEngine_finalize, 0);
  rb_define_method(cFlixEngine, "version", FlixEngine_version, 0);
  rb_define_method(cFlixEngine, "copyright", FlixEngine_copyright, 0);
  rb_define_method(cFlixEngine, "source_path=", FlixEngine_set_source_path, 1);
  rb_define_method(cFlixEngine, "source_path", FlixEngine_get_source_path, 0);
  rb_define_method(cFlixEngine, "output_path=", FlixEngine_set_output_path, 1);
  rb_define_method(cFlixEngine, "output_path", FlixEngine_get_output_path, 0);
  rb_define_method(cFlixEngine, "success?", FlixEngine_get_success, 0);
  rb_define_method(cFlixEngine, "flix_error", FlixEngine_get_flix_errors, 0);
  rb_define_method(cFlixEngine, "system_error", FlixEngine_get_system_errors, 0);
  rb_define_method(cFlixEngine, "state", FlixEngine_get_state, 0);
  rb_define_method(cFlixEngine, "duration", FlixEngine_get_duration, 0);
  rb_define_method(cFlixEngine, "scale", FlixEngine_set_scale_filter, 1);
  rb_define_method(cFlixEngine, "png", FlixEngine_set_png_filter, 1);
  rb_define_method(cFlixEngine, "encode", FlixEngine_encode, 0);
  rb_define_method(cFlixEngine, "encoding?", FlixEngine_encoding, 0);
  rb_define_method(cFlixEngine, "percent_complete", FlixEngine_percent_complete, 0);
  rb_define_method(cFlixEngine, "enable_flv", FlixEngine_enable_flv, 0);
  rb_define_method(cFlixEngine, "valid?", FlixEngine_validate, 0);
}
