# pragma once
# include <Siv3D/UDPClient.hpp>
# include <Siv3D/AsyncTask.hpp>
# define  ASIO_STANDALONE
# include <asio/asio.hpp>
namespace s3d::detail
{
    class UDPSession;
}
namespace s3d
{
    class UDPClient::UDPClientDetail
    {
    public:
        UDPClientDetail();
        ~UDPClientDetail();

        bool open(uint16 localPort);
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
        std::shared_ptr<asio::io_service> m_io_service;
        std::unique_ptr<asio::io_service::work> m_work;
        AsyncTask<void> m_io_service_thread;

        std::shared_ptr<detail::UDPSession> m_session;
        UDPError m_error = UDPError::OK;
        bool m_isOpen = false;
    };
} 