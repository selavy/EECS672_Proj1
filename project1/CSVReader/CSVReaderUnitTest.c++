#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "CSVReader.h"

int main( int argc, char ** argv )
{
  char * filename;

  if( argc == 2 )
    filename = argv[1];
  else
    {
      int filename_sz = strlen("ExchangeRates.csv") + 1;
      filename = (char*) malloc( filename_sz );
      strcpy( filename, "ExchangeRates.csv" );
    }

  CSVReader * reader;

  try
    {
      reader = new CSVReader(filename); // test constructor & CSVReader::readFile()
      std::vector<std::string> dates = reader->getDates(); // test CSVReader::getDates()

      unsigned int n = reader->getNumberOfSeries(); // test CSVReader::getNumberOfSeries()
      std::string datename = reader->getNameOfSeries( 0 ); // test CSVReader::getNameOfSeries()

      std::cout << std::endl;
      n--;
      for( unsigned int i = 0; i < n; ++i )
	{
	  std::string name = reader->getNameOfSeries( i + 1 );
	  std::vector<double> series = reader->getData( i ); // test CSVReader::getData()

	  std::cout << name << ": " << std::endl;
	  std::vector<std::string>::const_iterator itt = dates.begin();
	  for( std::vector<double>::const_iterator it = series.begin(); it != series.end() && itt != dates.end(); ++it )
	    {
	      std::cout << *itt << " " << *it << std::endl;
	      ++itt;
	    }
	}

    }
  catch( const std::exception& e )
    {
      std::cout << e.what() << std::endl;
    }
  catch( ... )
    {
      std::cout << "Error" << std::endl;
    }

  return 0;
}
