#include <string>
#include <vector>

class ghandler {

public:

  ghandler();
  ~ghandler();

  std::vector<std::string> handle_packet(std::string, std::vector<std::string>);
  
};
