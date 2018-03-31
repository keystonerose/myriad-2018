#ifndef MYRIAD_MERGE_ENGINE_HPP
#define MYRIAD_MERGE_ENGINE_HPP

#include <QImage>
#include <QObject>
#include <QString>
#include <QThread>

#include <vector>

namespace myr {

    /// Controller class for performing a merge operation on a list of new image files into a
    /// collection of existing ones by identifying those new images that are visual duplicates of
    /// ones already in the collection and appraising whether to overwrite the existing version of
    /// the image with the new one.

    class MergeEngine : public QObject {
        Q_OBJECT

    public:

        /// Constructs an engine object, launches a worker thread and, in that worker thread, starts
        /// recursively scanning the directory located at `collectionRootPath` for a collection
        /// supported image files. The progress of this scan may be monitored via the
        /// `collectionCountChanged()` signal.

        explicit MergeEngine(const QString& collectionRootPath);

        ~MergeEngine();

        void addInput(const QImage& image);
        void run();

    Q_SIGNALS:

        void collectionCountChanged(int files, int folders);

    private:

        QThread _workerThread;
    };
}

#endif

