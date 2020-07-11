#include <stdexcept>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <kvan/topdir.h>

// see also https://unix.stackexchange.com/a/317215/37162
//
static fs::path get_self_path()
{
  fs::path self_exe = "/proc/self/exe";
  return fs::read_symlink(self_exe);
}

TopDirImpl::TopDirImpl(const char* marker_file)
{
  fs::path argv0 = get_self_path();
  if (!argv0.has_filename()) {
    throw runtime_error(__func__);
  }
  
  fs::path c_dir = argv0.remove_filename();
  bool marker_file_found = false;
  do {
    fs::path marker_pn = c_dir / marker_file;
    if (fs::exists(marker_pn)) {
      topdir = marker_pn.remove_filename();
      marker_file_found = true;
      break;
    }
    c_dir = fs::canonical(c_dir / "..");
  } while (c_dir != fs::path("/") && fs::exists(c_dir));

  if (!marker_file_found) {
    throw runtime_error("TopDir::init: marker file not found");
  }    
}
  
