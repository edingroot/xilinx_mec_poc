#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
using namespace boost::asio;


class udpserver
{
    public:
	udpserver(io_service & service, unsigned char ue_udp_port):socket_(service,ip::udp::endpoint(ip::udp::v4(), 0)), ue_udp_port_(ue_udp_port)
    {
		current_port = socket_.local_endpoint().port();
		start_receive();
    }
	unsigned short current_port;

    private:
	void start_receive()
	{
	    socket_.async_receive_from(buffer(recv_buffer_,max_length),
		    remote_endpoint_,
		    boost::bind(&udpserver::handle_receive,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_send(std::shared_ptr<std::string> message,
		const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
	    cout<<"received_buffer: ";
	    cout<<*message;
	}
	

	void handle_receive(const boost::system::error_code& error,
		std::size_t bytes_transferred)
	{
	    if (!error || error == boost::asio::error::message_size)
	    {
		if(bytes_transferred>0)
		{
		    std::string*ptr_string= new std::string(recv_buffer_,recv_buffer_+bytes_transferred);
		    std::shared_ptr<std::string> message(ptr_string);
			std::cout << *ptr_string; // print buffer

			// connect to UE assigned UDP port
			boost::asio::ip::address_v4 ue_ipv4 = remote_endpoint_.address().to_v4();
			boost::asio::ip::udp::endpoint ue_udp_endpoint(ue_ipv4, ue_udp_port_);

			// send data to UE assigned UDP port
		    socket_.async_send_to(boost::asio::buffer(recv_buffer_,bytes_transferred), ue_udp_endpoint,
			    boost::bind(&udpserver::handle_send, this, message,
			    boost::asio::placeholders::error,
			    boost::asio::placeholders::bytes_transferred));

			// socket_.async_send_to(boost::asio::buffer(recv_buffer_,bytes_transferred), remote_endpoint_,
			//     boost::bind(&udpserver::handle_send, this, message,
			//     boost::asio::placeholders::error,
			//     boost::asio::placeholders::bytes_transferred));

		    start_receive();
		}
	    }
	}
	boost::thread t;
	ip::udp::socket socket_;
	unsigned short ue_udp_port_;
	ip::udp::endpoint remote_endpoint_;
	enum {max_length=1024};
	char recv_buffer_[max_length];
//	boost::array<char,1024>recv_buffer_;

};

