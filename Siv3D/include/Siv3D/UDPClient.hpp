# pragma once
# include <memory>
# include "Common.hpp"
# include "IPv4Address.hpp"

namespace s3d
{
    enum class UDPError : uint8
    {
        OK,
        NoBufferSpaceAvailable,
        Error,
    };

    class UDPClient
    {
    public:
        UDPClient();
        ~UDPClient();

        bool open(uint16 localPort = 0);

        void close();

        bool isOpen() const;

        bool hasError() const;

        UDPError getError() const;

        size_t available();

        bool skip(size_t size);

        bool lookahead(void* dst, size_t size) const;

        bool read(void* dst, size_t size);

        bool send(const IPv4Address& remoteIP, uint16 remotePort, const void* data, size_t size);

    private:
        class UDPClientDetail;
        std::unique_ptr<UDPClientDetail> pImpl;
    };
} 