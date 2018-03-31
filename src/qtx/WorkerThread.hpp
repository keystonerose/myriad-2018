#ifndef QTX_WORKER_THREAD_HPP
#define QTX_WORKER_THREAD_HPP

#include <QThread>

#include <memory>
#include <utility>
#include <vector>

namespace qtx {

    /// Wrapper class for `QThread` that manages the ownership and thread association of a number of
    /// worker objects that will execute within the thread (typically communicating via signal-slot
    /// connections set up by calling code). Exposes a subset of the functionality of `QThread`,
    /// which can be expanded as necessary, and automatically quits the thread upon destruction.

    class WorkerThread : public QObject {
        Q_OBJECT

    public:

        explicit WorkerThread(QObject* parent = nullptr);

        /// Destroys the `WorkerThread` object, automatically quitting the `QThread` that it
        /// manages. `QThread::quit()` is not exposed by `WorkerThread` directly; once started, the
        /// `QThread` object continues running until this destructor is called.

        ~WorkerThread();

        /// Constructs a worker object of type `T` to execute within the thread, with the
        /// constructor arguments `args` (as if by `T(args...)`). `T` must be a subclass of
        /// `QObject`.
        ///
        /// A reference to the worker object is returned: that object is owned by the
        /// `WorkerThread`, and will be deleted when the `WorkerThread` object is destroyed (which
        /// terminates the thread if it is running). The returned reference is valid until such a
        /// time.

        template <typename T, typename... Args>
        auto emplaceWorker(Args&&... args) -> T&;

        /// Launches the thread, as if by `QThread::start(priority)`, and ensures that
        /// responsibility for the deletion of any workers created prior to the call is updated
        /// accordingly in an exception-safe manner.

        auto start(QThread::Priority priority = QThread::InheritPriority) -> bool;

    Q_SIGNALS:

        void finished();
        void started();

    private:

        QThread _thread;
        std::vector<std::unique_ptr<QObject>> _pendingWorkers;
    };

    template <typename T, typename... Args>
    auto WorkerThread::emplaceWorker(Args&&... args) -> T& {

        auto worker = std::make_unique<T>(std::forward<Args>(args)...);
        worker->moveToThread(&_thread);
        QObject::connect(&_thread, &QThread::finished, worker.get(), &QObject::deleteLater);

        if (_thread.isRunning()) {
            return *worker.release();
        }
        else {
            const auto rawWorker = worker.get();
            _pendingWorkers.push_back(std::move(worker));
            return *rawWorker;
        }
    }
}

#endif

