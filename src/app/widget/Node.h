#pragma once

namespace tomo
{
  namespace widget
  {
    class ToolchainEditor;

    struct Node : QGraphicsItem
    {
      TBD_PROPERTY_REF_RO(std::list<Port*>,sourcePorts)
      TBD_PROPERTY_REF_RO(std::list<Port*>,sinkPorts)
      
      ToolchainEditor* parent();
      ToolchainEditor const* parent() const;

    private:
      ToolchainEditor* parent_;
    };
  }
}
