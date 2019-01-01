#include <memory>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

struct FileAccessor {
  using Path = std::string;

  virtual std::unique_ptr<std::istream> maybeOpenForRead(const Path &) = 0;
};


struct SystemFiles : FileAccessor {
  std::unique_ptr<std::istream> maybeOpenForRead(const Path &path) override
  {
    auto maybe_stream = std::make_unique<std::ifstream>(path);
    std::ifstream &s = *maybe_stream;

    if (!s) {
      return {};
    }

    return maybe_stream;
  }
};


struct FakeFiles : FileAccessor {
  std::map<Path,std::string> text_map;

  void store(const Path &path,const std::string &text)
  {
    text_map[path] = text;
  }

  std::unique_ptr<std::istream> maybeOpenForRead(const Path &path) override
  {
    auto iter = text_map.find(path);

    if (iter==text_map.end()) {
      return {};
    }

    const std::string &text = iter->second;
    return std::make_unique<std::istringstream>(text);
  }
};
