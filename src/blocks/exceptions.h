#ifndef BLOCKS_EXCEPTIONS_H
#define BLOCKS_EXCEPTIONS_H

#include <exception>
#include <string>

namespace blocks {

class base_exception : public std::exception {
  public:
    base_exception(const std::string& message) : msg_{message} {}
    ~base_exception() throw() {}
    const char* what() const throw() { return msg_.c_str(); }

  private:
    std::string msg_;
};

class invalid_operation_error : public base_exception {
  public:
    invalid_operation_error(const std::string& message)
        : base_exception{message} {}
};

class illegal_port_error : public base_exception {
  public:
    illegal_port_error(const std::string& message) : base_exception{message} {}
};

} // namespace blocks

#endif // BLOCKS_EXCEPTIONS_H