#include "ImageInfo.hpp"

#include "Exception.hpp"

#include "qtx/QStringHash.hpp"

#include "pHash.h"

#include <QDir>
#include <QFile>
#include <QImage>
#include <QMimeDatabase>
#include <QTemporaryFile>

#include <unordered_map>

using namespace myr;

namespace {

    struct MetaTypeRegistrar {
        explicit MetaTypeRegistrar() {
            qRegisterMetaType<ImageInfo>();
            qRegisterMetaType<ImageInfoSeq>("myr::ImageInfoSeq");
        }
    };

    MetaTypeRegistrar registrar;

    /// Generates a checksum for the contents of the file located at `path`, which may be zero if
    /// the file was able to be opened but no data were able to read from it. Throws `FileIOError`
    /// if the file could not be opened.

    auto fileChecksum(const QString& path) -> std::uint16_t {

        auto file = QFile{path};
        if (!file.open(QIODevice::ReadOnly)) {
            throw FileIOError{path};
        }

        const auto data = file.readAll();
        return qChecksum(data, data.length());
    }

    /// Returns an `ImageFormat` code identifying the format of the image file at the location
    /// `path`. If the file does not have a recognised image (or non-image) MIME type,
    /// `ImageFormat::Other` is returned (and it is the responsibility of calling code to detect
    /// this error through other means).

    auto imageFileFormat(const QString& path) -> ImageFormat {

        static const auto formatsByMIMEName =  std::unordered_map<QString, ImageFormat>{
            {QStringLiteral("image/bmp"),  ImageFormat::Bmp},
            {QStringLiteral("image/gif"),  ImageFormat::Gif},
            {QStringLiteral("image/jpeg"), ImageFormat::Jpeg},
            {QStringLiteral("image/png"),  ImageFormat::Png}};

        static const auto mimeDb = QMimeDatabase{};
        const auto mimeName = mimeDb.mimeTypeForFile(path).name();

        const auto iter = formatsByMIMEName.find(mimeName);
        return (iter != formatsByMIMEName.end()) ? iter->second : ImageFormat::Other;
    }
}

void ImageInfo::setFile(const QString& path) {

    const auto image = QImage{path};
    if (image.isNull()) {
        throw FileIOError{path};
    }

    _width  = image.width();
    _height = image.height();

    _checksum = fileChecksum(path);
    _format   = imageFileFormat(path);

    _fileInfo.setFile(path);

    // While ulong64 and std::uint64_t are specified to be compatible types, there is potentially a
    // conversion between the two that prevents a reference to result.phash from being passed
    // directly to `ph_dct_imagehash()`.

    auto phash = ulong64{0};

    // The pHash library only supports JPEG and BMP files, so if the image is not already in one of
    // those formats, a temporary bitmap file is created and used to calculate the hash value.

    switch (_format) {
    case ImageFormat::Bmp: [[fallthrough]];
    case ImageFormat::Jpeg:

        ph_dct_imagehash(path.toLatin1(), phash);
        break;

    default:

        auto tempBMP = QTemporaryFile{QDir::tempPath() + "/myriad_XXXXXX.bmp"};
        const auto tempPath = QFileInfo{tempBMP}.filePath();

        if (!tempBMP.open()) {
            throw FileIOError{tempPath};
        }

        image.save(&tempBMP, "BMP");
        tempBMP.close();
        ph_dct_imagehash(tempPath.toLatin1(), phash);
    }

    _phash = phash;
}
