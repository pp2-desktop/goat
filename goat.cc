#include <iostream>
#include <string>
#include <cstdlib>
#include <glog/logging.h>
//#include <boost/asio.hpp>
#include "gconnector.hpp"
#include "app.hpp"

int main(int argc, char** argv) {
  
  /*
  for(int i=0; i<argc; i++) {
    if(i!=0) {
      std::string str(argv[i]);
      std::cout << str << std::endl;
    }
  }
  */
  system("rm -rf ./log");
  const int dir_err = system("mkdir -p ./log");
  if (-1 == dir_err) {
      printf("Error creating directory!n");
      exit(1);
    }

  if(argc<3) {
    std::cout << "[error] argv가 없거나 적음" << std::endl;
    std::cout << "[help] 주소 자리번호" << std::endl;
    return 1;
  }

  // tmp
  std::string _ip(argv[1]);

  int slot_seat_num = std::stoi(std::string(argv[2]).c_str());
  app::get().mid = slot_seat_num;
  std::cout << "[debug] 유저 슬롯자리: " << app::get().mid << std::endl;
  

  //INFO, WARNING, ERROR, FATAL
  google::InitGoogleLogging("log");


  FLAGS_log_dir = "./log";
  FLAGS_alsologtostderr = 1;
  int num_cookies = 10;
  LOG(INFO) << "Found " << num_cookies << " cookies";

  fLS::FLAGS_log_dir = "c:/Documents/logs";
  
  boost::asio::io_service io_service;
  tcp::resolver resolver(io_service);

  auto endpoint_iterator = resolver.resolve({ _ip.c_str(), "10102" });
 
  auto is_connect = false;

  gconnector c(io_service, endpoint_iterator, [&is_connect](bool rtn) {

      is_connect = rtn;
      std::cout << "is_connect:" << is_connect << std::endl;

    });

  std::thread t([&io_service](){ 
      io_service.run(); 
    });


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
    msg.add_chunk("kyunghun");
    msg.add_chunk("1234");
    c.write(msg);
    std::cout << "패킷 보냄" << std::endl;
    getchar();
  }

  c.close();
  t.join();
  
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
