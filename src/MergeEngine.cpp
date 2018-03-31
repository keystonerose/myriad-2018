#include "MergeEngine.hpp"

#include "ImageScanner.hpp"

#include <QMetaObject>

#include <memory>

using namespace myr;

MergeEngine::MergeEngine(const QString& collectionRootPath) {

    auto scanner = std::make_unique<ImageScanner>();

    scanner->moveToThread(&_workerThread);
    QObject::connect(&_workerThread, &QThread::finished, scanner.get(), &QObject::deleteLater);

    _workerThread.start();
    if (!_workerThread.isRunning()) {
        return;
    }

    // If the worker thread has been started successfully, then the worker objects associated
    // with it will be deleted automatically when that thread emits the `QThread::finished()`
    // signal, so we release ownership of them.

    const auto rawScanner = scanner.release();

    QObject::connect(
        rawScanner, &ImageScanner::countChanged,
        this, &MergeEngine::collectionCountChanged);

    QMetaObject::invokeMethod(rawScanner, [=] { rawScanner->scan(collectionRootPath); });
}

MergeEngine::~MergeEngine() {
    _workerThread.quit();
    _workerThread.wait();
}

void MergeEngine::addInput(const QImage&) {
}

void MergeEngine::run() {
}
