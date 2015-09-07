#include <iostream>
#include <string>
#include <cstring>
#include <boost/lexical_cast.hpp>

#define BUF_SIZE 1024

class packet {

  size_t header_length_;
  const std::size_t max_payload_length_;
  const char token_length_;

  size_t chunk_cnt_;

  const unsigned chunk_cnt_pos_;  
  unsigned payload_pos_;

  int packet_total_length_;

  size_t body_length_;

  char buffer_[BUF_SIZE];

  void update_header_length(size_t);
  void inc_chunk_cnt();
  void add_chunk2buffer(size_t size, std::string data);
  std::string buffer2string(char*, int);

  std::vector<std::string> chunk_container_;
public:

  enum { header_length = sizeof(size_t), chunk_cnt_length = sizeof(size_t) };

  packet();
  ~packet();

  
  char* get_buf() { return buffer_; }
  char* get_buf_body() { return buffer_ + header_length; }
  int get_packet_length() { return packet_total_length_; }
  bool decode_header();
  bool decode_body();
  size_t get_body_length() { return body_length_; }

  std::string get_payload_type();
  std::vector<std::string> get_payload();


  /*
    청크가 생길때마다 헤더길이 수정, 청크갯수, 청크마다 길이, 1바이트의 토큰
   */
  template <class __DATA__>
  bool add_chunk(__DATA__ data) {
    std::string c = boost::lexical_cast<std::string>(data);
    //std::string c = std::to_string(data);
    add_chunk2buffer(c.size(), c);
    return true;
  }

  void reset();

};
