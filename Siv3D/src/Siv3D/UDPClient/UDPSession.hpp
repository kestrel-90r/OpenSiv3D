# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/Common.hpp>
# include <Siv3D/Byte.hpp>
# define  ASIO_STANDALONE
# include <asio/asio.hpp>

namespace s3d::detail
{
    class UDPSession : public std::enable_shared_from_this<UDPSession>
    {
    private:
        asio::ip::udp::socket m_socket;
        UDPError m_error = UDPError::OK;
        bool m_isActive = false;
        asio::ip::udp::endpoint m_remoteEndpoint;

        static constexpr size_t maxBufferSize = 32 * 1024 * 1024;
        static constexpr size_t receiveBufferSize = 65536;

        mutable std::mutex m_mutexReceivedBuffer;
        mutable std::mutex m_mutexSendingBuffer;

        Array<Byte> m_receivedBuffer;
        Array<Byte> m_receiveBuffer;

        Array<Array<Byte>> m_sendingBuffer;
        bool m_isSending = false;

        void send_internal();
        void onSend(const asio::error_code& error, size_t bytesTransferred, const std::shared_ptr<UDPSession>& session);
        void onReceive(const asio::error_code& error, size_t bytesTransferred, const std::shared_ptr<UDPSession>& session);

    public:
        explicit UDPSession(asio::io_service& io_service);
        
        void setRemoteEndpoint(const asio::ip::udp::endpoint& endpoint);
        asio::ip::udp::endpoint getRemoteEndpoint() const;
        asio::ip::udp::socket& socket();
        
        void init();
        void close();
        void startReceive();
        
        bool isActive() const;
        UDPError getError() const;
        
        size_t available();
        bool skip(size_t size);
        bool lookahead(void* dst, size_t size) const;
        bool read(void* dst, size_t size);
        bool send(const void* data, size_t size);
    };
} 