#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include <qfio/qfio.hpp>

#include <qch_vm/qch_vm.hpp>
#include <qch_vm/spec.hpp>
#include "qch_dis.hpp"

std::vector<std::string> qch_dis::disassembler::operator()(
  const std::vector<uint8_t> &program
) {
  std::vector<std::string> lines;

  qch_vm::load_program(m, program);

  qch::instruction inst = qch::unknown_instruction;
  for (int i = 0; i < m.program_size; i += 2) {
    inst = fetch_instruction(m);
    m.pc += 2;

    std::stringstream ss;
    ss << inst.name;

    switch (inst.args) {
      case qch::args_config::R:
        ss << " " << qch::reg_token << std::hex << +qch::get_r(inst);
        break;
      case qch::args_config::RR: {
          auto [x, y] = qch::get_rr(inst);
          ss << " " << qch::reg_token << std::hex << +x << " ";
          ss << qch::reg_token << std::hex << +y;
        }
        break;
      case qch::args_config::RB: {
          auto [x, b] = qch::get_rb(inst);
          ss << " " << qch::reg_token << std::hex << +x << " ";
          ss << "0x" << std::hex << +b;
        }
        break;
      case qch::args_config::RRN: {
          auto [x, y, n] = qch::get_rrn(inst);
          ss << " " << qch::reg_token << std::hex << +x << " ";
          ss << qch::reg_token << std::hex << +y << " ";
          ss << "0x" << std::hex << +n;
        }
        break;
      case qch::args_config::A:
        ss << " 0x" << std::hex << +qch::get_a(inst);
        break;
      case qch::args_config::D:
        ss << " " << qch::data_token << std::hex << inst.data;
        break;
      case qch::args_config::Z:
      default: break;
    }

    lines.push_back(ss.str());
  }

  return lines;
}
