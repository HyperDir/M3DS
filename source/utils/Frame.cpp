#include <m3ds/utils/Frame.hpp>

#include <m3ds/utils/Input.hpp>

#ifdef __3DS__
extern "C" {
    #include <3ds/types.h>
    #include <3ds/services/apt.h>
}
#endif

namespace M3DS {
    Frame::Frame() noexcept {
#ifdef __3DS__
        aptHandleSleep();
        aptHandleJumpToHome();
#endif

        Input::update();
    }
}
