#pragma once

#define TBD_GL_TRAITS_CALL(call)\
    template<typename...ARGS>\
    inline void operator()(ARGS..._args)\
    {\
      call(_args...);\
    }

#define TBD_GL_TRAITS(traitname,call,...)\
  template<>\
  struct traitname<__VA_ARGS__>\
  {\
    TBD_GL_TRAITS_CALL(call)\
  };
