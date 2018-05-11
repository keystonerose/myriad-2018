#include "ImageHasher.hpp"
#include "ImageScanner.hpp"

#include "qtx/WorkerThread.hpp"

#include <QEventLoop>
#include <QGuiApplication>
#include <QImage>
#include <QObject>

#include <cstdlib>
#include <iostream>
#include <string_view>

using namespace myr;

namespace {

    void printHashCount(const std::string_view id, int count) {
        std::cout << "ImageHasher::countChanged(" << count << ") [id: " << id << "]\n";
        std::cout << std::flush;
    }

    void printHashFinished(const std::string_view id) {
        std::cout << "ImageHasher::hashFinished() [id: " << id << "]\n";
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

    auto& scanner           = thread.emplaceWorker<ImageScanner>();
    auto& collectionHasher  = thread.emplaceWorker<ImageHasher>();
    auto& inputHasher       = thread.emplaceWorker<ImageHasher>();

    thread.start();

    QObject::connect(&scanner, &ImageScanner::countChanged, printScanCount);
    QObject::connect(&scanner, &ImageScanner::finished,     printScanFinished);

    QObject::connect(&collectionHasher, &ImageHasher::countChanged,  [] { printHashCount("collection"); });
    QObject::connect(&collectionHasher, &ImageHasher::finished,      [] { printHashFinished("collection"); });

    QObject::connect(&inputHasher, &ImageHasher::countChanged,  [] { printHashCount("input"); });
    QObject::connect(&inputHasher, &ImageHasher::finished,      [] { printHashFinished("input"); });

    auto loop = QEventLoop{};

    QObject::connect(&scanner, &ImageScanner::finished, &collectionHasher, &ImageHasher::exec);
    QObject::connect(&scanner, &ImageScanner::finished, &inputHasher,      &ImageHasher::exec);

    QObject::connect(&collectionHasher, &ImageHasher::finished, &loop, &QEventLoop::quit);

    QMetaObject::invokeMethod(&scanner, [&scanner] { scanner.exec("test/collection"); });
    loop.exec();

    return EXIT_SUCCESS;
}
