// Copyright (c) 2014 Intel Corporation.
// Author:  LI,Binfei (binfeix.li@intel.com)

#include "utils/filesystem.h"

#include <string>

#ifndef _WIN32
#include <unistd.h> 
#include <fcntl.h>
#include <dirent.h>
#else
#include <io.h>
#define access _access
#define PATH_EOF -1
#endif

using namespace std;

//-------------------------------------------------------------------------------------------------
namespace pa2davatar {
namespace utils {

//-------------------------------------------------------------------------------------------------
path::path(
  const string &file, const ::std::string &parent, bool isfolder) 
  : file_(file), parent_(parent), isfolder_(isfolder) {
}

bool path::exists() {
  return !access(file_.c_str(), 0);
}

const string path::extension() const {
  size_t pos = file_.rfind('.');
  if (pos != string::npos) {
    return file_.substr(pos, file_.length() - pos);
  }

  return "";
}

//-------------------------------------------------------------------------------------------------
#ifndef _WIN32
class filesystem_entry {
 public:
  filesystem_entry() : handle_(nullptr), pathInfo_(nullptr) {
  }
  explicit filesystem_entry(const ::std::string& file) {
    handle_ = opendir(file.c_str());
    if (handle_ != nullptr) {
      //strip "." and ".."
      pathInfo_ = readdir(handle_); // strip "."
      pathInfo_ = readdir(handle_); // strip ".."
      pathInfo_ = readdir(handle_);
    }
  }
  filesystem_entry(filesystem_entry& entry) {
    if (this != &entry) {
      closedir(handle_);
      handle_ = entry.handle_;
      pathInfo_ = entry.pathInfo_;
    }
  }
  ~filesystem_entry() {
    release();
  }
  void release() {
    if (handle_ != nullptr)
      closedir(handle_);
    handle_ = nullptr;
    pathInfo_ = nullptr;
  }
  

  filesystem_entry& next() {
    pathInfo_ = readdir(handle_);
    if (pathInfo_ == nullptr) {
      release();
    }
    return *this;
  }
  
  bool valid() {
    if (handle_ == nullptr)
      return false;
    return true;
  }
  path get() {
    bool isfolder = (pathInfo_->d_type == DT_DIR);
    return path(pathInfo_->d_name, "", isfolder);
  }

  bool operator==(const filesystem_entry& entry) const {
    if (handle_ == entry.handle_) //TODO(binfei):just judgement handle
      return true;
    return false;
  }

 private:
  struct dirent* pathInfo_;
  DIR* handle_;
};
#else
class filesystem_entry {
 public:
  filesystem_entry() : handle_(PATH_EOF){
  }
  explicit filesystem_entry(const ::std::string& file) {
    memset(&pathInfo_, 0x0, sizeof(pathInfo_));
    //strip "." and ".."
    handle_ = _findfirst((file + "/*").c_str(), &pathInfo_);
    if (handle_ == PATH_EOF) 
      return;
    _findnext(handle_, &pathInfo_);
    if (_findnext(handle_, &pathInfo_)) {
      handle_ = PATH_EOF;
      memset(&pathInfo_, 0x0, sizeof(pathInfo_));
    }
  }
  filesystem_entry(const filesystem_entry& entry) {
    if (this != &entry) {
      _findclose(handle_);
      handle_ = entry.handle_;
      pathInfo_ = entry.pathInfo_;
    }
  }
  ~filesystem_entry() {
    release();
  }
  
  void release() {
    _findclose(handle_);
    handle_ = PATH_EOF;
  }

  filesystem_entry& next() {
    memset(&pathInfo_, 0x0, sizeof(pathInfo_));
    if (_findnext(handle_, &pathInfo_))
      release();
    return *this;
  }

  bool valid() {
    if (handle_ == PATH_EOF)
      return false;
    return true;
  }
  path get() {
    bool isfolder = ((pathInfo_.attrib & _A_ARCH)!=_A_ARCH);
    return path(pathInfo_.name, "", isfolder);
  }

  bool operator==(const filesystem_entry& entry) const {
    //TODO(binfei):just judgement handle,bug
    if (handle_ == entry.handle_) 
      return true;
    return false;
  }

 private:
  struct _finddata_t pathInfo_;
  intptr_t handle_;
};
#endif

//-------------------------------------------------------------------------------------------------
directory_iterator::directory_iterator(const ::std::string& file) {
  entry_.reset(new filesystem_entry(file));
}

directory_iterator::directory_iterator() {
  entry_.reset(new filesystem_entry());
}

directory_iterator::~directory_iterator() {
}

path directory_iterator::operator*() {
  return entry_->get();
}

directory_iterator& directory_iterator::operator++() {
  entry_->next();
  return *this;
}

bool directory_iterator::operator!=(const directory_iterator& other) const {
  return !(*other.entry_.get() == *entry_.get());
}

//-------------------------------------------------------------------------------------------------
recursive_directory_iterator::recursive_directory_iterator(const ::std::string& file) {
  string folder_parent = file;
  string folder_self = file;
  while (true) {
    shared_ptr<filesystem_entry> entry(new filesystem_entry(folder_self));
    entrys_.push_back(make_pair(folder_parent, entry));
    path _path = entry->get();
    if (!entry->valid() || !_path.isfolder())
      break;
    folder_self = folder_parent + "/" + _path.name();
    folder_parent = folder_self;
  }
}

recursive_directory_iterator::recursive_directory_iterator() {
}

recursive_directory_iterator::~recursive_directory_iterator() {
}

path recursive_directory_iterator::operator*() {
  shared_ptr<filesystem_entry> entry(entrys_.back().second);
  return path(entry->get().name(), entrys_.back().first);
}

recursive_directory_iterator& recursive_directory_iterator::operator++() {
  while (true) {
    if (entrys_.empty())
      break;

    shared_ptr<filesystem_entry> entry = entrys_.back().second;
    entry->next();
    if (entry->valid()) {
      path _path = entry->get();
      if (_path.isfolder()) {
        recursive_directory_iterator next(entrys_.back().first + "/" + _path.name());
        entrys_.insert(entrys_.end(), next.entrys_.begin(), next.entrys_.end());
      }
      break;
    }
    entrys_.pop_back();
  }

  return *this;
}

bool recursive_directory_iterator::operator!=(const recursive_directory_iterator& other) const {
  return !(other.entrys_.size() == entrys_.size());//TODO(binfei):just judgement size,bug
}

} //! namespace pa2davatar
} //! namespace utils
