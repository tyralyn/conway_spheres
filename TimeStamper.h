
#ifndef TIMESTAMPER_H
#define TIMESTAMPER_H

#include <string>
#include <sstream>

class TimeStamper{
public:
    const std::string timestampedFilename(std::string const & prefix, std::string const & suffix){
        mStringStream << prefix << "_" << time(NULL) << suffix;
        return mStringStream.str();
    }
private:
    std::stringstream mStringStream;
};

#endif