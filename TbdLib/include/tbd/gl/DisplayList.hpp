#pragma once

#include <vector>
#include "types.hpp"

namespace tbd
{
  namespace gl
  {
    struct DisplayList
    {
      DisplayList() : id_(0) {}
      ~DisplayList() { free(); }

      inline bool initialize()
      {
        if (id_) return false; // List is already initialized
        id_ = glGenLists(1);
        return true;
      }
      
      bool valid() const
      {
        return id_ != 0;
      }

      operator bool() const
      {
        return valid();
      }



      inline void free()
      {
        if (!id_) return;
        glDeleteLists(id_,1);
        id_ = 0;
      }

      inline void call() const
      {
        glCallList(id_);
      }

      TBD_PROPERTY_RO(GLuint,id)
    };

    struct DisplayLists
    {
      DisplayLists() : id_(0), number_(0) {}
      DisplayLists(GLuint _number) 
      {
        initialize(_number);
      }
      ~DisplayLists() { free(); }

      inline void free()
      {
        if (!id_) return;
        glDeleteLists(id_,number_);
        id_= 0;
      }

      bool valid() const
      {
        return id_ != 0;
      }

      operator bool() const
      {
        return valid();
      }

      inline bool initialize(GLuint _number)
      {
        if (!_number) return false;
        if (id_ && (number_==_number)) return false; // Lists are already initialized
        if (number_!=_number) free();

        id_ = glGenLists(_number);
        number_=_number;
        return true;
      }

      inline void call() const
      {
        std::vector<GLuint> _lists(number_);
        for (GLuint _i = 0; _i < number_; ++_i)
          _lists[_i] = _i;
        call(_lists);
      }

      template<typename LISTS>
      void call(LISTS const& _lists) const
      {
        if (_lists.empty()) return;
        glListBase(id_);
        glCallLists(_lists.size(), type_id<typename LISTS::value_type>(),_lists.data());
        glListBase(0);
      }

      TBD_PROPERTY_RO(GLuint,id)
      TBD_PROPERTY_RO(GLuint,number)
    };


    template<typename DISPLAY_LIST, typename FUNCTOR>
    void compile_list(DISPLAY_LIST const& _list, FUNCTOR _f)
    {
      glNewList(_list.id(),GL_COMPILE);
      _f();
      glEndList();
    }

    template<typename DISPLAY_LISTS, typename FUNCTOR>
    void compile_lists(DISPLAY_LISTS const& _lists, FUNCTOR _f)
    {
      for (GLuint _i = 0; _i < _lists.number(); ++_i)
      {
        glNewList(_lists.id() + _i,GL_COMPILE);
        _f(_i);
        glEndList();
      }
    }
  }
}
