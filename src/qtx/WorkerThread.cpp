#include "WorkerThread.hpp"

using namespace qtx;

WorkerThread::WorkerThread(QObject* const parent)
  : _thread{parent} {
    QObject::connect(&_thread, &QThread::finished, this, &WorkerThread::finished);
    QObject::connect(&_thread, &QThread::started,  this, &WorkerThread::started);
}

WorkerThread::~WorkerThread() {
    _thread.quit();
    _thread.wait();
}

auto WorkerThread::start(const QThread::Priority priority) -> bool {

    _thread.start(priority);

    // If the thread has been started successfully, then the worker objects will be deleted
    // automatically when it emits the `QThread::finished()` signal, so we release ownership
    // of them.

    const auto running = _thread.isRunning();
    if (running) {

        for (auto&& worker : _pendingWorkers) {
            worker.release();
        }
        _pendingWorkers.clear();
    }

    return running;
}
