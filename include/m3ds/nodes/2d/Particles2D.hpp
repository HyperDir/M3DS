#pragma once

#include <m3ds/nodes/2d/Node2D.hpp>
#include <m3ds/reference/resource/ParticleMaterial2D.hpp>

namespace M3DS {
    class Particles2D : public Node2D {
        M_CLASS(Particles2D, Node2D)
    public:
        struct Particle2D {
            Transform2D transform {};
            PixelsPerSecond<Vector2> velocity {};
            RadiansPerSecond<float> angularSpeed {};
            RadiansPerSecond<float> orbitSpeed {};

            std::uint16_t frame {};
            Seconds<float> age {};
        };

        std::shared_ptr<ParticleMaterial2D> particleMaterial = std::make_shared<ParticleMaterial2D>();
        bool oneShot = false;

        void emit() noexcept;
    protected:
        void update(Seconds<float> delta) override;
        void draw(RenderTarget2D& target) override;
    private:
        float mTimer = 0;
        std::size_t mNewestParticle = 0;

        std::vector<Particle2D> mParticles {};
        std::minstd_rand mRandom { std::random_device{}() };

        [[nodiscard]] Particle2D createParticle() noexcept;
    };
}
