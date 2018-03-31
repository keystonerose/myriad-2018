#include "ImageScanner.hpp"

#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QThread>

#include <cassert>

using namespace myr;

namespace {

    auto supportedMIMETypes() -> const QList<QByteArray>&;

    /// Determines whether the file at the filesystem location `path` is an image file able to be
    /// processed by the application. May only be called after the `QApplication` instance has been
    /// created.

    auto fileSupported(const QString& path) -> bool {

        const QMimeDatabase mimeDb;
        const auto mimeName = mimeDb.mimeTypeForFile(path).name();

        const auto& supported = supportedMIMETypes();
        return supported.contains(mimeName.toLatin1());
    }

    /// Returns a list of all image MIME types that the application is able to process. May only be
    /// called after the `QApplication` instance has been created.

    auto supportedMIMETypes() -> const QList<QByteArray>& {
        static auto result = QImageReader::supportedMimeTypes();
        return result;
    }
}

ImageScanner::ImageScanner()
  : _countChangedSignal{*this, &ImageScanner::countChanged} {}

void ImageScanner::emitCountChanged(const qtx::SignalFilterType type) const {
    _countChangedSignal.tryEmit(type, _images.size(), _folderCount);
}

void ImageScanner::recursiveScan(const QString& rootPath) {

    const auto rootInfo = QFileInfo{rootPath};
    if (!rootInfo.exists()) {
        return;
    }

    if (rootInfo.isFile()) {

        if (fileSupported(rootPath)) {
            _images.emplace_back(rootPath);
            emitCountChanged();
        }
    }
    else if (rootInfo.isDir()) {

        auto rootDir = QDir{rootPath};
        rootDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        const auto items = rootDir.entryInfoList();
        ++_folderCount;
        emitCountChanged();

        const auto thread = this->thread();
        assert(thread);

        const auto end = items.end();
        for (auto iter = items.begin(); iter != end && !thread->isInterruptionRequested(); ++iter) {
            recursiveScan(iter->absoluteFilePath());
        }
    }
}

void ImageScanner::scan(const QString& rootPath) {

    emitCountChanged(qtx::SignalFilterType::Flush);
    recursiveScan(rootPath);

    emitCountChanged(qtx::SignalFilterType::Flush);
    Q_EMIT scanFinished(_images);
}
