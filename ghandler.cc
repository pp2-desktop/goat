#include "ghandler.hpp"
#include <iostream>
#include <glog/logging.h>
#include <string>
#include "app.hpp"

ghandler::ghandler() {

}

ghandler::~ghandler() {
 
}

std::vector<std::string> ghandler::handle_packet(std::string type, std::vector<std::string> payload) {

  std::vector<std::string> actions;

  if (type == "join_slot_server_ans") {

    std::string rtn = payload[0];
    std::cout << rtn << std::endl;

    if (rtn == "ok") {
      LOG(INFO) << "[debug] 슬롯서버 입장 성공";
      actions.push_back("join_machine_req");
      actions.push_back(std::to_string(app::get().mid));
    } else {
      LOG(FATAL) << "[error] joinslot_server_ans fail";
    }

  } else if (type =="join_machine_ans") {
    std::string rtn = payload[0];
    if (rtn == "ok") {
      LOG(INFO) << "[debug] 슬롯머신 앉기 성공";
      actions.push_back("set_line_bet_req");
      actions.push_back("200");
      //actions.push_back(std::to_string(app::get().mid));
    } else {
      LOG(INFO) << "[error] join_machine_ans fail";
    }


  } else if (type =="set_line_bet_ans") {

    actions.push_back("spin_req");
    actions.push_back("100");

  } else if (type == "spin_ans" ) {
    
    std::cout << "[debug] 스핀 돌린 결과 받음 " << app::get().spin_cnt << std::endl;

    if (app::get().spin_cnt < app::get().max_spin_cnt) {
      actions.push_back("spin_req");

      if(app::get().spin_cnt > 5) {
	actions.push_back("2500");
      } else {
	actions.push_back("5000");
      }

    } else {
      actions.push_back("leave_machine_req");
    }
    app::get().spin_cnt++;

  } else if (type == "leave_machine_ans") {

    std::cout << "[debug] 슬롯에서 일어남" << std::endl;
    actions.push_back("leave_slot_server_req");

  } else if(type == "leave_slot_server_ans") {

    std::cout << "[debug] 슬롯서버 나감" << std::endl;
    app::get().is_done = true;

  } else {
    std::cout << "[error] 핸들링 프로토콜 존재하지 않음" << std::endl;
  }

 
  
  return actions;
}
