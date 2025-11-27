#pragma once

#include <vector>

#include <m3ds/containers/HeapArray.hpp>
#include <m3ds/utils/Debug.hpp>

#include <m3ds/nodes/Node.hpp>

#include <m3ds/utils/binding/Registry.hpp>

namespace M3DS {
    template <typename... Args>
    class AbstractSignal {
        std::vector<std::pair<NodePath, const MutableGenericMethod*>> mConnections {};
    public:
        constexpr AbstractSignal() noexcept = default;

        void connect(const MutableGenericMethod* method, NodePath path) {
            mConnections.emplace_back(
                std::move(path),
                method
            );
        }

        void emit(Node* owner, Args... args) const {
            Debug::log<1>("Emitting Signal... Caught by {} listeners.", mConnections.size());
            for (const auto& [path, connection]: mConnections) {
                if (Node* node = owner->getNode(path)) {
                    std::ignore = connection->autoCall(node, args...);
                } else {
                    Debug::err("Invalid NodePath in Signal Emission! {}", path);
                }
            }
        }

        void disconnect(const NodePath& obj) {
            erase_if(mConnections, [&](auto& pair) { return pair.first == obj; });
        }

        void disconnectAll() {
            mConnections.clear();
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
        [[nodiscard]] Error serialise(const BinaryOutFileAccessor file) const {
            Debug::log<1>("Serialising signal...");
            {
                const SerialisationHeader header {
                    .connections = static_cast<std::uint16_t>(mConnections.size())
                };
                if (!file.write(header))
                    return Error::file_write_fail;
            }

            for (const auto& [path, connection] : mConnections) {
                if (path.empty())
                    return Error::failed_to_get_path_to_node;

                const std::string_view pathView = path.toString();
                const std::string_view methodClassName = connection->getClassName();
                const std::string_view methodName = connection->getName();

                const ConnectionHeader header {
                    .pathLength = static_cast<std::uint16_t>(pathView.length()),
                    .methodClassNameLength = static_cast<std::uint16_t>(methodClassName.length()),
                    .methodNameLength = static_cast<std::uint16_t>(methodName.length()),
                };
                if (
                    !file.write(header) ||
                    !file.write(std::span{pathView}) ||
                    !file.write(std::span{methodClassName}) ||
                    !file.write(std::span{methodName})
                ) {
                    return Error::file_write_fail;
                }
            }

            return Error::none;
        }

        [[nodiscard]] Error deserialise(const BinaryInFileAccessor file) {
            Debug::log<1>("Deserialising signal...");

            SerialisationHeader header {};
            if (!file.read(header))
                return Error::file_read_fail;

            mConnections.resize(header.connections);

            for (auto&& [path, connection] : mConnections) {
                ConnectionHeader connectionHeader {};
                if (!file.read(connectionHeader))
                    return Error::file_read_fail;

                HeapArray<char> data {};
                data.resize(connectionHeader.pathLength + connectionHeader.methodClassNameLength + connectionHeader.methodNameLength);
                if (!file.read(std::span{data}))
                    return Error::file_read_fail;

                const std::string_view methodClassName { data.data() + connectionHeader.pathLength, connectionHeader.methodClassNameLength };
                const std::string_view methodName { data.data() + connectionHeader.methodClassNameLength, connectionHeader.methodNameLength };

                path = NodePath { std::string(data.data(), connectionHeader.pathLength) };

                connection = Registry::getMutableMethod(methodClassName, methodName);
            }

            return Error::none;
        }
    };

    using Signal = AbstractSignal<>;
}
