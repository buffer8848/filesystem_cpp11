// Copyright (c) 2015 Intel Corporation.
// Author: Li, Binfei (binfeix.li@intel.com)

#include <algorithm>

#include "gtest/gtest.h"

#include "common.h"
#include "utils/filesystem.h"

using namespace std;
using namespace pa2davatar::utils;

//-------------------------------------------------------------------------------------------------
TEST(filesystem, work) {
  for_each(directory_iterator(STRING_DATA_SKETCH), directory_iterator(), 
    [&](path file) {
    cout << "Found files: " << file.name() << endl;
  });

  for_each(recursive_directory_iterator(STRING_DATA_SKETCH), recursive_directory_iterator(), 
    [&](path file) {
    cout << "Found files(recursive): " << file.name() << endl;
  });
}