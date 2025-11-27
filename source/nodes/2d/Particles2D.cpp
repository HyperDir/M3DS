#include <m3ds/nodes/2d/Particles2D.hpp>

namespace M3DS {
    void Particles2D::emit() noexcept {
        if (oneShot)
            mParticles.clear();
    }

    Error Particles2D::serialise(const BinaryOutFileAccessor file) const noexcept {
        if (const Error error = Node2D::serialise(file); error != Error::none)
            return error;

        if (!file.write(oneShot))
            return Error::file_write_fail;

        if (const Error error = M3DS::serialise(particleMaterial.get(), file); error != Error::none)
            return error;

        return Error::none;
    }

    Error Particles2D::deserialise(const BinaryInFileAccessor file) noexcept {
        if (const Error error = Node2D::deserialise(file); error != Error::none)
            return error;

        if (!file.read(oneShot))
            return Error::file_read_fail;

        if (const Error error = M3DS::deserialise(particleMaterial, file); error != Error::none)
            return error;

        return Error::none;
    }

    void Particles2D::update(const Seconds<float> delta) {
        Node2D::update(delta);

        if (!particleMaterial) {
            mTimer = 0;
            return;
        }
        mTimer -= delta;

        if (mParticles.size() > particleMaterial->count) {
            mParticles.resize(particleMaterial->count);
        } else {
            mParticles.reserve(particleMaterial->count);
        }

        while (mTimer <= 0 && mParticles.size() < particleMaterial->count) {
            mTimer += particleMaterial->duration / static_cast<float>(particleMaterial->count);
            mParticles.emplace_back(createParticle());
            mNewestParticle = mParticles.size() - 1;
        }

        bool any = false;

        for (auto it = mParticles.begin(); it != mParticles.end(); ++it) {
            auto& particle = *it;

            if (particle.age > particleMaterial->duration) {
                if (!oneShot) {
                    particle = createParticle();
                    mNewestParticle = std::distance(mParticles.begin(), it);
                } else continue;
            }

            any = true;
            particle.transform.position += particle.velocity * delta;

            particle.velocity += particle.velocity.normalise() * particleMaterial->linearAcceleration * delta;
            particle.velocity += particle.transform.position.normalise().rotated(std::numbers::pi_v<float>/2.f) * particleMaterial->tangentialAcceleration * delta;
            particle.velocity += particleMaterial->globalAcceleration * delta;

            particle.transform.rotation += particle.angularSpeed * delta;
            particle.angularSpeed += particleMaterial->angularAcceleration * delta;

            particle.transform.position += particle.transform.position.normalise().rotated(std::numbers::pi_v<float>/2.f) * particle.orbitSpeed * delta;
            particle.orbitSpeed += particleMaterial->orbitAcceleration * delta;

            particle.age += delta;
        }

        if (oneShot && !any) {
            mParticles.clear();
        }
    }

    void Particles2D::draw(RenderTarget2D& target) {
        if (!particleMaterial || !particleMaterial->texture)
            return;

        const auto drawParticle = [&](const Particle2D& particle) {
            if (particle.age > particleMaterial->duration)
                return;
            const Transform2D xform = getGlobalTransform().offset(particle.transform);

            target.drawTextureFrame(
                particleMaterial->texture,
                xform,
                particle.frame
            );
        };

        // Rotated reverse iteration
        // Newest particles draw under older particles
        for (std::size_t i {}; i < mNewestParticle; ++i) {
            drawParticle(mParticles[mNewestParticle - i]);
        }

        for (std::size_t i = mParticles.size() - 1; i > mNewestParticle; --i) {
            drawParticle(mParticles[i]);
        }

        Node2D::draw(target);
    }

    Particles2D::Particle2D Particles2D::createParticle() noexcept {
        return {
            { .scale = particleMaterial->scale(mRandom) },
            Vector2::fromAngle(particleMaterial->startAngle(mRandom)) * particleMaterial->speed(mRandom),
            particleMaterial->angularSpeed(mRandom),
            particleMaterial->orbitSpeed(mRandom),
            particleMaterial->frame(mRandom)
        };
    }

    REGISTER_METHODS(
        Particles2D,
        MUTABLE_METHOD(emit)
    );

    REGISTER_MEMBERS(
        Particles2D,
        MEMBER(oneShot)
    );
}
