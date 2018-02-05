#ifndef __FILEERROR_H__
#define __FILEERROR_H__

#include <exception>

class FileError : public std::exception{

  virtual const char* what() const throw(){
    return "The file entered as parameter couldn't be opened.";
  }

};

#endif
