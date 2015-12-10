#include "ghandler.hpp"
#include <iostream>
#include <glog/logging.h>
#include <string>
#include <chrono>
#include <thread>
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
      actions.push_back("get_my_info_req");

    } else {
      LOG(INFO) << "[error] join_machine_ans fail";
    }




  } else if ( type == "get_my_info_ans") {
    std::string rtn = payload[0];
    if (rtn == "ok") {
      LOG(INFO) << "[debug] 나의 정보 가져오기 성공";
    } else {
      LOG(INFO) << "[error] 나의 정보 가져오기 실패";
    }

    actions.push_back("set_line_bet_req");
    actions.push_back("200");
    actions.push_back(std::to_string(app::get().mid));

  } else if (type =="set_line_bet_ans") {
    std::string rtn = payload[0];
    if (rtn == "ok") {
      LOG(INFO) << "[debug] 라인벳 설정 성공";
    } else {
      LOG(INFO) << "[error] 라인벳 설정 실패";
    }

    actions.push_back("spin_req");
    actions.push_back("200");

  } else if (type == "spin_ans" ) {

    if (app::get().spin_cnt < app::get().max_spin_cnt) {
      std::cout << "[debug] 스핀 돌린 결과 받음" << app::get().spin_cnt << std::endl;
      actions.push_back("spin_req");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

    } else {
      std::cout << "[error] 스핀 돌린 결과 실패" <<  std::endl;
      actions.push_back("leave_machine_req");

    }
    app::get().spin_cnt++;

  } else if (type == "leave_machine_ans") {

    std::cout << "[debug] 슬롯에서 일어남" << std::endl;
    actions.push_back("leave_slot_server_req");

  } else if(type == "leave_slot_server_ans") {

    std::cout << "[debug] 슬롯서버 나감" << std::endl;
    app::get().is_done = true;
    app::get().is_next = true;
    app::get().wait4next.notify_one();

  } else {
    std::cout << "[error] 핸들링 프로토콜 존재하지 않음" << std::endl;
  }

 
  return actions;
}
