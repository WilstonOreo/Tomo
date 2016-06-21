#pragma once

#include <tbd/tuple_to_strings.h>
#include <tbd/property.h>

namespace tomo
{
  namespace base
  {
    struct Signal
    {
      typedef std::string message_type;

      enum Type{ 
        MOUSE_CLICK, 
        MOUSE_MOVE,
        MOUSE_WHEEL,
        KEY_PRESS,
        UPDATE,
        RESIZE
      };

      template<typename...ARGS>
      Signal(const Type& _type, ARGS&&..._args) : 
        type_(_type) 
      {
        messages_ = tbd::args_to_strings(_args...);
      }

      TBD_PROPERTY_REF_RO(Type,type)
      TBD_PROPERTY_REF_RO(std::vector<message_type>,messages)
    };

    class SignalInterface
    {
    public: 
      typedef std::vector<Signal::Type> signals_type;

      virtual ~SignalInterface() {}

      inline virtual void sendSignal(const Signal& _signal) 
      {
      }

      /// Convenience template 
      template<typename...ARGS>
      void signal(ARGS&&..._args) 
      {
        sendSignal(Signal(_args...));
      }

      /// Returns list of supported signals an element understands
      inline virtual signals_type supportedSignals() const 
      {
        return signals_type();
      }
    };
  }
}
