#ifndef FRAMES_H
#define FRAMES_H

#include "Eigen/Core"
#include "Eigen/Geometry"
#include <cmath>

// Frame
template <typename T> concept FrameConcept = requires { typename T::frame; };

template <typename T> struct Frame { using frame = T; };

// Position4
template <typename Scalar, FrameConcept Reference, FrameConcept Target,
          FrameConcept Expressed>
class Position3; // forward declare to use in concept

template <typename T> concept Position3Concept = T::is_position3 &&requires {
  typename T::reference;
  typename T::target;
  typename T::expressed;
};

template <typename Scalar, FrameConcept Reference, FrameConcept Target,
          FrameConcept Expressed>
class Position3 : public Eigen::Matrix<Scalar, 3, 1> {
public:
  using Base = Eigen::Matrix<Scalar, 3, 1>;
  using reference = Reference;
  using target = Target;
  using expressed = Expressed;
  static constexpr bool is_position3 = true;

  Position3(void) : Base() {}

  // This constructor allows you to construct Position3 from Eigen expressions
  template <typename OtherDerived>
  Position3(const Eigen::MatrixBase<OtherDerived> &other) : Base(other) {}

  // This method allows you to assign Eigen expressions to Position3
  template <typename OtherDerived>
  Position3 &operator=(const Eigen::MatrixBase<OtherDerived> &other) {
    this->Base::operator=(other);
    return *this;
  }

  template <Position3Concept OtherPosition>
  Position3<Scalar, Reference, typename OtherPosition::target, expressed>
  operator+(const OtherPosition &other) {
    static_assert(std::is_same<typename expressed::frame,
                               typename OtherPosition::expressed::frame>::value,
                  "Expressed frames don't match.");
    static_assert(std::is_same<typename target::frame,
                               typename OtherPosition::reference::frame>::value,
                  "Left target frame doesn't match right reference frame.");
    return this->Base::operator+(other);
  }

  Position3<Scalar, target, reference, expressed> operator-() {
    return this->Base::operator-();
  }
};

template <FrameConcept Reference, FrameConcept Target, FrameConcept Expressed>
using Position3d = Position3<double, Reference, Target, Expressed>;

// Alternate wrapper approach
template <typename Mat, FrameConcept Reference, FrameConcept Target,
          FrameConcept Expressed>
class Position3Wrapper {
public:
  using Base = Mat;
  using reference = Reference;
  using target = Target;
  using expressed = Expressed;
  static constexpr bool is_position3 = true;

  Position3Wrapper(Mat &mat) : mat_(mat) {}
  Position3Wrapper(void) { mat_ = Base(); }

  template <typename OtherDerived>
  Position3Wrapper(const Eigen::MatrixBase<OtherDerived> &other) {
    mat_ = Base(other);
  }

  template <typename OtherDerived>
  Position3Wrapper &operator=(const Eigen::MatrixBase<OtherDerived> &other) {
    mat_ = other;
    return *this;
  }

  // Establish friendship so operators can access mat_
  template <typename OtherMat, FrameConcept OtherReference,
            FrameConcept OtherTarget, FrameConcept OtherExpressed>
  friend class Position3Wrapper;

  template <Position3Concept OtherPosition>
  auto operator+(const OtherPosition &other) {
    static_assert(std::is_same<typename expressed::frame,
                               typename OtherPosition::expressed::frame>::value,
                  "Expressed frames don't match.");
    static_assert(std::is_same<typename target::frame,
                               typename OtherPosition::reference::frame>::value,
                  "Left target frame doesn't match right reference frame.");
    return Position3Wrapper<decltype(mat_ + other.mat_), reference,
                            typename OtherPosition::target, expressed>(
        mat_ + other.mat_);
  }

private:
  Mat mat_;
};

// Rotation3
template <typename T> concept Rotation3Concept = T::is_rotation3 &&requires {
  typename T::reference;
  typename T::target;
};
template <typename Scalar, FrameConcept Reference, FrameConcept Target>
class Rotation3 : public Eigen::Quaternion<Scalar> {
public:
  using Base = Eigen::Quaternion<Scalar>;
  using reference = Reference;
  using target = Target;
  static constexpr bool is_rotation3 = true;

  Rotation3(void) : Base() {}

  template <typename OtherDerived>
  Rotation3(const Eigen::QuaternionBase<OtherDerived> &other) : Base(other) {}

  template <typename OtherDerived>
  Rotation3 &operator=(const Eigen::QuaternionBase<OtherDerived> &other) {
    this->Base::operator=(other);
    return *this;
  }

  template <Rotation3Concept OtherRotation>
  Rotation3<Scalar, Reference, typename OtherRotation::target>
  operator*(const OtherRotation &other) {
    static_assert(std::is_same<typename target::frame,
                               typename OtherRotation::reference::frame>::value,
                  "Left target frame doesn't match right reference frame.");
    return this->Base::operator*(other);
  }

  template <Position3Concept OtherPosition>
  Position3<Scalar, typename OtherPosition::reference,
            typename OtherPosition::target, Reference>
  operator*(const OtherPosition &other) {
    static_assert(std::is_same<typename target::frame,
                               typename OtherPosition::expressed::frame>::value,
                  "Left target frame doesn't match right expressed frame.");
    return this->Base::operator*(other);
  }

  Rotation3<Scalar, target, reference> inverse() const {
    return this->Base::inverse();
  }
};

template <FrameConcept Reference, FrameConcept Target>
using Rotation3d = Rotation3<double, Reference, Target>;

// template <typename Scalar, FrameConcept Reference, FrameConcept Target>
// class Transform3 : public Eigen::Transform<Scalar,3>

#endif