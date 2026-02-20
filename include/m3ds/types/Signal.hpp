#pragma once

#include <vector>

#include <m3ds/containers/HeapArray.hpp>
#include <m3ds/utils/Debug.hpp>

#include <m3ds/nodes/Node.hpp>

#include <m3ds/utils/binding/Registry.hpp>

namespace M3DS {
    class BaseSignal {
        friend class Node;
    public:
        ~BaseSignal() noexcept {
            disconnectAll();
        }

        void connect(Node* node, const MutableGenericMethod* method) {
            if (method) {
                mConnections.emplace_back(node, method);
                node->mIncomingConnections.emplace_back(this, method);
            } else {
                Debug::err("Invalid method passed to signal!");
            }
        }

        void disconnect(Node* node, const MutableGenericMethod* method) {
            std::erase_if(mConnections, [&](auto& pair) { return pair.first == node && pair.second == method; });
            std::erase_if(node->mIncomingConnections, [&](auto& pair) { return pair.first == this && pair.second == method; });
        }

        void disconnect(Node* node) {
            std::erase_if(mConnections, [node](auto& pair) { return pair.first == node; });
            std::erase_if(node->mIncomingConnections, [this](auto& pair) { return pair.first == this; });
        }

        void disconnectAll() {
            for (Node* node : mConnections | std::views::keys)
                std::erase_if(node->mIncomingConnections, [this](auto& pair) { return pair.first == this; });
            mConnections.clear();
        }
    protected:
        std::vector<std::pair<Node*, const MutableGenericMethod*>> mConnections {};
    };

    template <typename... Args>
    class AbstractSignal : public BaseSignal {
    public:
        constexpr AbstractSignal() noexcept = default;

        void emit([[maybe_unused]] Args... args) const {
            Debug::log<1>("Emitting Signal... Caught by {} listeners.", mConnections.size());
            for (const auto& [node, connection]: mConnections) {
                std::ignore = connection->autoCall(node, args...);
            }
        }
    private:
        struct SerialisationHeader {
            std::uint16_t connections {};
        };

        struct ConnectionHeader {
            std::uint16_t pathLength {};
            std::uint16_t methodClassNameLength {};
            std::uint16_t methodNameLength {};
        };
    public:
        [[nodiscard]] Failure serialise(const Node* owner, Serialiser& serialiser) const {
            Debug::log<1>("Serialising signal...");
            {
                const SerialisationHeader header {
                    .connections = static_cast<std::uint16_t>(mConnections.size())
                };
                if (!serialiser.write(header))
                    return Failure{ ErrorCode::file_write_fail };
            }

            for (const auto& [node, connection] : mConnections) {
                const NodePath path = owner->getPathTo(node);

                if (path.empty())
                    return Failure{ ErrorCode::failed_to_get_path_to_node };

                const std::string_view pathView = path.toString();
                const std::string_view methodClassName = connection->getClassName();
                const std::string_view methodName = connection->getName();

                const ConnectionHeader header {
                    .pathLength = static_cast<std::uint16_t>(pathView.length()),
                    .methodClassNameLength = static_cast<std::uint16_t>(methodClassName.length()),
                    .methodNameLength = static_cast<std::uint16_t>(methodName.length()),
                };
                if (
                    !serialiser.write(header) ||
                    !serialiser.write(std::span{pathView}) ||
                    !serialiser.write(std::span{methodClassName}) ||
                    !serialiser.write(std::span{methodName})
                ) {
                    return Failure{ ErrorCode::file_write_fail };
                }
            }

            return Success;
        }

        [[nodiscard]] Failure deserialise(Node* owner, Deserialiser& deserialiser) {
            Debug::log<1>("Deserialising signal...");

            SerialisationHeader header {};
            if (!deserialiser.read(header))
                return Failure{ ErrorCode::file_read_fail };

            std::vector<std::pair<NodePath, const MutableGenericMethod*>> connections { header.connections };

            for (auto&& [path, connection] : connections) {
                ConnectionHeader connectionHeader {};
                if (!deserialiser.read(connectionHeader))
                    return Failure{ ErrorCode::file_read_fail };

                HeapArray<char> data {};
                data.resize(connectionHeader.pathLength + connectionHeader.methodClassNameLength + connectionHeader.methodNameLength);
                if (!deserialiser.read(std::span{data}))
                    return Failure{ ErrorCode::file_read_fail };

                const std::string_view methodClassName { data.data() + connectionHeader.pathLength, connectionHeader.methodClassNameLength };
                const std::string_view methodName { data.data() + connectionHeader.pathLength + connectionHeader.methodClassNameLength, connectionHeader.methodNameLength };

                path = NodePath { std::string(data.data(), connectionHeader.pathLength) };

                connection = Registry::getMutableMethod(methodClassName, methodName);
            }

            deserialiser.defer(
                [owner, this, data=std::move(connections)] {
                    assert(mConnections.empty());
                    mConnections.reserve(data.size());

                    for (auto&& [path, method] : data) {
                        if (Node* node = owner->getNode(path)) {
                            mConnections.emplace_back(node, method);
                        } else {
                            Debug::err("Invalid NodePath in Signal Emission! {}", path);
                        }
                    }
                }
            );

            return Success;
        }
    };

    using Signal = AbstractSignal<>;
}
