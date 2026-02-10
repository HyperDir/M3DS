#pragma once

#ifdef __3DS__
#include <c3d/renderqueue.h>
#endif

#include <m3ds/utils/Input.hpp>

namespace M3DS {
    class DrawEnvironment {
        friend class Root;

        DrawEnvironment() noexcept {
#ifdef __3DS__
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
#endif
        }
        ~DrawEnvironment() {
#ifdef __3DS__
            C3D_FrameEnd(0);
#endif
        }
    };

    class Frame {
        friend class Root;

        Frame() noexcept {
#ifdef __3DS__
            aptHandleSleep();
            aptHandleJumpToHome();
#endif

            Input::update();
        }
    };
}
