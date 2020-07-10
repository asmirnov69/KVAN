#include <stdexcept>
#include <sstream>
#include <iterator>
using namespace std;

#include <kvan/string-utils.h>

class WordDelimitedByComma : public std::string {};
static std::istream& operator>>(std::istream& is, WordDelimitedByComma& output)
{
  std::getline(is, output, ',');
  return is;
}

vector<string> string_split(const string& s, char del)  
{
  if (del != ',') {
    throw runtime_error(__func__);
  }
  
  std::istringstream iss(s);
  vector<string> v((std::istream_iterator<WordDelimitedByComma>(iss)),
		   std::istream_iterator<WordDelimitedByComma>());
  return v;
}
