#pragma once

#include <fstream>

namespace tomo
{
  namespace item
  {
    class FileSourceInterface 
    {
    public:
      virtual void load(std::string const& _path) = 0;
      void reload() 
      {
        load(path());
      }
      virtual std::string const& path() const = 0; 
    };

    template<typename ITEM, typename LOADER>
    struct FileSource : public FileSourceInterface 
    {
      typedef std::ifstream stream_type;
      typedef LOADER loader_type;
      typedef ITEM item_type;

      FileSource(item_type& _item) :
        item_(_item) {}

      explicit FileSource(const std::string& _path, item_type* _item = nullptr) :
        item_(_item) 
      {
        load(_path);
      }
 
      void load(std::string const& _path)
      {
        path_=_path;
        loader_type()(path_,item_);
      }

      operator bool() const
      {
        return !std::string(path_).empty();
      }

      static Id typeId() { return "FileSource"; }

      TBD_PROPERTY_REF_RO(std::string,path)
    private:
      item_type& item_;
    };
  }
    
  using item::FileSource;
}

namespace tbd
{
  template<typename ITEM,typename LOADER>
  struct TypeInfo<tomo::item::FileSource<ITEM,LOADER>>
  {
    typedef std::string token_type;
    inline token_type operator()()
    {
      return tomo::item::FileSource<ITEM,LOADER>::typeId();
    }
  };
}


