# include <Siv3D/UDPClient.hpp>
# include "UDPClientDetail.hpp"
# include "UDPSession.hpp"

namespace s3d
{
    UDPClient::UDPClientDetail::UDPClientDetail()
    {
    }

    UDPClient::UDPClientDetail::~UDPClientDetail()
    {
        close();
    }

    bool UDPClient::UDPClientDetail::open(uint16 localPort)
    {
        if (!m_work)
        {
            m_io_service = std::make_shared<asio::io_service>();
            m_work = std::make_unique<asio::io_service::work>(*m_io_service);
            m_io_service_thread = Async([this] { m_io_service->run(); });
        }

        if (m_isOpen)
        {
            return false;
        }

        m_session = std::make_shared<detail::UDPSession>(*m_io_service);
        
        try
        {
            m_session->socket().open(asio::ip::udp::v4());
            m_session->socket().bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), localPort));
        }
        catch (const std::exception& e)
        {
            m_error = UDPError::Error;
            return false;
        }

        m_isOpen = true;
        m_session->init();
        m_session->startReceive();
        return true;
    }

    void UDPClient::UDPClientDetail::close()
    {
        if (m_session)
        {
            m_session->close();
            m_session.reset();
        }

        m_isOpen = false;
        m_error = UDPError::OK;

        if (m_work)
        {
            m_work.reset();
            m_io_service->stop();
            m_io_service_thread.wait();
            m_io_service->restart();
            m_io_service.reset();
        }
    }

    bool UDPClient::UDPClientDetail::isOpen() const
    {
        return m_isOpen;
    }

    bool UDPClient::UDPClientDetail::hasError() const
    {
        return getError() != UDPError::OK;
    }

    UDPError UDPClient::UDPClientDetail::getError() const
    {
        if (!m_session || (m_error != UDPError::OK))
        {
            return m_error;
        }
        return m_session->getError();
    }

    size_t UDPClient::UDPClientDetail::available()
    {
        if (!m_session)
        {
            return 0;
        }
        return m_session->available();
    }

    bool UDPClient::UDPClientDetail::skip(size_t size)
    {
        if (!m_session)
        {
            return false;
        }
        return m_session->skip(size);
    }

    bool UDPClient::UDPClientDetail::lookahead(void* dst, size_t size) const
    {
        if (!m_session)
        {
            return false;
        }
        return m_session->lookahead(dst, size);
    }

    bool UDPClient::UDPClientDetail::read(void* dst, size_t size)
    {
        if (!m_session)
        {
            return false;
        }
        return m_session->read(dst, size);
    }

    bool UDPClient::UDPClientDetail::send(const IPv4Address& remoteIP, uint16 remotePort, const void* data, size_t size)
    {
        if (!m_session || !m_isOpen)
        {
            return false;
        }

        asio::ip::udp::endpoint endpoint(
            asio::ip::address::from_string(remoteIP.to_string()),
            remotePort
        );

        m_session->setRemoteEndpoint(endpoint);
        return m_session->send(data, size);
    }
} 