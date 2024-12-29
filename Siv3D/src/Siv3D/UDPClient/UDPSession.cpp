# include <Siv3D/Array.hpp>
# include <Siv3D/Common.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/UDPClient.hpp> 
# include "UDPSession.hpp"

namespace s3d::detail
{
    UDPSession::UDPSession(asio::io_service& io_service)
        : m_socket(io_service), m_receiveBuffer(receiveBufferSize)
    {
        m_receiveBuffer.resize(receiveBufferSize);
    }

    void UDPSession::send_internal()
    {
        if (m_sendingBuffer.empty())
        {
            m_isSending = false;
            return;
        }

        m_isSending = true;
        const Array<Byte>& data = m_sendingBuffer.front();

        m_socket.async_send_to(
            asio::const_buffer(data.data(), data.size()),
            m_remoteEndpoint,
            [this, self = shared_from_this()](const asio::error_code& error, size_t bytesTransferred)
            {
                onSend(error, bytesTransferred, self);
            }
        );
    }

    void UDPSession::onReceive(const asio::error_code& error, size_t bytesTransferred, const std::shared_ptr<UDPSession>&)
    {
        if (error)
        {
            m_error = UDPError::Error;
            close();
            return;
        }

        {
            std::lock_guard lock{ m_mutexReceivedBuffer };
            if (m_receivedBuffer.size() + bytesTransferred > maxBufferSize)
            {
                m_error = UDPError::NoBufferSpaceAvailable;
                close();
                return;
            }
            m_receivedBuffer.insert(m_receivedBuffer.end(),
                m_receiveBuffer.begin(),
                m_receiveBuffer.begin() + bytesTransferred);
        }

        startReceive();
    }

    void UDPSession::startReceive()
    {
        if (!m_isActive)
        {
            return;
        }

        m_socket.async_receive_from(
            asio::mutable_buffer(m_receiveBuffer.data(), receiveBufferSize),
            m_remoteEndpoint,
            [this, self = shared_from_this()](const asio::error_code& error, size_t bytesTransferred)
            {
                onReceive(error, bytesTransferred, self);
            }
        );
    }


    void UDPSession::setRemoteEndpoint(const asio::ip::basic_endpoint<asio::ip::udp>& endpoint)
    {
        m_remoteEndpoint = endpoint;
    }

    asio::basic_datagram_socket<asio::ip::udp, asio::any_io_executor>& UDPSession::socket()
    {
        return m_socket;
    }

    void UDPSession::init()
    {
        m_isActive = true;
        startReceive();
    }

    void UDPSession::close()
    {
        m_isActive = false;
        if (m_socket.is_open())
        {
            m_socket.close();
        }
    }

    UDPError UDPSession::getError() const
    {
        return m_error;
    }

    size_t UDPSession::available()
    {
        std::lock_guard lock{ m_mutexReceivedBuffer };
        return m_receivedBuffer.size();
    }

    bool UDPSession::skip(size_t size)
    {
        std::lock_guard lock{ m_mutexReceivedBuffer };
        if (m_receivedBuffer.size() < size)
        {
            return false;
        }
        m_receivedBuffer.erase(m_receivedBuffer.begin(), m_receivedBuffer.begin() + size);
        return true;
    }

    bool UDPSession::lookahead(void* dst, size_t size) const
    {
        std::lock_guard lock{ m_mutexReceivedBuffer };
        if (m_receivedBuffer.size() < size)
        {
            return false;
        }
        std::memcpy(dst, m_receivedBuffer.data(), size);
        return true;
    }

    bool UDPSession::read(void* dst, size_t size)
    {
        if (!lookahead(dst, size))
        {
            return false;
        }
        return skip(size);
    }

    bool UDPSession::send(const void* data, size_t size)
    {
        if (!m_isActive)
        {
            return false;
        }

        Array<Byte> buffer(static_cast<const Byte*>(data), static_cast<const Byte*>(data) + size);

        {
            std::lock_guard lock{ m_mutexSendingBuffer };
            m_sendingBuffer.push_back(std::move(buffer));
        }

        if (!m_isSending)
        {
            send_internal();
        }

        return true;
    }
    void UDPSession::onSend(const std::error_code& error, size_t bytesTransferred, const std::shared_ptr<UDPSession>& /*self*/)
    {
        if (error)
        {
            m_error = UDPError::Error;
            close();
            return;
        }

        {
            std::lock_guard lock{ m_mutexSendingBuffer };
            m_sendingBuffer.pop_front();
        }

        send_internal();
    }
} 