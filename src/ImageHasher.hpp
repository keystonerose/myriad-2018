#ifndef MYRIAD_IMAGE_HASHER_HPP
#define MYRIAD_IMAGE_HASHER_HPP

#include "ImageInfo.hpp"

#include "qtx/SignalFilter.hpp"

#include <QObject>
#include <QString>

#include <vector>

namespace myr {

    class ImageHasher : public QObject {
        Q_OBJECT

    public:

        explicit ImageHasher();
        void hash(const std::vector<QString>& imagePaths);

    signals:

        void countChanged(int value);
        void hashFinished(const std::vector<ImageInfo>& imageInfo);

    private:

        void emitCountChanged(qtx::SignalFilterType type = qtx::SignalFilterType::Timed) const;

        std::vector<ImageInfo> _imageInfo;
        int _folderCount = 0;

        qtx::SignalFilter<ImageHasher, int> _countChangedSignal;
    };
}

#endif
