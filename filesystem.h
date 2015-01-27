// Copyright (c) 2014 Intel Corporation.
// Author:  LI,Binfei (binfeix.li@intel.com)

#ifndef UTILS_FILESYSTEM_H
#define UTILS_FILESYSTEM_H

#include <list>
#include <memory>
#include <string>
//#include <xutility>

//-------------------------------------------------------------------------------------------------
namespace pa2davatar {
namespace utils {

//-------------------------------------------------------------------------------------------------
class path {
public:
  explicit path(
    const ::std::string& file, const ::std::string parent = "", bool isfolder = false);
  bool exists();
  bool isfolder() {
    return isfolder_;
  }

  const ::std::string extension() const;
  ::std::string name() const {
    if (parent_.empty())
      return file_;
    else 
      return parent_ + "/" + file_;
  }

private:
  ::std::string parent_;
  ::std::string file_;
  bool isfolder_;
};

//-------------------------------------------------------------------------------------------------
//folder
class filesystem_entry;
class directory_iterator 
  : public ::std::iterator<::std::input_iterator_tag, void, void, void, void> {
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
  : public ::std::iterator<::std::input_iterator_tag, void, void, void, void> {
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
  ::std::list<::std::pair<::std::string, ::std::shared_ptr<filesystem_entry> > > entrys_;
};

} //! namespace pa2davatar
} //! namespace utils

#endif //! UTILS_FILESYSTEM_H