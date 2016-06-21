#pragma once

#include <gex/index.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include "Item.hpp"

namespace tomo
{
  namespace item
  {
    struct RegionsLoader
    {
      template<typename PATH, typename ITEM>
      bool operator()(const PATH& _path, ITEM& _item)
      {
        _item.clear();
        std::ifstream _is(_path);
        std::string _line;
        while (std::getline(_is, _line))
        {
          std::vector<std::string> _split;
          boost::algorithm::split(_split,_line,boost::is_any_of("="));
          if (_split.size() != 2) continue;
          std::istringstream _boundsStr(_split[0]);
          std::istringstream _valueStr(_split[1]);
          gex::Bounds3 _bounds;
          gex::Scalar _value = 1.0;
          _boundsStr >> _bounds.min().x();
          _boundsStr >> _bounds.min().y();
          _boundsStr >> _bounds.min().z();
          _boundsStr >> _bounds.max().x();
          _boundsStr >> _bounds.max().y();
          _boundsStr >> _bounds.max().z();

          _valueStr >> _value;
          if (!_bounds.valid()) continue;
          _item.insert(_bounds,_value);
        }
        return true;
      }
    };

    class Regions : public Item<Regions,RegionsLoader>
    {
      TOMO_PARAMETER_LIST
      (
        (bool) invert
      )
    public:
      typedef gex::RTree3Node<gex::Scalar> node_type;
      typedef gex::Bounds3 bounds_type;
      TOMO_ITEM("generic",Regions)

      friend RegionsLoader;

      Regions(parent_type const* _parent) : 
        Item<Regions,RegionsLoader>(_parent),
        invert_(false) {}

      void clear()
      {
        data_.clear();
      }

      void insert(const bounds_type& _bounds, const gex::Scalar& _value)
      {
        data_.insert({_bounds,_value});
      }

      bool loadFromFile(std::string const& _filename)
      {
        return RegionsLoader()(_filename,*this);
      }

      TBD_PROPERTY_REF_RO(gex::RTree3<gex::Scalar>,data)
    };
  }
}
