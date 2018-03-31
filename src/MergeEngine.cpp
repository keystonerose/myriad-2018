#include "MergeEngine.hpp"

#include "qtx/SignalFilter.hpp"

#include <QByteArray>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QThread>

using namespace myr;
using qtx::SignalFilter;

namespace {

    const QList<QByteArray>& supportedMimeTypes();

    /// Determines whether the file at the filesystem location `path` is an image file able to be
    /// processed by the application. May only be called after the `QApplication` instance has been
    /// created.

    bool fileSupported(const QString& path) {

        const QMimeDatabase mimeDb;
        const auto mimeName = mimeDb.mimeTypeForFile(path).name();

        const auto& supported = supportedMimeTypes();
        return supported.contains(mimeName.toLatin1());
    }

    /// Returns a list of all image MIME types that the application is able to process. May only be
    /// called after the `QApplication` instance has been created.

    const QList<QByteArray>& supportedMimeTypes() {
        static auto result = QImageReader::supportedMimeTypes();
        return result;
    }
}

MergeEngine::MergeEngine(const QString& collectionRootPath)
  : _collectionRoot{collectionRootPath} {}

void MergeEngine::scanForImages(const QString& rootPath) {

    const auto rootInfo = QFileInfo{rootPath};
    if (!rootInfo.exists()) {
        return;
    }

    const auto inputCountSignal = SignalFilter{*this, &MergeEngine::inputCountChanged};

    if (rootInfo.isFile()) {

        if (fileSupported(rootPath)) {
            _inputs.emplace_back(rootPath);
            inputCountSignal.tryEmit(_inputs.size(), _inputFolderCount);
        }
    }
    else if (rootInfo.isDir()) {

        auto rootDir = QDir{rootPath};
        rootDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        const auto items = rootDir.entryInfoList();
        ++_inputFolderCount;
        inputCountSignal.tryEmit(_inputs.size(), _inputFolderCount);

        const auto end = items.end();
        for (auto iter = items.begin(); iter != end && !threadInterrupted(); ++iter) {
            scanForImages(iter->absoluteFilePath());
        }
    }
}

auto MergeEngine::threadInterrupted() const -> bool {
    return QThread::currentThread()->isInterruptionRequested();
}
