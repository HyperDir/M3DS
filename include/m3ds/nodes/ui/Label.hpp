#pragma once

#include <m3ds/nodes/ui/UINode.hpp>
#include <m3ds/containers/HeapArray.hpp>
#include <m3ds/reference/resource/Font.hpp>

namespace M3DS {
    class Label : public UINode {
        M_CLASS(Label, UINode)
    public:
        enum class Justify : std::uint8_t {
            left,
            centre,
            right
        };

        Colour colour = Colours::black;

        [[nodiscard]] std::string_view getText() const noexcept;
        [[nodiscard]] const std::shared_ptr<Font>& getFont() const noexcept;
        [[nodiscard]] bool getCapitalise() const noexcept;
        [[nodiscard]] Justify getJustify() const noexcept;

        void setText(std::string_view text) noexcept;
        void setFont(std::shared_ptr<Font> font) noexcept;
        void setCapitalise(bool to) noexcept;
        void setJustify(Justify to) noexcept;
    protected:
        void draw(RenderTarget2D &target) override;
        void updateMinSize() noexcept override;
        void resize() noexcept override;
    private:
        struct LabelGlyph {
            Vector2i position {};
            const Font::Glyph* glyph {};
        };

        std::string mText {};
        std::shared_ptr<Font> mFont = Font::getDefaultFont();
        std::vector<LabelGlyph> mGlyphs {};
        bool mCapitalise {};
        Justify mJustify = Justify::left;

        char getFinalChar(char c) const noexcept;
    };
}
