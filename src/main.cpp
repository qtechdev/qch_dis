#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>
#include <variant>

#include <qxdg/qxdg.hpp>
#include <qfio/qfio.hpp>

#include "qch_dis.hpp"

static const std::regex program_re(R"re((.*)(\.ch8)$)re");

std::vector<uint16_t> concat(const std::vector<uint8_t> &bytes) {
  std::vector<uint16_t> program;

  for (int i = 0; i < bytes.size(); i += 2) {
    uint16_t b = (bytes[i] << 8) | bytes[i + 1];
    program.push_back(b);
  }

  return program;
};

int main(int argc, const char *argv[]) {
  xdg::base base_dirs = xdg::get_base_directories();
  auto program_files = xdg::search_data_dirs(base_dirs, "qch_dis", program_re);

  int index = 0;
  while ((index < 1) || (index > program_files.size())) {
    std::cout << "Choose program!\n";
    for (int i = 0; i < program_files.size(); i++) {
      std::cout << (i + 1) << ") " << program_files[i] << "\n";
    }

    std::cin >> index;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  std::string program_path = program_files[index - 1];
  std::string asm_path = std::regex_replace(
    program_path, program_re, "$1.qch_asm"
  );
  auto program_bytes = fio::readb(program_path);

  if (!program_bytes) {
    std::cerr << "could not read file " << program_path << "\n";
    return -1;
  }

  qch_dis::disassembler dis;
  std::vector<std::string> lines;
  try {
    lines = dis(*program_bytes);
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << "\n";
    return -1;
  }

  fio::write(asm_path, "", true);

  for (const auto &s : lines) {
    fio::write(asm_path, s, false);
    fio::write(asm_path, "\n", false);
  }

  return 0;
}
