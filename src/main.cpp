#include "ImageHasher.hpp"
#include "ImageScanner.hpp"

#include "qtx/WorkerThread.hpp"

#include <QEventLoop>
#include <QGuiApplication>
#include <QImage>
#include <QObject>

#include <cstdlib>
#include <iostream>

using namespace myr;

namespace {

    void printHashCount(int count) {
        std::cout << "ImageHasher::countChanged(" << count << ")\n";
        std::cout << std::flush;
    }

    void printScanCount(int files, int folders) {
        std::cout << "ImageScanner::countChanged(" << files << ", " << folders << ")\n";
        std::cout << std::flush;
    }

    void printScanFinished() {
        std::cout << "ImageScanner::scanFinished()\n";
        std::cout << std::flush;
    }
}

auto main(int argc, char** argv) -> int {

    const auto app = QGuiApplication{argc, argv};
    auto thread = qtx::WorkerThread{};

    auto& scanner = thread.emplaceWorker<ImageScanner>();
    auto& hasher  = thread.emplaceWorker<ImageHasher>();

    thread.start();

    QObject::connect(&scanner, &ImageScanner::countChanged, printScanCount);
    QObject::connect(&scanner, &ImageScanner::scanFinished, printScanFinished);
    QObject::connect(&hasher,  &ImageHasher::countChanged,  printHashCount);

    QMetaObject::invokeMethod(&scanner, [&scanner] { scanner.scan("test/collection"); });

    // This event loop will eventually be part of a UI thread; for now, we print debug information
    // to the terminal and require Ctrl+C to terminate the application.

    auto loop = QEventLoop{};
    loop.exec();

    return EXIT_SUCCESS;
}
