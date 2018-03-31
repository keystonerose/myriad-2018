#include "ImageHasher.hpp"

#include <QThread>

using namespace myr;

ImageHasher::ImageHasher()
  : _countChangedSignal{*this, &ImageHasher::countChanged} {}

void ImageHasher::emitCountChanged(const qtx::SignalFilterType type) const {
    _countChangedSignal.tryEmit(type, _imageInfo.size());
}

void ImageHasher::hash(const std::vector<QString>& imagePaths) {

    emitCountChanged(qtx::SignalFilterType::Flush);

    const auto thread = this->thread();
    assert(thread);

    const auto end = imagePaths.end();
    for (auto iter = imagePaths.begin(); (iter != end) && !thread->isInterruptionRequested(); ++iter) {
        _imageInfo.emplace_back(*iter);
        emitCountChanged();
    }

    emitCountChanged(qtx::SignalFilterType::Flush);
    Q_EMIT hashFinished(_imageInfo);
}
