#include <iostream>
#include <random>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace std;
using namespace boost::asio;

#define MAX_PACK 100
#define CHUNK_PACK_SIZE 4096
#define BUFFER_MAX_LENGTH 65540
#define HEAD_PACK_SIZE 4

class udpserver
{
    public:
	udpserver(io_service & service, unsigned short ue_udp_port):socket_(service,ip::udp::endpoint(ip::udp::v4(), 0)), ue_udp_port_(ue_udp_port)
    {
		total_pack = -1;
		current_port = socket_.local_endpoint().port();
		start_receive();
    }
	unsigned short current_port;

    private:
	void start_receive()
	{
	    socket_.async_receive_from(buffer(recv_buffer_,BUFFER_MAX_LENGTH),
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
	    // cout<<"received_buffer: ";
	    // cout<<*message<<endl;
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
				// std::cout << *ptr_string; // print buffer

				std::cout << "Start to receive jpg PACK" << std::endl;

				char *longbuf = NULL;
				bool isExistedFirstPACK = false;

				if (total_pack == -1 && bytes_transferred == HEAD_PACK_SIZE) { // Normal case
					std::cout << "Normal case" << std::endl;
					total_pack = ((int * ) recv_buffer_)[0];
					longbuf = new char[CHUNK_PACK_SIZE * total_pack];
				} else if (total_pack != -1 && bytes_transferred == CHUNK_PACK_SIZE) { // Drop and renew case and put the first PACK into buffer
					std::cout << "Drop and renew PACK case" << std::endl;
					longbuf = new char[CHUNK_PACK_SIZE * total_pack];
					memcpy(&longbuf[0 * CHUNK_PACK_SIZE], recv_buffer_, CHUNK_PACK_SIZE);
					isExistedFirstPACK = true;
				} else { // Other case: not allowed
					std::cerr << "Now allowed case: RESET" << std::endl;
					total_pack = -1;
					start_receive();
					return;
				}
				
				std::cout << "expecting length of packs:" << total_pack << std::endl;
				
				int i = 0;
				if (isExistedFirstPACK) {
					i = 1;
				}

				for (i; i < total_pack; i++) {
					// std::cout << "Now receive " << i + 1 << "th PACK" << std::endl;
					int recvMsgSize;
					recvMsgSize = socket_.receive_from(buffer(recv_buffer_, BUFFER_MAX_LENGTH), remote_endpoint_);
					// std::cout << "recvMsgSize: " << recvMsgSize << std::endl;

					if (recvMsgSize == HEAD_PACK_SIZE) {
						std::cerr << "Transfer uncompeletely: RESET" << std::endl;
						// Free dynamic source and return to OS
						if (longbuf != NULL) {
							delete [] longbuf;
							longbuf = NULL;
						}
						total_pack = ((int * ) recv_buffer_)[0];
						start_receive();
						return;
					}

					if (recvMsgSize != CHUNK_PACK_SIZE) {
						std::cerr << "Received unexpected size pack: " << recvMsgSize << " : RESET" << std::endl;
						if (longbuf != NULL) {
							delete [] longbuf;
							longbuf = NULL;
						}
						total_pack = -1;
						start_receive();
						continue;
					}
					memcpy(&longbuf[i * CHUNK_PACK_SIZE], recv_buffer_, CHUNK_PACK_SIZE);
				}

				std::random_device rd;
				std::default_random_engine gen = std::default_random_engine(rd());
				std::uniform_int_distribution<int> dis(1,100000);

				// output to a jpg file
				std:: ofstream frameof;
				frameof.open("./image_"+to_string(dis(gen))+".jpg");
				for(int i = 0 ; i < CHUNK_PACK_SIZE * total_pack ; i++){
					frameof<<longbuf[i];
				}
				frameof.close();


				// connect to UE assigned UDP port
				boost::asio::ip::address_v4 ue_ipv4 = remote_endpoint_.address().to_v4();
				boost::asio::ip::udp::endpoint ue_udp_endpoint(ue_ipv4, ue_udp_port_);

				// send data to UE assigned UDP port
				std::string recv_buffer_rsp_ = "OK No Problem\n";
				socket_.async_send_to(boost::asio::buffer(recv_buffer_rsp_, recv_buffer_rsp_.size()), ue_udp_endpoint,
				    boost::bind(&udpserver::handle_send, this, message,
				    boost::asio::placeholders::error,
				    boost::asio::placeholders::bytes_transferred));

				// free dynamical source and return to OS and reset total_pack
				if (longbuf != NULL) {
					delete [] longbuf;
					longbuf = NULL;
				}
				total_pack = -1;

				start_receive();
			}
	    }
	}

	ip::udp::socket socket_;
	unsigned short ue_udp_port_;
	ip::udp::endpoint remote_endpoint_;
	char recv_buffer_[BUFFER_MAX_LENGTH];
	int total_pack;
//	boost::array<char,1024>recv_buffer_;

};

