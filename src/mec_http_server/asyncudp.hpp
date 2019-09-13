#include <iostream>
#include <sstream>
#include <random>
#include <memory>
#include <cmath>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include "opencv2/opencv.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

// For xilinux Urtra 96 FPGA board 
#ifdef DPU
#include <glog/logging.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <xilinx/ai/classification.hpp>
#include <xilinx/ai/nnpp/classification.hpp>
#include <xilinx/ai/demo.hpp>
#include "process_result_for_boost_server.hpp"
#endif
// END



using namespace cv;
using namespace std;
using namespace boost::asio;


#define MAX_PACK 100
#define CHUNK_PACK_SIZE 8000
#define BUFFER_MAX_LENGTH 65540
#define HEAD_PACK_SIZE 4

class udpserver
{
    public:
	udpserver(io_service & service, unsigned short ue_udp_port, int timeout):socket_(service,ip::udp::endpoint(ip::udp::v4(), 0)), ue_udp_port_(ue_udp_port), timeout_(timeout)
    {
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
    	uuid_string = boost::lexical_cast<std::string>(uuid);
		total_pack = -1;
		current_port = socket_.local_endpoint().port();
		index = 1;
		isTimeout = new bool;
		*(isTimeout) = false;
		namedWindow("recv_"+uuid_string, WINDOW_AUTOSIZE);
		start_receive();
    }

	void setTimer(boost::asio::deadline_timer &stop_timer, io_service& service){
		stop_timer_ptr_ = &stop_timer;
		io_service_ptr_ = &service;
		(*isTimeout) = true;
		(*stop_timer_ptr_).expires_from_now(boost::posix_time::seconds(timeout_));
		(*stop_timer_ptr_).async_wait(
			[this](const boost::system::error_code &ec)
			{
				if(*isTimeout){
					cout << "Yes, We Stoooooooooooped"<< endl;
					(*io_service_ptr_).stop();
				} else {
					cout << "Reset timer"<< endl;
				}
			});
		// (*isTimeout) = false;
	}

	// void resetTimer(){
	// 	(*isTimeout) = false;
	// 	(*stop_timer_ptr_).expires_from_now(boost::posix_time::seconds(timeout_));
	// 	(*stop_timer_ptr_).async_wait(
	// 		[this](const boost::system::error_code &ec)
	// 		{
	// 			if((*isTimeout)){
	// 				cout << "Yes, We Stoooooooooooped"<< endl;
	// 				(*io_service_ptr_).stop();
	// 			} else {
	// 				cout << "Reset timer"<< endl;
	// 			}
	// 		});
	// 	(*isTimeout) = false;
	// }


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

				if (bytes_transferred != HEAD_PACK_SIZE && bytes_transferred != CHUNK_PACK_SIZE) {
					std::cerr << "Error: please check HEAD_PACK_SIZE and CHUNK_PACK_SIZE" << std::endl;
					std::cout << "Drop 1 packet, size = " << bytes_transferred << std::endl;
					total_pack = -1;
					start_receive();
					return;
				}

				char *longbuf = NULL;
				bool isExistedFirstPACK = false;

				if (total_pack == -1 && bytes_transferred == HEAD_PACK_SIZE) { // Normal case
					std::cout << "Normal case" << std::endl;
					// total_pack = ((int * ) recv_buffer_)[0];
					total_length = ((int * ) recv_buffer_)[0];
					total_pack = ceil((double) total_length / CHUNK_PACK_SIZE);
					// longbuf = new char[CHUNK_PACK_SIZE * total_pack];
					longbuf = new char[total_length];
				} else if (total_pack != -1 && bytes_transferred == CHUNK_PACK_SIZE) { // Drop and renew case and put the first PACK into buffer
					std::cout << "Drop and renew PACK case" << std::endl;
					// longbuf = new char[CHUNK_PACK_SIZE * total_pack];
					longbuf = new char[total_length];
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
						delete [] longbuf;
						longbuf = NULL;
						// total_pack = ((int * ) recv_buffer_)[0];
						total_length = ((int * ) recv_buffer_)[0];
						total_pack = ceil((double) total_length / CHUNK_PACK_SIZE);
						start_receive();
						return;
					}

					if (recvMsgSize != CHUNK_PACK_SIZE) {
						std::cerr << "Received unexpected size pack: " << recvMsgSize << " : RESET" << std::endl;
						delete [] longbuf;
						longbuf = NULL;
						total_pack = -1;
						start_receive();
						return;
					}

					if(i != total_pack - 1){
						memcpy(&longbuf[i * CHUNK_PACK_SIZE], recv_buffer_, CHUNK_PACK_SIZE);
					} else {
						memcpy(&longbuf[i * CHUNK_PACK_SIZE], recv_buffer_, total_length % CHUNK_PACK_SIZE);
					}
					
				}


				// output to a jpg file
				// std:: ofstream frameof;
				// std:: ostringstream index_with_zero;
				// index_with_zero << setw(6) << setfill('0') << index;
				// cout << "index = " << index_with_zero.str() << std::endl;
				// frameof.open("./image_"+index_with_zero.str()+".jpg");
				// cout << "Start to combine and total_length = " << total_length << endl;
				// for(int i = 0 ; i < total_length /*CHUNK_PACK_SIZE * total_pack*/ ; i++){
				// 	frameof<<longbuf[i];
				// }
				// frameof.close();
				// index += 1;

				Mat rawData = Mat(1, total_length, CV_8UC1, longbuf);
				Mat frame = imdecode(rawData, IMREAD_COLOR);
				if (frame.size().width == 0) {
					cerr << "decode failure!" << endl;
					total_pack = -1;
					longbuf = NULL;
					total_pack = -1;
					start_receive();
					return;
				}
				std::string frame_result_text;
				try
				{
#ifdef DPU
					frame_result_text = xilinx::ai::main_for_jpeg_demo_for_boost_server(
					frame,
					[] {
						return xilinx::ai::Classification::create(xilinx::ai::CLASSIFICATION_RESNET_50);
					},
					process_result);

					// imshow("recv_"+uuid_string, frame_result);
					// waitKey(1);
					// usleep(1);
#elif CAFF
					// TODO
#else
					imshow("recv_"+uuid_string, frame);
					waitKey(1);
					usleep(1);
#endif

				}
				catch(Exception& e)
				{
					cerr << "show failure!" << endl;
					delete [] longbuf;
					longbuf = NULL;
					total_pack = -1;
					start_receive();
					return;
				}
				

				// connect to UE assigned UDP port
				boost::asio::ip::address_v4 ue_ipv4 = remote_endpoint_.address().to_v4();
				boost::asio::ip::udp::endpoint ue_udp_endpoint(ue_ipv4, ue_udp_port_);

				// send data to UE assigned UDP port
				std::string recv_buffer_rsp_;
				ptree root;
				std::ostringstream rspos;
#ifdef DPU
				root.put<std::string>("rawdata", frame_result_text);
				write_json(rspos, root);
				recv_buffer_rsp_ = rspos.str();
#elif CAFF
				// TODO
				root.put<std::string>("rawdata", "OK No Problem\n");				
				write_json(rspos, root);
				recv_buffer_rsp_ = rspos.str();
#else
				root.put<std::string>("rawdata", "OK No Problem\n");				
				write_json(rspos, root);
				recv_buffer_rsp_ = rspos.str();
#endif
				socket_.async_send_to(boost::asio::buffer(recv_buffer_rsp_, recv_buffer_rsp_.size()), ue_udp_endpoint,
				    boost::bind(&udpserver::handle_send, this, message,
				    boost::asio::placeholders::error,
				    boost::asio::placeholders::bytes_transferred));

				// free dynamic source and return to OS and reset total_pack
				delete [] longbuf;
				longbuf = NULL;
				total_pack = -1;
				
				std::cout << "Success to receive a jpg image" << std::endl;
				// reset stop timer
				// resetTimer(); // TODO: reason for bad performance, and now set 3600 timeout temporarily 

				start_receive();
			}
	    }
	}

	ip::udp::socket socket_;
	unsigned short ue_udp_port_;
	ip::udp::endpoint remote_endpoint_;
	char recv_buffer_[BUFFER_MAX_LENGTH];
	int total_pack;
	int total_length;
	int index;
	boost::asio::deadline_timer *stop_timer_ptr_;
	io_service *io_service_ptr_;
	int timeout_;
	bool *isTimeout;
	string uuid_string;
//	boost::array<char,1024>recv_buffer_;

};

