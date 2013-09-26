#ifndef __CSV_READER_H__
#define __CSV_READER_H__

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

class CSVReader
{
 public:
  CSVReader( const char * filename );
  unsigned int getNumberOfSeries();
  std::string getNameOfSeries( unsigned int id );
  std::vector<double> getData( unsigned int id );
  std::vector<std::string> getDates();
  unsigned int getNumberOfPoints();

 private:
  void readFile();

  bool _read;
  std::ifstream _file;
  unsigned int _nseries;
  unsigned int _points;
  std::vector<std::vector<double> > _data;
  std::vector<std::string> _names;
  std::vector<std::string> _dates;
};

#endif
