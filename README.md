# M3DS - A 3DS Game Development Framework

M3DS is a library created to simplify the 3DS game development process. It provides simple utilities for:

- Loading, rendering, and animation of 3D models through a custom `.mod3ds` format.
- Loading and playback of WAV audio files (with some sample rate requirements).
- Creating customisable User Interfaces.
- Obtaining input from the user.
- Safely and efficiently reading and writing of binary files.
- Serialisation and Deserialisation to binary files.
- Creating custom animations based off any reasonable public fields on a `Node` object.
- Simple script interface for trivial extensibility.
- Complicated Node inheritance for complex extensibility.

M3DS provides a Godot-like Node system, making organising projects simple. Additionally, M3DS has its own simple physics engine, SPhys, designed primarily for Kinematic Bodies rather than Rigid Bodies.

M3DS does not use exceptions or runtime type information, however it can be recompiled with the former enabled to support catching `std::bad_alloc`.

## Build Instructions
1. Ensure `libctru` is installed and up to date.
    - On Windows, use the devkitPro Updater at https://github.com/devkitPro/installer/releases
    - On Linux or macOS, follow the instructions at https://devkitpro.org/wiki/Getting_Started
2. Open a terminal in the `m3ds` folder.
3. Run `make`. To install system-wide, run `make install` instead.

The final `libm3ds.a` will be found in the `lib` directory.

## Example: Hello World
```c++
#include <m3ds/M3DS.hpp>

int main() {
    // Initialises systems, sets bottom screen as console (configurable)
    M3DS::Init _ {};

    // Supports formatting through std::format
    M3DS::Debug::log("Hello, {}!", "World");

    // Create a root node and begin main loop (so program doesn't exit immediately)
    // Press START to exit in default mainLoop.
    M3DS::Root root {};
    root.mainLoop();
}
```
Compile under regular libctru rules, linking against M3DS with `-lm3ds`. 
Compile using C++26 with `-std=c++26`.
Also requires linking with Citro2D and Citro3D with `-lcitro2d` and `-lcitro3d`.

## Example: Rendering a 3D Model with Skeletal Animations
```c++
#include <m3ds/M3DS.hpp>

int main() {
    // Initialise systems, sets bottom screen as console (configurable)
    M3DS::Init _ {};

    // Create a root node
    M3DS::Root root {};
    // Create a viewport for the top screen
    M3DS::Viewport* topScreen = root.emplaceChild<M3DS::Viewport>(M3DS::Screen::top);

    // Create a light for the scene
    M3DS::Light3D* light = topScreen->emplaceChild<M3DS::Light3D>();
    light->setTranslation(M3DS::Vector3{1, 1, 0});
    light->enable();

    // Create a camera for the scene. Camera forward direction is -z by default
    M3DS::Camera3D* camera = topScreen->emplaceChild<M3DS::Camera3D>();
    camera->setTranslation(M3DS::Vector3{0, 1.5, 5});
    camera->enable();

    // Load a 3D model (file converted from glTF using M3DSModelConverter)
    std::expected mesh = M3DS::Mesh::load("romfs:/GDBot.mod3ds");
    if (!mesh)
        M3DS::Debug::terminate("Failed to load model with error code: {}!", std::to_underlying(mesh.error()));

    // Create a MeshInstance using the mesh loaded in the previous step
    M3DS::MeshInstance* meshInstance = topScreen->emplaceChild<M3DS::MeshInstance>(mesh.value());

    const std::size_t animationCount = meshInstance->getAnimationCount();
    if (animationCount != 0) {
        M3DS::Debug::log("Playing animation {}", 0);
        meshInstance->playAnimationIdx(0);
    }

    std::size_t idx {};

    // Begin main loop
    // Lambda can be passed to Root::mainLoop() to modify behaviour.
    root.mainLoop([&](const float delta) {
        if (M3DS::Input::isKeyPressed(M3DS::Input::Key::start))
            root.exit();

        if (M3DS::Input::isKeyPressed(M3DS::Input::Key::a)) {
            idx = (idx + 1) % animationCount;
            M3DS::Debug::log("Playing animation {}", idx);
            meshInstance->playAnimationIdx(idx);
        }

        root.treeInput();
        root.treeUpdate(delta);
        root.treeDraw(M3DS::Draw::draw_3d);
    });
}
```

## Example: Creating a Script

Note: Scripts currently do not support serialisation. To allow serialisation, a custom Node must be created and registered to the Registry.

```c++
#include <m3ds/M3DS.hpp>

// Create a custom script based on Node
class MyScript : public M3DS::Script<M3DS::Node> {
public:
    // Called when script is attached to Node
    void ready() override {
        // Base node can be accessed with base()
        M3DS::Debug::log("Attached MyScript to Node: {}!", base()->getClass());
    }

    // Called every frame
    void update(M3DS::Seconds<float> delta) override {
        M3DS::Debug::log("Frame Time: {}", delta);
    }

    // Draw calls
    void draw([[maybe_unused]] const M3DS::RenderTarget2D& target) override {}
    void draw([[maybe_unused]] const M3DS::RenderTarget3D& target) override {}

    // Called on input events
    void input(M3DS::Input::InputFrame& inputFrame) override {
        M3DS::Debug::log("Left Joy: {:.4}", inputFrame.getLeftJoy());
    }

    // Custom functions can be added
    void myFunc() const {
        M3DS::Debug::log("myFunc()");
    }
};

int main() {
    M3DS::Init _ {};

    M3DS::Root root {};

    // Instantiate script (creates Node child and attaches script automatically)
    MyScript* script = root.emplaceChild<MyScript>();
    script->myFunc();

    root.mainLoop();
}
```

## Example: Custom Node
Creating a custom Node requires implementing four special member functions. Additionally, to properly enable serialisation, the custom Node must be registered with the Registry. The `REGISTER_MEMBERS`/`REGISTER_NO_MEMBERS` and `REGISTER_METHODS`/`REGISTER_NO_METHODS` macros can be used in place of manually writing out the full definition. 

Only certain types are supported for binding. These are listed in the BindableTypes TypePack.
```c++
#include <m3ds/M3DS.hpp>

class MyNode : public M3DS::Node {
    M_CLASS(MyNode, M3DS::Node);
};

M3DS::Error MyNode::serialise(M3DS::BinaryOutFileAccessor file) const noexcept {
    return SuperType::serialise(file);
}

M3DS::Error MyNode::deserialise(M3DS::BinaryInFileAccessor file) noexcept {
    return SuperType::deserialise(file);
}

const M3DS::GenericMember* MyNode::getMemberStatic(const std::string_view name) noexcept {
    return SuperType::getMemberStatic(name);
}

M3DS::BoundMethodPair MyNode::getMethodStatic(const std::string_view name) noexcept {
    return SuperType::getMethodStatic(name);
}

int main() {
    M3DS::Init _ {};
    // Register MyNode in the Registry.
    M3DS::Registry::registerType<MyNode>();

    M3DS::Root root {};
    root.emplaceChild<MyNode>();

    root.mainLoop();
}
```
