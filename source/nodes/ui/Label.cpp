#include <m3ds/nodes/ui/Label.hpp>

namespace M3DS {
    std::string_view Label::getText() const noexcept {
        return mText;
    }

    const std::shared_ptr<Font> & Label::getFont() const noexcept {
        return mFont;
    }

    bool Label::getCapitalise() const noexcept {
        return mCapitalise;
    }

    Label::Justify Label::getJustify() const noexcept {
        return mJustify;
    }

    void Label::setText(const std::string_view text) noexcept {
        mText = text;
        queueResize();
    }

    void Label::setFont(std::shared_ptr<Font> font) noexcept {
        mFont = std::move(font);
        queueResize();
    }

    void Label::setCapitalise(const bool to) noexcept {
        mCapitalise = to;
        queueResize();
    }

    void Label::setJustify(const Justify to) noexcept {
        mJustify = to;
        queueResize();
    }

    Error Label::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Error error = UINode::serialise(file); error != Error::none)
            return error;

        if (mText.size() > std::numeric_limits<std::uint16_t>::max())
            return Error::file_invalid_data;

        const bool defaultFont = (mFont == Font::getDefaultFont());

        if (
            !file.write(static_cast<std::uint16_t>(mText.size())) ||
            !file.write(std::span{mText}) ||
            !file.write(mCapitalise) ||
            !file.write(mJustify) ||
            !file.write(defaultFont)
        )
            return Error::file_write_fail;

        if (defaultFont)
            return Error::none;
        return M3DS::serialise(mFont.get(), file);
    }

    Error Label::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Error error = UINode::deserialise(file); error != Error::none)
            return error;

        std::uint16_t length;
        if (!file.read(length))
            return Error::file_read_fail;

        mText.resize(length);

        bool defaultFont;
        if (
            !file.read(std::span{mText}) ||
            !file.read(mCapitalise) ||
            !file.read(mJustify) ||
            !file.read(defaultFont)
        )
            return Error::file_read_fail;

        if (defaultFont)
            return Error::none;

        return M3DS::deserialise(mFont, file);
    }

    char Label::getFinalChar(const char c) const noexcept {
        return (mCapitalise && c >= 'a' && c <= 'z') ? c - 32 : c;
    }

    void Label::draw(RenderTarget2D& target) {
        for (const auto& [position, glyph] : mGlyphs)
            target.drawChar(getGlobalTransform().offset(static_cast<Vector2>(position)), *mFont, *glyph, colour);

        UINode::draw(target);
    }

    void Label::updateMinSize() noexcept {
        UINode::updateMinSize();

        if (!mFont)
            return;

        // Size is justification-agnostic
        Vector2i bounding = static_cast<Vector2i>(getMinSize());
        Vector2i cursor {};

        for (const char c : mText) {
            if (c == '\n') {
                cursor.x = 0;
                cursor.y += mFont->getLineHeight() + mFont->getLineSpacing();
            } else if (c == ' ') {
                cursor.x += mFont->getSpaceSize();
            } else {
                const auto& [size, uvs] = mFont->getGlyph(getFinalChar(c));
                cursor.x += size.x + mFont->getLetterSpacing();

                bounding = max(bounding, cursor + Vector2i{0, mFont->getLineHeight()});
            }
        }

        mInternalMinSize = max(mInternalMinSize, static_cast<Vector2>(bounding) - Vector2{static_cast<float>(mFont->getLetterSpacing()), 0});
    }

    void Label::resize() noexcept {
        UINode::resize();

        mGlyphs.clear();
        if (!mFont)
            return;

        mGlyphs.reserve(mText.size());

        Vector2i cursor {};
        const std::uint8_t lineHeight = mFont->getLineHeight();
        const std::uint8_t lineSpacing = mFont->getLineSpacing();
        const std::uint8_t letterSpacing = mFont->getLetterSpacing();
        const std::uint8_t spaceSize = mFont->getSpaceSize();

        if (mJustify == Justify::left) {
            for (const char c : mText) {
                if (c == '\n') {
                    cursor.x = 0;
                    cursor.y += lineHeight + lineSpacing;
                } else if (c == ' ') {
                    cursor.x += spaceSize;
                } else {
                    const Font::Glyph& glyph = mFont->getGlyph(getFinalChar(c));
                    mGlyphs.emplace_back(cursor, &glyph);
                    cursor.x += glyph.size.x + letterSpacing;
                }
            }
        } else if (mJustify == Justify::right || mJustify == Justify::centre) {
            for (std::size_t i {}; i < mText.size(); ++i) {
                const std::size_t lineStart = i;

                int lineWidth {};
                for (; mText[i] != '\n' && i < mText.size(); ++i) {
                    const char c = mText[i];
                    if (c == ' ') {
                        lineWidth += spaceSize;
                    } else {
                        lineWidth += mFont->getGlyph(getFinalChar(c)).size.x + letterSpacing;
                    }
                }

                if (i != lineStart && mText[i - 1] != ' ')
                    lineWidth -= letterSpacing;

                cursor.x = static_cast<int>(getSize().x) - lineWidth;
                if (mJustify == Justify::centre)
                    cursor.x /= 2;

                for (std::size_t j = lineStart; j < i; ++j) {
                    const char c = mText[j];
                    if (c == ' ') {
                        cursor.x += spaceSize;
                    } else {
                        const Font::Glyph& glyph = mFont->getGlyph(getFinalChar(c));

                        mGlyphs.emplace_back(cursor, &glyph);
                        cursor.x += glyph.size.x + letterSpacing;
                    }
                }

                cursor.y += lineHeight + lineSpacing;
            }
        }
    }

    REGISTER_METHODS(
        Label,

        CONST_METHOD(getText),
        CONST_METHOD(getCapitalise),
        MUTABLE_METHOD(setText),
        MUTABLE_METHOD(setCapitalise)
    );

    REGISTER_MEMBERS(
        Label,

        PRIVATE_MEMBER(text, getText, setText),
        PRIVATE_MEMBER(capitalise, getCapitalise, setCapitalise)
    )
}
