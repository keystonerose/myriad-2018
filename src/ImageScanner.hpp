#ifndef MYRIAD_IMAGE_SCANNER_HPP
#define MYRIAD_IMAGE_SCANNER_HPP

#include "qtx/SignalFilter.hpp"

#include <QImage>
#include <QObject>
#include <QString>

#include <vector>

namespace myr {

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

        explicit ImageScanner();
        void scan(const QString& rootPath);

    signals:

        void countChanged(int files, int folders);
        void scanFinished(const std::vector<QImage>& images);

    private:

        void emitCountChanged(qtx::SignalFilterType type = qtx::SignalFilterType::Timed) const;
        void recursiveScan(const QString& rootPath);

        std::vector<QImage> _images;
        int _folderCount = 0;

        qtx::SignalFilter<ImageScanner, int, int> _countChangedSignal;
    };
}

#endif
