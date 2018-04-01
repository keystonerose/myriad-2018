#ifndef MYRIAD_IMAGE_SCANNER_HPP
#define MYRIAD_IMAGE_SCANNER_HPP

#include "qtx/EmissionFilter.hpp"

#include <QObject>
#include <QString>

#include <vector>

namespace myr {

    // TODO:DOC update
    /// Worker class that recursively scans directories for supported image files. Directories to
    /// scan are specified via (typically cross-thread) invocation of the `scan()` member function;
    /// output is via the `countChanged()` and `scanFinished()` signals.
    ///
    /// As currently implemented, a sequence of calls to `scan()` results in a separate emission of
    /// `scanFinished()` when each one completes (even if a subsequent `scan()` invocation was made
    /// before the completion of the previous one).

    class ImageScanner : public QObject {
        Q_OBJECT

    public:
        void exec(const QString& rootPath) const;

    signals:
        void countChanged(int files, int folders) const;
        void finished(const std::vector<QString>& imagePaths) const;
    };
}

#endif
