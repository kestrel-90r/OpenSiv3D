# include <Siv3D/UDPClient.hpp>
# include "UDPClientDetail.hpp"

namespace s3d
{
    UDPClient::UDPClient()
        : pImpl{ std::make_unique<UDPClientDetail>() } {}

    UDPClient::~UDPClient() = default;

    bool UDPClient::open(const uint16 localPort)
    {
        return pImpl->open(localPort);
    }

    void UDPClient::close()
    {
        pImpl->close();
    }

    bool UDPClient::isOpen() const
    {
        return pImpl->isOpen();
    }

    bool UDPClient::hasError() const
    {
        return pImpl->hasError();
    }

    UDPError UDPClient::getError() const
    {
        return pImpl->getError();
    }

    size_t UDPClient::available()
    {
        return pImpl->available();
    }

    bool UDPClient::skip(const size_t size)
    {
        return pImpl->skip(size);
    }

    bool UDPClient::lookahead(void* dst, const size_t size) const
    {
        return pImpl->lookahead(dst, size);
    }

    bool UDPClient::read(void* dst, const size_t size)
    {
        return pImpl->read(dst, size);
    }

    bool UDPClient::send(const IPv4Address& remoteIP, const uint16 remotePort, const void* data, const size_t size)
    {
        return pImpl->send(remoteIP, remotePort, data, size);
    }
} 