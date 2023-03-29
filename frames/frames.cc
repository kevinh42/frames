#include "frames/frames.h"
#include <cxxabi.h>
#include <iostream>
#include <memory>
#include <string>
std::string demangled(std::string const &sym) {
  std::unique_ptr<char, void (*)(void *)> name{
      abi::__cxa_demangle(sym.c_str(), nullptr, nullptr, nullptr), std::free};
  return {name.get()};
}

int main() {

  struct Frame_A : Frame<Frame_A> {};
  struct Frame_B : Frame<Frame_B> {};
  struct Frame_C : Frame<Frame_C> {};

  Position3d</*Reference=*/Frame_A, /*Target=*/Frame_B, /*Expressed=*/Frame_A>
      p_AB_A;
  p_AB_A << 1, 1, 1;
  Position3d</*Reference=*/Frame_A, /*Target=*/Frame_C, /*Expressed=*/Frame_A>
      p_AC_A;
  p_AC_A << -1.5, 1.5, 1.5;
  Position3d</*Reference=*/Frame_B, /*Target=*/Frame_A, /*Expressed=*/Frame_A>
      p_BA_A;
  p_BA_A << -1, -1, -1;
  Position3d</*Reference=*/Frame_B, /*Target=*/Frame_C, /*Expressed=*/Frame_A>
      p_BC_A;
  p_BC_A << 0.5, 0.5, 0.5;
  Position3d</*Reference=*/Frame_B, /*Target=*/Frame_C, /*Expressed=*/Frame_B>
      p_BC_B;
  p_BC_B << std::sqrt(0.75), 0, 0;

  // Additive inverse of position swaps reference and target and sign of values
  std::cout << (-p_AB_A).transpose() << std::endl;

  // Positions in the same frame add when left target and right reference match
  std::cout << (p_AB_A + p_BC_A).transpose() << std::endl;
  std::cout << demangled(typeid(decltype(p_AB_A + p_BC_A)).name()) << std::endl;
  std::cout << (p_AB_A + -p_AB_A + p_AB_A + p_BC_A).transpose() << std::endl;

  // Wrapper version supports CwiseBinaryOp
  Position3Wrapper<Eigen::Vector3d, Frame_A, Frame_B, Frame_A> p_AB_A_wrapper;
  Position3Wrapper<Eigen::Vector3d, Frame_B, Frame_C, Frame_A> p_BC_A_wrapper;
  std::cout << demangled(
                   typeid(decltype(p_AB_A_wrapper + p_BC_A_wrapper)).name())
            << std::endl;

  // Expressed frame must match to add positions
  // std::cout<<(p_AB_A + p_BC_B).transpose() << std::endl;

  // Left target and right reference must match to add positions
  // std::cout<<(p_AB_A + p_AC_A).transpose() << std::endl;

  Rotation3d<Frame_A, Frame_B> R_AB =
      Eigen::Quaterniond::FromTwoVectors(p_BC_B, p_BC_A);
  std::cout << p_BC_A.transpose() << std::endl;
  std::cout << p_BC_B.transpose() << std::endl;
  std::cout << R_AB << std::endl;

  // Inverse of rotation switches signs
  static_assert(std::is_same<decltype(R_AB.inverse())::reference::frame,
                             decltype(R_AB)::target::frame>::value,
                "Inverse should switch reference and target.");

  // Rotations multiply when left target and right reference match
  std::cout << (R_AB * R_AB.inverse()) << std::endl;
  static_assert(std::is_same<decltype(R_AB * R_AB.inverse())::target::frame,
                             decltype(R_AB)::reference::frame>::value,
                "Target frame should be the same as reference.");

  // Multiplying rotation by position changes expressed frame
  std::cout << (R_AB * p_BC_B).transpose() << std::endl;
  static_assert(std::is_same<decltype(R_AB * p_BC_B)::expressed::frame,
                             decltype(R_AB)::reference::frame>::value,
                "Expressed frame should change to the rotation's reference.");

  return 0;
}