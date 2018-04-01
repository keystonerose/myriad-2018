#ifndef MYRIAD_IMAGE_HASHER_HPP
#define MYRIAD_IMAGE_HASHER_HPP

#include "ImageInfo.hpp"

#include <QObject>
#include <QString>

#include <vector>

namespace myr {

    class ImageHasher : public QObject {
        Q_OBJECT

    public:
        void exec(const std::vector<QString>& imagePaths) const;

    signals:
        void countChanged(int value) const;
        void finished(const myr::ImageInfoSeq& imageInfo) const;
    };
}

#endif
