#pragma once

#ifdef __3DS__
#include <3ds.h>
#include <citro2d.h>

#elifdef M3DS_SFML
#include <SFML/Graphics/Shader.hpp>
#endif

#include <m3ds/nodes/AnimationPlayer.hpp>
#include <m3ds/nodes/AudioPlayer.hpp>
#include <m3ds/nodes/CanvasItem.hpp>
#include <m3ds/nodes/CanvasLayer.hpp>
#include <m3ds/nodes/Node.hpp>
#include <m3ds/nodes/Root.hpp>
#include <m3ds/nodes/Timer.hpp>
#include <m3ds/nodes/Tween.hpp>
#include <m3ds/nodes/Viewport.hpp>

#include <m3ds/nodes/2d/Area2D.hpp>
#include <m3ds/nodes/2d/Camera2D.hpp>
#include <m3ds/nodes/2d/CollisionObject2D.hpp>
#include <m3ds/nodes/2d/KinematicBody2D.hpp>
#include <m3ds/nodes/2d/Node2D.hpp>
#include <m3ds/nodes/2d/Particles2D.hpp>
#include <m3ds/nodes/2d/Sprite2D.hpp>
#include <m3ds/nodes/2d/StaticBody2D.hpp>

#include <m3ds/nodes/3d/Area3D.hpp>
#include <m3ds/nodes/3d/BoneAttachment3D.hpp>
#include <m3ds/nodes/3d/Camera3D.hpp>
#include <m3ds/nodes/3d/CollisionObject3D.hpp>
#include <m3ds/nodes/3d/KinematicBody3D.hpp>
#include <m3ds/nodes/3d/Light3D.hpp>
#include <m3ds/nodes/3d/MeshInstance.hpp>
#include <m3ds/nodes/3d/Node3D.hpp>
#include <m3ds/nodes/3d/PhysicsBody3D.hpp>
#include <m3ds/nodes/3d/StaticBody3D.hpp>

#include <m3ds/nodes/ui/containers/BoxContainer.hpp>
#include <m3ds/nodes/ui/containers/CentreContainer.hpp>
#include <m3ds/nodes/ui/containers/Container.hpp>
#include <m3ds/nodes/ui/containers/FillContainer.hpp>
#include <m3ds/nodes/ui/containers/HBoxContainer.hpp>
#include <m3ds/nodes/ui/containers/MarginContainer.hpp>
#include <m3ds/nodes/ui/containers/PanelContainer.hpp>
#include <m3ds/nodes/ui/containers/ScrollContainer.hpp>
#include <m3ds/nodes/ui/containers/VBoxContainer.hpp>

#include <m3ds/nodes/ui/Button.hpp>
#include <m3ds/nodes/ui/Label.hpp>
#include <m3ds/nodes/ui/Panel.hpp>
#include <m3ds/nodes/ui/ProgressBar.hpp>
#include <m3ds/nodes/ui/TextureRect.hpp>
#include <m3ds/nodes/ui/UINode.hpp>

#include <m3ds/utils/Debug.hpp>
#include <m3ds/utils/Visitor.hpp>

#include <m3ds/reference/resource/TileSet.hpp>

namespace M3DS {
#ifdef __3DS__
    enum class Console {
        top,
        bottom,
        none
    };

    struct SystemEnvironment {
        Console console = Console::bottom;
        bool new3DSEnhancements = true;
        bool stereoscopic3d = true;
    };

    template <SystemEnvironment env = SystemEnvironment{}>
#endif
    struct Init {
        Init() noexcept {
#ifdef __3DS__
            fsInit();
            romfsInit();
            hidInit();

            gfxInitDefault();
            gfxSet3D(env.stereoscopic3d);

            if constexpr (env.console == Console::top)
                consoleInit(GFX_TOP, nullptr);
            if constexpr (env.console == Console::bottom)
                consoleInit(GFX_BOTTOM, nullptr);

            if constexpr (env.console != Console::none)
                Debug::log("M3DS Application Initialised!");

            C3D_Init(0x100000);

            if constexpr (env.new3DSEnhancements) {
                ptmSysmInit();
                if (bool isNew3DS {}; R_SUCCEEDED(PTMSYSM_CheckNew3DS(&isNew3DS)) && isNew3DS) {
                    if (R_FAILED(PTMSYSM_ConfigureNew3DSCPU(0b11)))
                        Debug::err("Failed to set New 3DS clock speed and L2 cache!");
                }
                ptmSysmExit();
            }

            ndspInit();

            ndspSetOutputMode(NDSP_OUTPUT_STEREO);

            ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
            ndspChnSetRate(0, sampleRate);
            ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

            ndspChnSetMix(0, std::array<float, 12>{ 1.f, 1.f }.data());

            std::set_terminate([]{
                Debug::err("\nProgram terminated abruptly.\n");
#ifdef __cpp_exceptions
                const std::exception_ptr ePtr = std::current_exception();
                if (ePtr) {
                    try {
                        std::rethrow_exception(ePtr);
                    } catch (const std::exception& e) {
                        Debug::log("Exception: {}", e.what());
                    } catch (...) {
                        Debug::log("Unknown exception");
                    }
                } else {
                    Debug::log("No exception thrown.");
                }
#endif
                Debug::log("Press START to exit.");

                while (aptMainLoop()) {
                    hidScanInput();
                    if (hidKeysDown() & KEY_START) break;
                }

                std::abort();
            });
#elifdef M3DS_SFML
            if (!sf::Shader::isAvailable())
                Debug::terminate("Shaders not supported!");
#endif

            Registry::clear();
            Registry::registerTypes<
                Object,
                Area2D,
                Camera2D,
                CollisionObject2D,
                KinematicBody2D,
                Node2D,
                Particles2D,
                PhysicsBody2D,
                Sprite2D,
                StaticBody2D,
                Area3D,
                BoneAttachment3D,
                Camera3D,
                CollisionObject3D,
                KinematicBody3D,
                Light3D,
                MeshInstance,
                Node3D,
                PhysicsBody3D,
                StaticBody3D,
                BoxContainer,
                CentreContainer,
                FillContainer,
                HBoxContainer,
                MarginContainer,
                PanelContainer,
                ScrollContainer,
                VBoxContainer,
                Button,
                Label,
                Panel,
                ProgressBar,
                TextureRect,
                UINode,
                AnimationPlayer,
                AudioPlayer,
                CanvasItem,
                CanvasLayer,
                Node,
                Root,
                Timer,
                Tween,
                Viewport,
                Object
                // BaseScript
            >();

            ResourceRegistry::registerResources<
                Font,
                // Mesh,
                ParticleMaterial2D,
                TileSet,
                Resource
            >();
        }

        ~Init() noexcept {
#ifdef __3DS__
	        ndspExit();

            C3D_Fini();
            gfxExit();

            hidExit();

            romfsExit();
            fsExit();
#endif
        }
    };

#define M3DS_REGISTRY_ENTRY(ObjectType) { #ObjectType, RegistryEntry{ []{ return static_cast<std::unique_ptr<Object>>(std::make_unique<ObjectType>()); }, ObjectType::getMemberStatic, ObjectType::getMethodStatic } }
}
