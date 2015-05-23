#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class Exceptions : public std::exception
{
public:
    Exceptions(const char* aInfos);
    virtual const char* what() const throw() {return _Infos;}
private:
    Exceptions();
    const char* _Infos;
};

#endif // EXCEPTIONS_H
