// Copyright (c) 2014 Intel Corporation.
// Author:  LI,Binfei (binfeix.li@intel.com)

#ifndef UTILS_FILESYSTEM_H
#define UTILS_FILESYSTEM_H

#include <list>
#include <memory>
#include <string>

//-------------------------------------------------------------------------------------------------
namespace avatar {
namespace utils {

//-------------------------------------------------------------------------------------------------
//path merge
class path {
public:
  explicit path(
    const ::std::string& file, const ::std::string parent = "", bool isfolder = false);
  bool isfolder() {
    return isfolder_;
  }

  ::std::string name() const {
    if (parent_.empty())
      return file_;
    else 
      return parent_ + "/" + file_;
  }

  ::std::string extension() const {
    size_t pos = file_.rfind('.');
    if (pos != ::std::string::npos) {
      return file_.substr(pos, file_.length() - pos);
    }

    return "";
  }

private:
  ::std::string parent_;
  ::std::string file_;
  bool isfolder_;
};

//path detect
class path2 {
public:
  explicit path2(const ::std::string& file): file_(file) {
    size_t index = file_.rfind('/');
    if (index != ::std::string::npos) {
      parent_ = file_.substr(0, index);
      name_ = file_.substr(index, file_.length() - index);
    }
    size_t pos = file_.rfind('.');
    if (pos != ::std::string::npos) {
      extension_ = file_.substr(pos, file_.length() - pos);
    }
  }

  bool exists() const;
  const ::std::string& name() const {
    return name_;
  }
  const ::std::string& parent() const {
    return parent_;
  }
  const ::std::string& extension() const {
    return extension_;
  }

private:
  ::std::string parent_;
  ::std::string name_;
  ::std::string extension_;
  ::std::string file_;
};

//-------------------------------------------------------------------------------------------------
//folder
class filesystem_entry;
class directory_iterator 
  : public ::std::iterator<std::input_iterator_tag, void, void, void, void> {
 public:
  explicit directory_iterator(const ::std::string& file);
  directory_iterator();
  ~directory_iterator();

  path operator*();
  directory_iterator& operator++();
  bool operator!=(const directory_iterator& other) const;
  bool operator==(const directory_iterator& other) const {
    return !(*this != other);
  }

 private:
  ::std::shared_ptr<filesystem_entry> entry_;
};

//-------------------------------------------------------------------------------------------------
class recursive_directory_iterator 
  : public ::std::iterator<std::input_iterator_tag, void, void, void, void> {
 public:
  explicit recursive_directory_iterator(const ::std::string& file);
  recursive_directory_iterator();
  ~recursive_directory_iterator();

  path operator*();
  recursive_directory_iterator& operator++();
  bool operator!=(const recursive_directory_iterator& other) const;
  bool operator==(const recursive_directory_iterator& other) const {
    return !(*this != other);
  }
  
 private:
  ::std::list<std::pair<std::string, ::std::shared_ptr<filesystem_entry> > > entrys_;
};

} //! namespace avatar
} //! namespace utils

#endif //! UTILS_FILESYSTEM_H
