#include <m3ds/nodes/AudioPlayer.hpp>

namespace M3DS {
#ifdef __3DS__
    AudioPlayer::AudioPlayer() noexcept {
        memset(mWaveBuf.data(), 0, sizeof(mWaveBuf));

        mWaveBuf[0].data_vaddr = mBuffer.data();
        mWaveBuf[0].status = NDSP_WBUF_DONE;
        mWaveBuf[0].nsamples = samplesPerBuf;

        mWaveBuf[1].data_vaddr = mBuffer.data() + samplesPerBuf;
        mWaveBuf[1].status = NDSP_WBUF_DONE;
        mWaveBuf[1].nsamples = samplesPerBuf;
    }

    void AudioPlayer::play(std::shared_ptr<WAV> wavFile) noexcept {
        mWav = std::move(wavFile);
        mStreamOffset = 0;
        resume();
    }

    void AudioPlayer::resume() noexcept {
        mPaused = false;
    }

    void AudioPlayer::pause() noexcept {
        mPaused = true;
        ndspChnWaveBufClear(0);
        // ndspChnReset(0);
    }

    void AudioPlayer::stop() noexcept {
        mStreamOffset = 0;
        pause();
    }

    void AudioPlayer::restart() noexcept {
        mStreamOffset = 0;
        resume();
    }

    void AudioPlayer::update(const Seconds<float> delta) {
        Node::update(delta);
        if (mPaused)
            return;

        ndspWaveBuf& curBuf = mWaveBuf[mFillBlock];
        if (mWav && curBuf.status == NDSP_WBUF_DONE) {
            auto* dest = curBuf.data_pcm16;

            const WAV::FormatInfo& info = mWav->getFormatInfo();
            const std::size_t frameSize = info.channels * (info.bitsPerSample / 8);
            const std::uint32_t frameCount = curBuf.nsamples;

            mWav->seek(mStreamOffset, std::ios::beg);

            const bool atEnd = !mWav->read(std::span{
                reinterpret_cast<std::byte*>(dest),
                frameCount * frameSize
            });

            if (atEnd) {
                if (loop) {
                    mStreamOffset = 0;
                } else {
                    stop();
                }
            } else {
                mStreamOffset = mWav->tell();
            }

            DSP_FlushDataCache(dest, frameCount * info.channels * sizeof(int16_t));

            ndspChnWaveBufAdd(0, &curBuf);
            mFillBlock = !mFillBlock;
        }
    }
#endif

    Failure AudioPlayer::serialise([[maybe_unused]] BinaryOutFileAccessor file) const noexcept {
        Debug::err("AudioPlayer Serialisation not implemented!");
        return Failure{ ErrorCode::not_implemented };
    }

    Failure AudioPlayer::deserialise([[maybe_unused]] BinaryInFileAccessor file) noexcept {
        Debug::err("AudioPlayer Serialisation not implemented!");
        return Failure{ ErrorCode::not_implemented };
    }



    REGISTER_METHODS(
        AudioPlayer,
        MUTABLE_METHOD(resume),
        MUTABLE_METHOD(pause),
        MUTABLE_METHOD(stop),
        MUTABLE_METHOD(restart)
    );

    REGISTER_MEMBERS(
        AudioPlayer,
        MEMBER(loop)
    );
}
