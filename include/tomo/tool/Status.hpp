#pragma once

namespace tomo
{
  namespace tool
  {
    class Status
    {
    public:
      typedef enum { LOCKED, UNLOCKED } lockedstate_type;

      Status(Interface const* _interface) : 
        interface_(_interface) {}

      bool valid() const
      {
        return interface_->valid();
      }

      TBD_PROPERTY_REF_RO(float,progress)
    
    private:
      Interface const* interface_;
    };
  }
}
