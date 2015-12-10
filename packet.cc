#include "packet.hpp"
#include <utility>
packet::packet() 
  : header_length_(sizeof(std::size_t)*2),
    max_payload_length_(1024*sizeof(char)-sizeof(std::size_t)),
    token_length_(sizeof(char)),
    chunk_cnt_pos_(sizeof(std::size_t)) {
  chunk_cnt_ = 0;
  body_length_ = 0;
  std::memset(buffer_, 0, BUF_SIZE);
  std::memset(buffer_, 0, BUF_SIZE);

  // header length | body chunk cnt  | payload
  //       8       |        8        |    ?
  payload_pos_   = sizeof(std::size_t) * 2;
  packet_total_length_ = sizeof(size_t) * 2;
}

/*
packet::packet(const packet& packet) 
  : header_length_(sizeof(std::size_t)*2),
    max_payload_length_(1024*sizeof(char)-sizeof(std::size_t)),
    token_length_(sizeof(char)),
    chunk_cnt_pos_(sizeof(std::size_t))
{
  this->chunk_cnt_ = packet.chunk_cnt_;
  this->body_length_ = packet.body_length_;
  std::memset(buffer_, 0, BUF_SIZE);
  std::memset(buffer_, 0, BUF_SIZE);

  this->payload_pos_ = packet.payload_pos_;
  this->packet_total_length_ = packet.packet_total_length_;
  this->chunk_container_ = packet.chunk_container_;
}
*/

packet::~packet() {

}

void packet::update_header_length(size_t len) {

  // each chunk length
  header_length_ += len;
  std::memcpy(buffer_ , &header_length_, sizeof(std::size_t));

}

void packet::inc_chunk_cnt() {
  ++chunk_cnt_;
  std::memcpy(&(buffer_[chunk_cnt_pos_]), &chunk_cnt_, sizeof(std::size_t));
};

void packet::add_chunk2buffer(size_t size, std::string data) {

  size += sizeof(token_length_);

  size_t chunk_len = sizeof(size_t) + size;

  inc_chunk_cnt();
  update_header_length(chunk_len);

    
  // chunk len + data copy
  std::memcpy(&(buffer_[payload_pos_]), &size, sizeof(size_t));

  payload_pos_ += sizeof(size_t);

  std::memcpy(&(buffer_[payload_pos_]), data.c_str(), data.size());

  payload_pos_ += data.size();

  buffer_[payload_pos_] = 'x';
    
  payload_pos_ += sizeof(char);

  packet_total_length_ += sizeof(size_t) + size;
}

bool packet::decode_header() {
  std::memcpy(&body_length_, buffer_, header_length);
  body_length_ = body_length_ - header_length;
  //std::cout << "[debug] 헤더 길이: " << body_length_ << std::endl;
  return true;
}

std::mutex m;
void packet::print_qsize(int size) {
  m.lock();
  std::cout << "size: " << size << std::endl;
  m.unlock();
}

std::string packet::buffer2string(char* buf, int len)
{
    std::string ret(buf, len);
    return ret;
    //return std::move(ret);
}

bool packet::decode_body() {

  size_t chunk_cnt = 0;
  std::memcpy(&chunk_cnt, get_buf_body(), sizeof(size_t));
  if(chunk_cnt <= 0) {
    return false;
  }

  //printf("%p\n", &chunk_container_);
  std::cout << "chunk cnt " << chunk_cnt << std::endl;
  std::cout << "start vec size: " << chunk_container_.size() << std::endl;


  int pos = chunk_cnt_length;


  for(unsigned i=0; i<chunk_cnt; i++) {

    size_t chunk_len = 0;
    std::memcpy(&chunk_len, get_buf_body()+pos, sizeof(size_t));
    if(chunk_len <= 0) {
      return false;
    }

    chunk_len = chunk_len  - sizeof(char);


    pos = pos + sizeof(chunk_len);

    char buf[BUF_SIZE] = {0,};

    std::memcpy(buf, get_buf_body()+pos, chunk_len);

    std::string payload(buf, chunk_len);
    //std::string payload = buffer2string(buf, chunk_len);


    chunk_container_.push_back(payload);
    pos = pos +  chunk_len + sizeof(char);
  }

  //std::cout << "end vec size: " << chunk_container_.size() << std::endl;




  return true;
}

std::string packet::get_payload_type() {
  return chunk_container_[0];
}

std::vector<std::string> packet::get_payload() {

  std::vector<std::string> payload_container;
  if(chunk_container_.size() > 1) {
    for(unsigned i=1; i<chunk_container_.size(); i++) {
      payload_container.push_back(chunk_container_[i]);
    }
  }

  return payload_container;
}


void packet::reset() {
  std::cout << "before clean v size: " << chunk_container_.size() << std::endl;
  chunk_container_.clear();
  std::cout << "after clean v size: " << chunk_container_.size() << std::endl;
}

