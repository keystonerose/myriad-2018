#ifndef MYRIAD_EXCEPTION_HPP
#define MYRIAD_EXCEPTION_HPP

#include <QString>

#include <stdexcept>
#include <string>

namespace myr {

    /// Thrown when an error occurs reading data from or writing data to a file on disk. The
    /// `what()` member function returns the filesystem path to the file that could not be read from
    /// or written to.

    class FileIOError : public std::runtime_error {
    public:

        explicit FileIOError(const std::string& path) : runtime_error{path} {}
        explicit FileIOError(const QString& path)     : runtime_error{path.toLatin1()} {}
    };
}

#endif
