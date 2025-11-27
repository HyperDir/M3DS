#pragma once

#include <m3ds/nodes/Node.hpp>
#include <m3ds/utils/WAV.hpp>
#include <m3ds/utils/LinearAllocator.hpp>

namespace M3DS {
    constexpr std::size_t sampleRate = 22050;
    constexpr std::size_t samplesPerBuf = sampleRate / 30;
    constexpr std::size_t bytesPerSample = 4;

    // TODO: serialisation

    class AudioPlayer : public Node {
        M_CLASS(AudioPlayer, Node)
    public:
        AudioPlayer() noexcept;

        void play(std::shared_ptr<WAV> wavFile) noexcept;

        void resume() noexcept;
        void pause() noexcept;
        void stop() noexcept;
        void restart() noexcept;

        bool loop = true;
    protected:
        void update(Seconds<float> delta) override;
    private:
#ifdef __3DS__
        LinearHeapArray<std::array<std::byte, 4>> mBuffer { samplesPerBuf * 2 };
        std::shared_ptr<WAV> mWav {};
        std::array<ndspWaveBuf, 2> mWaveBuf {};
        long mStreamOffset {};

        bool mFillBlock {};
#endif
        bool mPaused {};
    };
}
