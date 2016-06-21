#pragma once

#include <sstream>
#include <tbd/parameter.h>

namespace tomo
{
  namespace base
  {
    namespace util
    {
      template<typename T>
      struct Formatter
      {
        std::string operator()(const T& _t)
        {
          std::stringstream _ss;
          _ss << _t;
          return _ss.str();
        }
      };

      template<>
      struct Formatter<std::string>
      {
        inline std::string operator()(const std::string& _t)
        {
          std::stringstream _ss;
          _ss << '"' << _t << '"';
          return _ss.str();
        }
      };

      template<typename T>
      std::string format(const T& _t)
      {
        return Formatter<T>()(_t);
      }

      template<typename OSTREAM, typename ITERATOR, typename PREFIX, typename SUFFIX>
      void print(
          OSTREAM& _os, 
          const ITERATOR& _begin, 
          const ITERATOR& _end, 
          const PREFIX& _prefix,
          const SUFFIX& _suffix,
          char _separator = ',')
      {
        if (_begin == _end) { _os << _prefix << _suffix; return; }
        _os << _prefix;
        for (auto it = _begin; it != _end; ++it)
        {
          _os << *it;
          if (it == _end) _os << _suffix; else _os << _separator;
        }
      }

/*
      template<typename PARAMETERSET>
      bool printParameters(std::ostream& _os, const PARAMETERSET& _p)
      {
        std::vector<std::string> _tokens;
        _p.apply([&](const tbd::ParameterInterface& _param)
        {
          if (_param.valueAsStr() == _param.defaultAsStr()) return;
          _tokens.push_back(_param.token());
        });
            
        if (_tokens.empty()) return false;
        for (auto it = _tokens.begin(); it != --_tokens.end(); ++it)
        {
          _os << (*it) << ',';
        }
        _os << _tokens.back();
        return true;
      }


      template<typename ELEMENT>
      std::string indent(const ELEMENT& _element, int _offset = 0)
      {
        return std::string(_element.depth() + _offset,'\t');
      }*/
    }
  }
}

