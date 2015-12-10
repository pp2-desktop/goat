#include <thread>
#include <chrono>
#include "gconnector.hpp"

gconnector::gconnector(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator, std::function<void(bool)> cb)
  : io_service_(io_service),
    socket_(io_service)
{
  //cb(false);
  

  do_connect(endpoint_iterator);

}

gconnector::~gconnector() {

}

void gconnector::do_connect(tcp::resolver::iterator endpoint_iterator) {

  boost::asio::async_connect(socket_, endpoint_iterator,
			     [this](boost::system::error_code ec, tcp::resolver::iterator)
			     {

			       if (!ec) {
				 std::cout << "[succ] 접속 성공" << std::endl;
				 do_read_header();
			       }
			       else {
				 std::cout << "[error] 접속 실패" << std::endl;
				
			       }
			     });
  
}

void gconnector::write(packet pkt) {

  boost::asio::async_write(socket_,
			   boost::asio::buffer(pkt.get_buf(),
					       pkt.get_packet_length()),
			   [this](boost::system::error_code ec, std::size_t)
			   {
			     if (!ec) {
			       std::cout << "[debug] write 성공" << std::endl;
			       //std::cout << "@@ 패킷 보냄 @@" << std::endl;
			       /*
				 write_msgs_.pop_front();
				 if (!write_msgs_.empty())
				 {
				 do_write();
				 }
			       */
			     }
			     else {
			       std::cout << "[error] write 실패" << std::endl;
			       socket_.close();
			     }
			   });


}

void gconnector::close() {

}

void gconnector::do_read_header() {
  //std::cout << "[debug] 패킷 오기 대기중" << std::endl;
  boost::asio::async_read(socket_,
			  boost::asio::buffer(read_packet_.get_buf(), packet::header_length),
			  [this] (boost::system::error_code ec, std::size_t)
			  {
			    if (!ec && read_packet_.decode_header()) {
			      //if (!ec) {
			      //std::cout << "@@ 헤더 받음 @@" << std::endl;
			      do_read_body();
			    }
			    else {
			      //std::cout << "@@ 접속 끈킴 @@" << std::endl;
			      socket_.close();
			    }
			  });
}




/*
void gconnector::do_write2(const packet& msg) {

  io_service_.post(
		   [this, msg]()
		   {
		     bool write_in_progress = !write_msgs_.empty();
		     write_msgs_.push_back(msg);

		     if (!write_in_progress)
		       {
			 do_write();
		       }

		   });
}
*/

void gconnector::do_write2(const packet& msg) {

  bool write_in_progress = !write_msgs_.empty();
  write_msgs_.push_back(msg);

  if (!write_in_progress)
    {
      do_write();
    }
}


void gconnector::do_write()
{

  boost::asio::async_write(socket_,
			   boost::asio::buffer(write_msgs_.front().get_buf(),
					       write_msgs_.front().get_packet_length()),
			   [this](boost::system::error_code ec, std::size_t)
			   {
			     if (!ec)
			       {
				 //int len = write_msgs_.front().get_packet_length();
				 //std::cout << "len: " << len << std::endl;
				 //std::cout << "size: " << write_msgs_.size() << std::endl;
				 write_msgs_.pop_front();

				 if (!write_msgs_.empty()) {
				   do_write();
				 }
			       }
			     else
			       {
				 std::cout << "[error]err: " << ec << std::endl;
				 socket_.close();
			       }
			   });
}






void gconnector::do_read_body() {

  //char* buf[256] = {0,};

  boost::asio::async_read(socket_,
			  boost::asio::buffer(read_packet_.get_buf_body(), read_packet_.get_body_length()), [this] (boost::system::error_code ec, std::size_t) {

			    //std::cout << "@@ 바디 받음 @@" << std::endl;
			    if (!ec) {

			      auto r = read_packet_.decode_body();


				if(r) {

				  //std::string type = read_packet_.get_payload_type();
				  //std::vector<std::string> v = read_packet_.get_payload();


				  std::vector<std::string> actions = handler_.handle_packet(read_packet_.get_payload_type(), read_packet_.get_payload());
				  
				  read_packet_.reset();
				  do_read_header();
				  


				  if(actions.size() > 0 ) {

				    // 체크 포인트 #2
				    // msg가 do_write보내기도 전에 사라질 가능성?
				    packet msg; 
				    for(unsigned i=0; i<actions.size(); i++) {
				      msg.add_chunk(actions[i]);
				    }

				    this->do_write2(msg);

				  } else {
				    socket_.close();
				  }
				  
				  
				} else {
				  socket_.close();
				}
				/*
				  size_t chunk_cnt = 0;
				  std::memcpy(&chunk_cnt, read_packet_.get_buf_body(), sizeof(size_t));
				  std::cout << "[debug] 청크 갯수: " << chunk_cnt << std::endl;
				*/
				//std::cout.write(read_msg_.body(), read_msg_.body_length());
				//std::cout << "\n";
				//do_read_header();
			      }
			    else
			      {
				socket_.close();
			      }
			  });
}
