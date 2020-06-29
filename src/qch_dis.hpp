#ifndef __QCH_DIS_HPP__
#define __QCH_DIS_HPP__
#include <cstdint>
#include <string>
#include <vector>

#include <qch_vm/qch_vm.hpp>

namespace qch_dis {
  class disassembler {
  public:
    std::vector<std::string> operator()(const std::vector<uint8_t> &program);
  private:
    qch_vm::machine m;
  };

}

#endif // __QCH_DIS_HPP__
