// from https://stackoverflow.com/a/11533116/1181482
#include <iostream>
#include <sstream>
#include <kvan/csv-reader.h>

int main() {
  const char input[] =
"Year,Make,Model,Description,Price\n"
"1997,Ford,E350,\"ac, abs, moon\",3000.00\n"
"1999,Chevy,\"Venture \"\"Extended Edition\"\"\",\"\",4900.00\n"
"1999,Chevy,\"Venture \"\"Extended Edition, Very Large\"\"\",\"\",5000.00\n"
"1996,Jeep,Grand Cherokee,\"MUST SELL!\n\
air, moon roof, loaded\",4799.00\n";
  
  std::istringstream ss(input);
  std::string title[5];
  int year;
  std::string make, model, desc;
  float price;
  csv_istream(ss)
    >> title[0] >> title[1] >> title[2] >> title[3] >> title[4];
  for (int i = 0; i < 5; i++) { std::cout << title[i] << " -- "; }
  std::cout << std::endl;
  while (csv_istream(ss) >> year >> make >> model >> desc >> price) {
    //...do something with the record...
    std::cout << year << " + " << make << " + " << desc << " + " << price;
    std::cout << std::endl;
  }
}
