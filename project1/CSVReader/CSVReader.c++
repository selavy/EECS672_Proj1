#include "CSVReader.h"
#include <cstdlib>

//#define __DEBUG__

CSVReader::CSVReader(
		     const char * filename //!< name of csv file to read
		     )
  :
  _read(false)
{
  _file.open( filename, std::ifstream::in );

  if( !_file.is_open() )
    throw std::invalid_argument("Unable to open file!");

  try
    {
      _nseries = 0;
      _points = 0;
      readFile();
      _read = true;
    }
  catch( std::exception& e )
    {
      std::cout << e.what() << std::endl;
    }
  catch( ... )
    {
      throw std::invalid_argument("File not formatted correctly");
    }

  _file.close();
} /* end CSVReader::CSVReader() */

unsigned int CSVReader::getNumberOfSeries()
{
  if(! _read )
    throw std::invalid_argument("File could not be read correctly");

  return _nseries;
} /* end CSVReader::getNumberOfSeries() */

std::string CSVReader::getNameOfSeries(
				       unsigned int id //!< id of desired series 0 <= id < _nseries
				       )
{
  if(! _read )
    throw std::invalid_argument("File not formatted correctly");

  if( id >= _nseries )
    throw std::out_of_range("Requested series not available");

  return _names.at( id );
} /* end CSVReader::getNameOfSeries() */

std::vector<double> CSVReader::getData(
				       unsigned int id //!< id of desired series 0 <= id < _nseries
				       )
{
  if(! _read )
    throw std::invalid_argument("File not formatted correctly");

  if( id >= _nseries )
    throw std::out_of_range("Requested series data not available");

  return _data.at( id );
} /* end CSVReader::getData() */

std::vector<std::string> CSVReader::getDates()
{
  if(! _read )
    throw std::invalid_argument("File not formatted correctly");

  return _dates;
} /* end CSVReader::getDates() */

/**
 * CSVReader::readFile()
 *
 * Assumes that _file is handle to open csv file
 */
void CSVReader::readFile()
{
  std::string line;
  /* Read over the definitions */
  while( _file.good() )
    {
      std::getline( _file, line );

#ifdef __DEBUG__
      std::cout << "skipping line: " << line << std::endl;
#endif
      if( line == "" )
	break;
    }

#ifdef __DEBUG__
  std::cout << "Skipped over first section" << std::endl;
#endif

  if(! _file.good() )
    throw std::invalid_argument("File format not correct");

  std::getline( _file, line ); // should be line with categories

  std::size_t found = line.find(",");
  std::size_t first = 0;

  /* Categories are delimited by "," */
  while( found != std::string::npos )
    {
      std::string catg = line.substr(first, (found - first));
      _names.push_back( catg );

#ifdef __DEBUG__
      std::cout << "found category: " << catg << std::endl;
#endif

      first = found + 1;
      /* found + 2 because found + 1 will include the "," */
      found = line.find(",", first+1);
    }

  /* get the last category */
  std::string catg = line.substr(first);
  _names.push_back( catg );

  _nseries = _names.size();

  if( _nseries == 0 )
    throw std::invalid_argument("File format not correct!"); 

#ifdef __DEBUG__
  std::cout << "found category: " << catg << std::endl;
#endif

#ifdef __DEBUG__
  std::cout << "Finished getting categories" << std::endl;
#endif

  _data.resize(_nseries - 1);

  /* start getting the data */
  /* similar process as above */
  while( _file.good() )
    {
      std::getline( _file, line );

      if( line == "" )
	break;
      /* line is comma delimited, should be _nseries - 1 commas */

      found = line.find(",");
      first = 0;
      unsigned int i;

#ifdef __DEBUG__
      std::cout << std::endl;
#endif

      for( i = 0; i < (_nseries - 1); ++i )
	{
	  if( found == std::string::npos )
	    throw std::invalid_argument("Could not read data. File not formatted correctly");

	  std::string data = line.substr(first, (found - first));

	  // assert( found >= first );
	  if( i == 0 )
	    {
	      _dates.push_back( data );
	    }
	  else
	    {
	      double ddata = std::strtod( data.c_str(), NULL );
	      _data.at(i - 1).push_back( ddata );
	    }

#ifdef __DEBUG__
	      std::cout << "found: " << data << std::endl;
#endif

	  first = found + 2;
	  found = line.find(",", first+1);
	}

      /* get the last data category */
      std::string data = line.substr( first );
      double ddata = std::strtod( data.c_str(), NULL );
      _data.at( i - 1 ).push_back( ddata );

#ifdef __DEBUG__
      std::cout << "found: " << data << std::endl;
#endif      
    }

  _points = _data.size();


#ifdef __DEBUG__
  std::cout << "FOUND:" << std::endl;
  std::cout << "------------------------------------" << std::endl;

  std::cout << std::endl << "Categories: " << std::endl;
  for( std::vector<std::string>::const_iterator it = _names.begin(); it != _names.end(); ++it )
    {
      std::cout << *it << std::endl;
    }

  std::cout << std::endl << "Dates: " << std::endl;
  for( std::vector<std::string>::const_iterator it = _dates.begin(); it != _dates.end(); ++it )
    {
      std::cout << *it << std::endl;
    }

  std::cout << std::endl << "Data: " << std::endl;
  for( unsigned int i = 0; i < ( _nseries - 1 ); ++i )
    {
      std::cout << std::endl << _names.at( i ) << ": " << std::endl;

      for( std::vector<double>::const_iterator it = _data.at( i ).begin(); it != _data.at( i ).end(); ++it )
	{
	  std::cout << *it << std::endl;
	}
    }
#endif

} /* end CSVReader::getDates() */



