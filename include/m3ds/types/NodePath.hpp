#pragma once

#include <string_view>

#include "Error.hpp"
#include "m3ds/utils/BinaryFile.hpp"

namespace M3DS {
    class NodePath {
        std::string mPath {};
    public:
        constexpr NodePath() noexcept = default;
        explicit constexpr NodePath(std::string path) noexcept;

        class NodePathIterator {
            friend class NodePath;
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = std::string_view;
            using iterator_category = std::bidirectional_iterator_tag;
        private:
            std::string_view mData {};

            const char* mStart {};
            const char* mEnd {};

            constexpr void findEnd() noexcept;
            constexpr void findEndReverse() noexcept;
        public:
            [[nodiscard]] constexpr std::string_view operator*() const noexcept;

            [[nodiscard]] constexpr bool operator==(const NodePathIterator& other) const noexcept;

            constexpr NodePathIterator& operator++() noexcept;
            constexpr NodePathIterator operator++(int) noexcept;

            constexpr NodePathIterator& operator--() noexcept;
            constexpr NodePathIterator operator--(int) noexcept;
        };

        [[nodiscard]] constexpr NodePathIterator begin() const noexcept;
        [[nodiscard]] constexpr NodePathIterator end() const noexcept;

        [[nodiscard]] constexpr std::string_view toString() const noexcept;

        [[nodiscard]] explicit constexpr operator std::string_view() const noexcept;

        [[nodiscard]] constexpr bool empty() const noexcept;

        [[nodiscard]] constexpr NodePath operator/(const std::string_view& path) const noexcept;
        constexpr NodePath operator/=(const std::string_view& path) noexcept;

        constexpr auto operator==(const NodePath& other) const noexcept;
        constexpr auto operator<=>(const NodePath& other) const noexcept;

        constexpr void reserve(std::size_t characters);

        [[nodiscard]] Failure serialise(BinaryOutFileAccessor file) const noexcept {
            if (mPath.size() > std::numeric_limits<size_t>::max())
                return Failure{ ErrorCode::file_invalid_data };

            if (
                !file.write(static_cast<std::uint16_t>(mPath.size())) ||
                !file.write(std::span{mPath})
            )
                return Failure{ ErrorCode::file_write_fail };

            return Success;
        }

        [[nodiscard]] Failure deserialise(BinaryInFileAccessor file) noexcept {
            std::uint16_t length;
            if (!file.read(length))
                return Failure{ ErrorCode::file_read_fail };

            mPath.resize(length);
            if (!file.read(std::span{mPath}))
                return Failure{ ErrorCode::file_read_fail };

            return Success;
        }
    };
}

namespace M3DS {
    constexpr NodePath::NodePath(std::string path) noexcept : mPath(std::move(path)) {}

    constexpr void NodePath::NodePathIterator::findEnd() noexcept {
        // std::string_view iterators aren't guaranteed to be const char*
        while (mEnd < mData.data() + mData.size() && *mEnd != '/')
            ++mEnd;
    }

    constexpr void NodePath::NodePathIterator::findEndReverse() noexcept {
        while (mStart > mData.data() && *(mStart - 1) != '/')
            --mStart;
    }

    constexpr std::string_view NodePath::NodePathIterator::NodePathIterator::operator*() const noexcept {
        return { mStart, mEnd };
    }

    constexpr bool NodePath::NodePathIterator::operator==(const NodePathIterator& other) const noexcept {
        return mStart == other.mStart;
    }

    constexpr NodePath::NodePathIterator& NodePath::NodePathIterator::operator++() noexcept {
        if (mEnd == mData.data() + mData.size()) {
            mStart = mEnd;
        } else {
            mStart = ++mEnd;
            findEnd();
        }
        return *this;
    }

    constexpr NodePath::NodePathIterator NodePath::NodePathIterator::operator++(int) noexcept {
        const auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr NodePath::NodePathIterator& NodePath::NodePathIterator::operator--() noexcept {
        if (mStart == mData.data()) {
            mEnd = mStart;
        } else {
            mEnd = --mStart;
            findEndReverse();
        }
        return *this;
    }

    constexpr NodePath::NodePathIterator NodePath::NodePathIterator::operator--(int) noexcept {
        const auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr NodePath::NodePathIterator NodePath::begin() const noexcept {
        NodePathIterator tmp {};
        tmp.mData = mPath;
        tmp.mStart = tmp.mEnd = &mPath[0];
        tmp.findEnd();
        return tmp;
    }

    constexpr NodePath::NodePathIterator NodePath::end() const noexcept {
        NodePathIterator tmp {};
        tmp.mData = mPath;
        tmp.mStart = tmp.mEnd = &mPath[mPath.size()];
        return tmp;
    }

    constexpr std::string_view NodePath::toString() const noexcept {
        return mPath;
    }

    constexpr NodePath::operator std::string_view() const noexcept {
        return mPath;
    }

    constexpr bool NodePath::empty() const noexcept {
        return mPath.empty();
    }

    constexpr NodePath NodePath::operator/(const std::string_view& path) const noexcept {
        NodePath tmp {*this};
        tmp /= path;
        return tmp;
    }

    constexpr NodePath NodePath::operator/=(const std::string_view& path) noexcept {
        mPath += '/';
        mPath += path;
        return *this;
    }

    constexpr auto NodePath::operator==(const NodePath& other) const noexcept {
        return mPath == other.mPath;
    }

    constexpr auto NodePath::operator<=>(const NodePath& other) const noexcept {
        return mPath <=> other.mPath;
    }

    constexpr void NodePath::reserve(const std::size_t characters) {
        mPath.reserve(characters);
    }
}

template <>
struct std::hash<M3DS::NodePath> {
    size_t operator()(const M3DS::NodePath& path) const noexcept {
        return std::hash<std::string_view>{}(std::string_view{path});
    }
};

template <> struct std::formatter<M3DS::NodePath> : std::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const M3DS::NodePath& path, FormatContext& ctx) const {
        return std::formatter<std::string_view>::format(std::string_view{path}, ctx);
    }
};

static_assert(std::formattable<M3DS::NodePath, char>);

constexpr std::ostream& operator<<(std::ostream& stream, const M3DS::NodePath& path) {
    return stream << static_cast<std::string_view>(path);
}
