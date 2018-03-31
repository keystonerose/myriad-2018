#ifndef MYRIAD_IMAGE_INFO_HPP
#define MYRIAD_IMAGE_INFO_HPP

#include <QFileInfo>
#include <QMetaType>
#include <QString>

#include <cstdint>
#include <vector>

namespace myr {

    /// Identifies specific image formats associated with special appraisal logic. While the
    /// application supports all formats supported by Qt's `QImage` API, most of these receive no
    /// special treatment and are therefore grouped under the `Other` enumerator.

    enum class ImageFormat { Other, Bmp, Gif, Jpeg, Png };

    /// Reads and stores information used to identify and compare the images processed by the
    /// application. In particular, the `phash()` member function returns a perceptual hash that may
    /// used to determine how similar an image is to another; the other properties are associated
    /// with logic used to determine which images are preferable when they are considered to be
    /// different.
    ///
    /// All of the significant work done by an `ImageInfo` object is performed during a call to
    /// `setFile()` (or upon construction if a path is provided); after that point, attributes may
    /// be queried from the object at negligible performance cost.

    class ImageInfo {
    public:

        friend bool operator==(const ImageInfo& lhs, const ImageInfo& rhs);

        explicit ImageInfo() = default;
        explicit ImageInfo(const QString& path) { setFile(path); }

        auto checksum()  const -> std::uint16_t { return _checksum; }
        auto file_size() const -> std::uint64_t { return _fileInfo.size(); }
        auto format()    const -> ImageFormat   { return _format; }
        auto height()    const -> int           { return _height; }
        auto path()      const -> QString       { return _fileInfo.absoluteFilePath(); }
        auto phash()     const -> std::uint64_t { return _phash; }
        auto width()     const -> int           { return _width; }

        /// Fetches information about the image file at the location `path` and constructs an
        /// `ImageInfo` object to store that information. Since the stored image attributes include
        /// the perceptual hash of the image, this is an expensive operation. Throws
        /// `myr::FileIOError` if image data could not be read from a file at `path`.

        void setFile(const QString& path);

    private:

        int _width  = 0;
        int _height = 0;

        ImageFormat   _format   = ImageFormat::Other;
        std::uint16_t _checksum = 0;
        std::uint64_t _phash    = 0;

        QFileInfo _fileInfo;
    };

    using ImageInfoSeq = std::vector<ImageInfo>;

    /// Tests whether two `ImageInfo` objects describe files at the same location on disk. Note that
    /// this is a stronger criterion than `lhs` and `rhs` being bytewise equivalent. (The weaker
    /// condition may be assessed by comparison of their checksums.)

    inline bool operator==(const ImageInfo& lhs, const ImageInfo& rhs) { return lhs._fileInfo == rhs._fileInfo; }
    inline bool operator!=(const ImageInfo& lhs, const ImageInfo& rhs) { return !(lhs == rhs); }
}

Q_DECLARE_METATYPE(myr::ImageInfo)
Q_DECLARE_METATYPE(myr::ImageInfoSeq)

#endif
