#include <tomo/tool/Tool.hpp>

#include <tomo/item/SourceAndSink.hpp>

#include <tomo/item/Regions.hpp>



namespace tomo
{
  using namespace gex;

  class RegionOperation :
    public Tool,
    public Serializer<RegionOperation>
  {
    TBD_PARAMETER_LIST
    (
      (MultiSink<item::Regions>) a,
      (MultiSink<item::Regions>) b,
      (SingleSink<item::Regions>) sink,
      (String) operation
    )
  public:
    typedef Serializer<RegionOperation> serializer_type;
    typedef Tool base_type;
    TOMO_TOOL(RegionOperation)

    RegionOperation(mutable_parent_type _parent) :
      base_type(_parent),
      serializer_type(*this),
      a_(_parent),
      b_(_parent),
      sink_(_parent),
      operation_("intersect")
    {}

    bool valid() const
    {
      return a() && b() && sink();
    }

    void process()
    {
      std::string _op = operation_;

      using namespace gex;
      typedef RTree3Node<Scalar> node_type;

      std::vector<node_type> _nodes;
      auto _getAll = [](const node_type& _node) { return true; };
      
      for (auto& _a : a())
      {
        _a->data().query(index::satisfies(_getAll),std::back_inserter(_nodes));
      }

      std::vector<node_type> _outNodes;

      if (_op == "merge")
        _outNodes = _nodes;


      for (auto& _node : _nodes)
      {
        if (_op == "merge")
        {       
          for (auto& _b : b())
          {
            _b->data().query(index::satisfies(_getAll),std::back_inserter(_outNodes));
          }
        } else
        if (_op == "intersect")
        {
          for (auto& _b : b())
          {
            _b->data().query(index::intersects(_node.first),std::back_inserter(_outNodes));
          }
        }
      }

      for (auto& _node : _outNodes)
      {
        sink_->insert(_node.first,_node.second);
      }
    }
  };
}

TOMO_TOOL_PLUGIN_INTERFACE(tomo::RegionOperation)
