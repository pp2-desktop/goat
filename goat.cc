#include <iostream>
#include <string>
#include <cstdlib>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "gconnector.hpp"
#include "app.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vector>
#include <thread>
#include <chrono> 

int main(int argc, char** argv) {

  boost::property_tree::ptree props;
  boost::property_tree::read_json("options.json", props);

  
  system("rm -rf ./log");
  const int dir_err = system("mkdir -p ./log");
  if (-1 == dir_err) {
    printf("Error creating directory!n");
    exit(1);
  }  
  google::InitGoogleLogging("log");

  FLAGS_log_dir = "./log";
  FLAGS_alsologtostderr = 1;

  //int num_cookies = 10;
  //LOG(INFO) << "Found " << num_cookies << " cookies";

  

  /*
  auto is_connect = false;

  gconnector c(io_service, endpoint_iterator, [&is_connect](bool rtn) {

      is_connect = rtn;
      std::cout << "is_connect:" << is_connect << std::endl;

    });
  */


  std::this_thread::sleep_for (std::chrono::seconds(1));

  std::mutex m;
  std::unique_lock<std::mutex> lk(m);

  boost::property_tree::ptree &childs = props.get_child("playing_pattern");
  for(boost::property_tree::ptree::value_type &vt : childs ) {

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({props.get<std::string>("host"), props.get<std::string>("port")});

    app::get().is_next = false;

    app::get().uid = vt.second.get<std::string>("uid");
    std::cout << "uid: " << app::get().uid << std::endl;

    app::get().mid = vt.second.get<int>("mid");
    std::cout << "mid: " << app::get().mid  << std::endl;

    app::get().max_spin_cnt = vt.second.get<int>("spin_cnt");
    app::get().spin_cnt = 0;

    auto is_connect = false;

    gconnector c(io_service, endpoint_iterator, [&is_connect](bool rtn) {
	is_connect = rtn;
	std::cout << "is_connect:" << is_connect << std::endl;
      });

    //io_service.run(); 
    
    std::thread t([&io_service](){ 
	io_service.run(); 
      });
    

    std::this_thread::sleep_for (std::chrono::seconds(2));

    std::string policy_request("<security-check-pass/>");
    policy_request += '\0';
    boost::asio::write(c.get_socket(), boost::asio::buffer(policy_request), boost::asio::transfer_all());

    std::this_thread::sleep_for (std::chrono::seconds(1));

    std::cout << "@@111111@@" << std::endl;
    packet msg;
    msg.add_chunk("join_slot_server_req");
    msg.add_chunk(app::get().uid);
    msg.add_chunk("1234");
    c.write(msg);

    std::cout << "@@222222@@" << std::endl;

    app::get().wait4next.wait(lk, [=](){
	std::cout << "@@wait@@" << std::endl;
	return app::get().is_next;
      });
    std::cout << "@@wait 풀림@@" << std::endl;
    
    std::cout << "@@join 풀림@@" << std::endl;
    t.join();
    //io_service.stop();
  }



  /*
  std::cout << "잠시 멈춤" << std::endl;
  getchar();


  std::string policy_request("<security-check-pass/>");
  policy_request += '\0';
  boost::asio::write(c.get_socket(), boost::asio::buffer(policy_request), boost::asio::transfer_all());

  std::cout << "시큐리티 체크 패킷 보냄" << std::endl;
  getchar();
  // 액션
  while(1) {
    packet msg; //gi skey
    msg.add_chunk("join_slot_server_req");
    msg.add_chunk("tester2");
    msg.add_chunk("1234");
    c.write(msg);

    packet msg2;
    msg2.add_chunk("get_my_info_req");
    c.write(msg2);



    std::cout << "패킷 보냄" << std::endl;
    getchar();
  }
  c.close();
*/
  
  return 0;
}



/*
void unit_test_packet_1() {
  std::cout << "a" << std::endl;

  packet p;

  p.add_chunk(15);
  p.add_chunk<std::string>("abcd");
  
  size_t header_len = -1;
  size_t chunk_cnt = -1;
  size_t chunk_length = -1;

  char delim = 'a';
  char* buf_ptr = p.buffer_;

  
  std::memcpy(&header_len, buf_ptr, sizeof(size_t));
  std::cout << "header_len: " << header_len << std::endl;

  std::memcpy(&chunk_cnt, buf_ptr+sizeof(size_t), sizeof(size_t));
  std::cout << "chunk_cnt: " << chunk_cnt << std::endl;

  std::memcpy(&chunk_length, buf_ptr+(sizeof(size_t)*2), sizeof(size_t));
  std::cout << "chunk_length: " << chunk_length << std::endl;


  char* v = new char[chunk_length];
  std::memcpy(v, buf_ptr+(sizeof(size_t)*3), chunk_length-1);
  std::string s(v);
  std::cout << s << std::endl;

  std::memcpy(&delim, buf_ptr+(sizeof(size_t)*3)+ chunk_length-1, 1);
  std::cout << "delim: " << delim << std::endl;
  
}
*/
