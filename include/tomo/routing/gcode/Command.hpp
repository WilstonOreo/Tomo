#pragma once

#include <sstream>
#include "tomo/base/assert.hpp"
#include <boost/algorithm/string/trim.hpp>
#include "Field.hpp"

namespace tomo
{
  namespace routing
  {
    namespace gcode
    {
      /// A GCode command holds a set of GCode fields
      struct Command :
        private std::vector<Field>
      {
        typedef std::vector<Field> container_type;

        using container_type::insert;
        using container_type::push_back;
        using container_type::emplace_back;
        using container_type::begin;
        using container_type::end;
        using container_type::clear;
        using container_type::erase;
        using container_type::empty;
        using container_type::reserve;
        using container_type::size;

        Command() {}

        Command(const container_type& _fields,
                const Token& _comment = Token()) :
          container_type(_fields),
          comment_(_comment) {}

        Command(const Token& _commandStr)
        {
          parse(_commandStr);
        }
        
        Command(const Field& _field)
        {
          this->push_back(_field);
        }

        template<typename...ARGS>
        Command& operator()(ARGS&&..._args)
        {
          this->emplace_back(_args...);
          return *this;
        }

        /// Parse a command from a string
        inline void parse(const Token& _commandStr)
        {
          comment_ = Token();
          auto _line = boost::algorithm::trim_copy(_commandStr);

          // Get comment comments
          size_t _commentPos = _line.find(';');
          if (_commentPos != Token::npos)
          {
            comment_ = _line.substr(_commentPos+1,_line.length());
            _line = _line.substr(0,_commentPos);
          }
          
          boost::algorithm::trim(_line);
          if (_line.empty()) return;

          // Get checksum token
          int _checksumToken = getChecksumToken(_line);
          bool _hasChecksum = _checksumToken != hasNoChecksum();
          if (_hasChecksum)
          {
            int _checksum = getChecksum(_line);
            TOMO_ASSERT(_checksum == _checksumToken);
          }

          // Get code fields tokens, separated by whitespace
          container_type::clear();
          if (_line.empty()) return;
          
          std::istringstream _iss(_line);
          Field _codeField;

          // Succeeding code fields are command parameters
          while (_iss.good())
          {
            _iss >> _codeField;
            if (!_codeField.empty())
            container_type::push_back(_codeField);
          }         
        }

        friend std::ostream& operator<<( std::ostream& _os, const Command& _c)
        {
          for (auto& _field : _c)
            _os << _field << " ";

          if (!_c.comment().empty())
            _os << " ; " << _c.comment();
          _os << std::endl;
          return _os;
        }

        TBD_PROPERTY_REF(Token,comment)
      private:
       /**@brief Algorithm to calculate checksum of a line
         * @detail Taken from http://reprap.org/wiki/GCode#Checking*/
        inline int getChecksum(const Token& _token) const
        {
          int _cs = 0;
          for (auto _c : _token)
            _cs = _cs ^ _c;
          _cs &= 0xff;
          return _cs;
        }
        
        inline static constexpr int hasNoChecksum() { return -1; }

        /**@brief Parse the rest of the line for a checksum token
         * @detail Returns -1 if line contains no checksum and
         *         otherwise, the value of the found checksum is returned*/
        inline int getChecksumToken(Token& _line) const
        {
          size_t _checksumPos = _line.find('*');
          if (_checksumPos != Token::npos)
          {
            Token _checksumInputStr = _line.substr(_checksumPos+1,_line.length());
            Token _checksumOutputStr;

            for (auto _c : _checksumInputStr)
              if (std::isdigit(_c)) _checksumOutputStr += _c;

            _line = _line.substr(0,_checksumPos);

            int _checksum;
            std::istringstream _is(_checksumInputStr);
            _is >> _checksum;
            return _checksum;
          }
          return hasNoChecksum();
        }
      }; 
    }
  }
}

