#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "packet.hpp"
#include <functional>
#include "ghandler.hpp"

using boost::asio::ip::tcp;

class gconnector { 


  boost::asio::io_service& io_service_;
  tcp::socket socket_;


  void do_connect(tcp::resolver::iterator endpoint_iterator);
  void do_read_header();
  void do_read_body();

  packet read_packet_;
  packet write_packet_;

  ghandler handler_;
 public:

  gconnector(boost::asio::io_service& io_service, tcp::resolver::iterator endpoint_iterator, std::function<void(bool)> cb);
  ~gconnector();


  void write(packet& pkt);
  void close();

  tcp::socket& get_socket() { return socket_; }
};
