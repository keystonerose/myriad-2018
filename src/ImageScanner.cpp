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

    template<typename F>
    void recursiveScan(
        const QString& rootPath, std::vector<QString>& imagePaths, int& folderCount, F update) {

        const auto rootInfo = QFileInfo{rootPath};
        if (!rootInfo.exists()) {
            return;
        }

        if (rootInfo.isFile()) {

            if (fileSupported(rootPath)) {
                imagePaths.push_back(rootPath);
                update();
            }
        }
        else if (rootInfo.isDir()) {

            auto rootDir = QDir{rootPath};
            rootDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

            const auto items = rootDir.entryInfoList();
            ++folderCount;
            update();

            const auto thread = QThread::currentThread();
            assert(thread);

            const auto end = items.end();
            for (auto iter = items.begin(); (iter != end) && !thread->isInterruptionRequested(); ++iter) {
                recursiveScan(iter->absoluteFilePath(), imagePaths, folderCount, update);
            }
        }
    }

    /// Returns a list of all image MIME types that the application is able to process. May only be
    /// called after the `QApplication` instance has been created.

    auto supportedMIMETypes() -> const QList<QByteArray>& {
        static auto result = QImageReader::supportedMimeTypes();
        return result;
    }
}

void ImageScanner::exec(const QString& rootPath) const {

    auto imagePaths  = std::vector<QString>{};
    auto folderCount = 0;

    const auto countFilter = qtx::EmissionFilter{*this, &ImageScanner::countChanged};
    countFilter.tryEmit(qtx::EmissionType::Flush, imagePaths.size(), folderCount);

    recursiveScan(
        rootPath, imagePaths, folderCount,
        [&] { countFilter.tryEmit(qtx::EmissionType::Timed, imagePaths.size(), folderCount); });

    countFilter.tryEmit(qtx::EmissionType::Flush, imagePaths.size(), folderCount);
    Q_EMIT finished(imagePaths);
}
