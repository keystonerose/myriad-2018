#include "MergeEngine.hpp"

#include <QEventLoop>
#include <QGuiApplication>
#include <QImage>
#include <QObject>

#include <cstdlib>
#include <iostream>

using namespace myr;

namespace {

    void printCollectionCount(int files, int folders) {
        std::cout << "MergeEngine::collectionCountChanged(" << files << ", " << folders << ")\n";
        std::cout << std::flush;
    }
}

auto main(int argc, char** argv) -> int {

    const auto app = QGuiApplication{argc, argv};

    auto engine = MergeEngine{"test/collection"};
    QObject::connect(&engine, &MergeEngine::collectionCountChanged, printCollectionCount);

    engine.addInput(QImage{"test/orange-siberian.jpg"});
    engine.addInput(QImage{"test/tunnel-cat.small.jpg"});
    engine.run();

    // This event loop will eventually be part of a UI thread; for now, we print debug information
    // to the terminal and require Ctrl+C to terminate the application.

    auto loop = QEventLoop{};
    loop.exec();

    return EXIT_SUCCESS;
}
