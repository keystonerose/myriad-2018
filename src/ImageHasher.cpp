#include "ImageHasher.hpp"

#include "qtx/EmissionFilter.hpp"

#include <QThread>

using namespace myr;

void ImageHasher::exec(const std::vector<QString>& imagePaths) const {

    auto info = ImageInfoSeq{};
    const auto countFilter = qtx::EmissionFilter{*this, &ImageHasher::countChanged};
    countFilter.tryEmit(qtx::EmissionType::Flush, info.size());

    const auto thread = QThread::currentThread();
    assert(thread);

    const auto end = imagePaths.end();
    for (auto iter = imagePaths.begin(); (iter != end) && !thread->isInterruptionRequested(); ++iter) {
        info.emplace_back(*iter);
        countFilter.tryEmit(qtx::EmissionType::Timed, info.size());
    }

    countFilter.tryEmit(qtx::EmissionType::Flush, info.size());
    Q_EMIT finished(info);
}
