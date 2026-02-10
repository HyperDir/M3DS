#include <m3ds/nodes/ui/Label.hpp>

namespace M3DS {
    std::string_view Label::getText() const noexcept {
        return mText;
    }

    const std::shared_ptr<Font>& Label::getFont() const noexcept {
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

    Failure Label::serialise(BinaryOutFileAccessor file) const noexcept {
        if (const Failure failure = SuperType::serialise(file))
            return failure;

        if (mText.size() > std::numeric_limits<std::uint16_t>::max())
            return Failure{ ErrorCode::file_invalid_data };

        const bool defaultFont = (mFont == Font::getDefaultFont());

        if (
            !file.write(static_cast<std::uint16_t>(mText.size())) ||
            !file.write(std::span{mText}) ||
            !file.write(mCapitalise) ||
            !file.write(mJustify) ||
            !file.write(defaultFont)
        )
            return Failure{ ErrorCode::file_write_fail };

        if (defaultFont)
            return Success;
        return M3DS::serialise(mFont.get(), file);
    }

    Failure Label::deserialise(BinaryInFileAccessor file) noexcept {
        if (const Failure failure = SuperType::deserialise(file))
            return failure;

        std::uint16_t length;
        if (!file.read(length))
            return Failure{ ErrorCode::file_read_fail };

        mText.resize(length);

        bool defaultFont;
        if (
            !file.read(std::span{mText}) ||
            !file.read(mCapitalise) ||
            !file.read(mJustify) ||
            !file.read(defaultFont)
        )
            return Failure{ ErrorCode::file_read_fail };

        if (defaultFont)
            return Success;

        return M3DS::deserialise(mFont, file);
    }

    char Label::getFinalChar(const char c) const noexcept {
        return (mCapitalise && c >= 'a' && c <= 'z') ? c - 32 : c;
    }

    void Label::draw(RenderTarget2D& target) {
        const Transform2D& transform = getGlobalTransform();

        mMesh.transform = Matrix4x4::fromTRS(
            { transform.position.x, transform.position.y, 0.f },
            Quaternion::fromAxisAngle({0.f, 0.f, 1.f}, transform.rotation),
            { transform.scale.x, transform.scale.y, 1.f }
        );

        mMesh.tint = colour;

        target.draw(mMesh);

        UINode::draw(target);
    }

    void Label::updateMinSize() noexcept {
        UINode::updateMinSize();

        if (!mFont)
            return;

        // Size is justification-agnostic
        Vector2 bounding = getMinSize();
        Vector2 cursor {};

        for (const char c : mText) {
            if (c == '\n') {
                cursor.x = 0;
                cursor.y += static_cast<float>(mFont->getLineHeight() + mFont->getLineSpacing());
            } else if (c == ' ') {
                cursor.x += mFont->getSpaceSize();
            } else {
                const auto& [size, uvs] = mFont->getGlyph(getFinalChar(c));
                cursor.x += static_cast<float>(size.x + mFont->getLetterSpacing());

                bounding = max(bounding, cursor + Vector2{0.f, static_cast<float>(mFont->getLineHeight())});
            }
        }

        mInternalMinSize = max(mInternalMinSize, bounding - Vector2{static_cast<float>(mFont->getLetterSpacing()), 0.f});
    }

    void Label::resize() noexcept {
        UINode::resize();

        mMesh.texture = mFont->getTexture();

        Vector2 cursor {};
        const float lineHeight = static_cast<float>(mFont->getLineHeight());
        const float lineSpacing = static_cast<float>(mFont->getLineSpacing());
        const float letterSpacing = static_cast<float>(mFont->getLetterSpacing());
        const float spaceSize = static_cast<float>(mFont->getSpaceSize());

        mMesh.vertices.resize(mText.size() * 6);
        std::size_t i {};

        std::span vertices = mMesh.vertices;

        const auto generateTriPair2D = [&](const std::size_t idx, const Vector2 topLeft, const Font::Glyph& glyph) {
            const float top = topLeft.y;
            const float left = topLeft.x;
            const float bottom = top + static_cast<float>(glyph.size.y);
            const float right = left + static_cast<float>(glyph.size.x);

            const std::size_t startIdx = idx * 6;

            vertices[startIdx] = { {left, top, 0.f}, {glyph.uvs.left, glyph.uvs.top}};
            vertices[startIdx + 1] = vertices[startIdx + 4] = { {right, top, 0.f}, {glyph.uvs.right, glyph.uvs.top}};
            vertices[startIdx + 2] = vertices[startIdx + 3] = { {left, bottom, 0.f}, {glyph.uvs.left, glyph.uvs.bottom}};
            vertices[startIdx + 5] = { {right, bottom, 0.f}, {glyph.uvs.right, glyph.uvs.bottom}};
        };

        // TODO: clear character slot for spaces/line breaks

        if (mJustify == Justify::left) {
            for (const char c : mText) {
                if (c == '\n') {
                    cursor.x = 0;
                    cursor.y += lineHeight + lineSpacing;
                } else if (c == ' ') {
                    cursor.x += spaceSize;
                } else {
                    const Font::Glyph& glyph = mFont->getGlyph(getFinalChar(c));

                    generateTriPair2D(i, cursor, glyph);

                    cursor.x += static_cast<float>(glyph.size.x) + letterSpacing;
                }
                ++i;
            }
        } else if (mJustify == Justify::right || mJustify == Justify::centre) {
            for (; i < mText.size(); ++i) {
                const std::size_t lineStart = i;

                float lineWidth {};
                for (; mText[i] != '\n' && i < mText.size(); ++i) {
                    const char c = mText[i];
                    if (c == ' ') {
                        lineWidth += spaceSize;
                    } else {
                        lineWidth += static_cast<float>(mFont->getGlyph(getFinalChar(c)).size.x) + letterSpacing;
                    }
                }

                if (i != lineStart && mText[i - 1] != ' ')
                    lineWidth -= letterSpacing;

                cursor.x = getSize().x - lineWidth;
                if (mJustify == Justify::centre)
                    cursor.x /= 2.f;
                cursor.x = std::floor(cursor.x);
                cursor.y = std::floor(cursor.y);

                for (std::size_t j = lineStart; j < i; ++j) {
                    const char c = mText[j];
                    if (c == ' ') {
                        cursor.x += spaceSize;
                    } else {
                        const Font::Glyph& glyph = mFont->getGlyph(getFinalChar(c));

                        generateTriPair2D(j, cursor, glyph);

                        cursor.x += static_cast<float>(glyph.size.x) + letterSpacing;
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
