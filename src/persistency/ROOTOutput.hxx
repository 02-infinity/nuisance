#ifndef PERSITENCY_ROOTOUTPUT_HXX_SEEN
#define PERSITENCY_ROOTOUTPUT_HXX_SEEN

#include "exception/exception.hxx"

#include "TFile.h"

#include <iostream>
#include <memory>
#include <string>

namespace nuis {
namespace persistency {

NEW_NUIS_EXCEPT(WriteToOutputFile_nullptr);

/// Will get/open a TFile that is described in the global config
///
/// The named streams will be used to configure the file name and open mode from
/// the global config element persistency.<name>: {file: output.root opts:
/// CREATE}
std::shared_ptr<TFile> GetOutputFile(std::string const &name = "default");

template <typename T>
inline void WriteToOutputFile(T *object, std::string const &object_name,
                       std::string dir_name = "",
                       std::string const &file_name = "default") {

if(!object){
  throw WriteToOutputFile_nullptr();
}

  TDirectory *ogdir = gDirectory;

  std::shared_ptr<TFile> f = GetOutputFile(file_name);

  TDirectory *d = f.get();
  while (dir_name.length()) {
    size_t next_slash = dir_name.find_first_of('/');
    std::string next_dir = dir_name.substr(0, next_slash);

    if (next_slash != std::string::npos) {
      dir_name = dir_name.substr(next_slash + 1);
    } else {
      dir_name = "";
    }

    TDirectory *nd = d->GetDirectory(next_dir.c_str());
    if (!nd) {
      nd = d->mkdir(next_dir.c_str());
    }
    nd->cd();
    d = nd;
  }

  d->cd();
  object->Write(object_name.c_str(), TObject::kOverwrite);

  if (ogdir) {
    ogdir->cd();
  }
}
} // namespace persistency
} // namespace nuis

#endif
