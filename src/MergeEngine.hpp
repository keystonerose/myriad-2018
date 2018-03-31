#ifndef MYRIAD_MERGE_ENGINE_HPP
#define MYRIAD_MERGE_ENGINE_HPP

#include <QDir>
#include <QImage>
#include <QObject>
#include <QString>

#include <vector>

namespace myr {

    /// Controller class for performing a merge operation on a list of new image files into a
    /// collection of existing ones by identifying those new images that are visual duplicates of
    /// ones already in the collection and appraising whether to overwrite the existing version of
    /// the image with the new one.

    class MergeEngine : public QObject {
        Q_OBJECT

    public:

        // TODO:DOC
        /// The collection is identified as the set of all image files that are filesystem descendants
        /// of a specified collection directory.

        explicit MergeEngine(const QString& collectionRootPath);

        void addInput(const QImage& image);
        void run();

    Q_SIGNALS:

        void inputCountChanged(int fileCount, int folderCount);

    private:

        void scanForImages(const QString& rootPath);

        auto threadInterrupted() const -> bool;

        QDir _collectionRoot;
        std::vector<QImage> _inputs;
        int _inputFolderCount = 0;
    };
}

#endif

