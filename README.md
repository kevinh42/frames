Coordinate frames are ubiquitous in robotics but tricky to get right. For high level applications you can (and should) abstract away all the linear algebra so you can think in frames (time your right hand spends rotating around is time it could be typing instead!). But sometimes you really want to do linear algebra. Okay, then how do you make sure you didn't mess anything up?

A few options:
1. Be really sure.
2. Stick to a consistent notation (eg. [monogram notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation__basics.html)). This doesn't mean you can't make mistakes, but you can spot them more easily.
3. Let the compiler check for you!

This is a proof-of-concept in C++ implementing an algebra based on monogram notation with static coordinate frame checking with templates and concepts. Currently I only have positions and rotations implemented. I provide some examples of the algebra, with examples that (correctly) fail the static check commented out.

With my main implementation (`Position3` and `Rotation3`), the possibility of mixing base Eigen types with my derived ones seems like an accident waiting to happen. It also doesn't support `CwiseBinaryOp` which means we don't get the full power of Eigen.

I also started another implementation (`Position3Wrapper`) that takes a wrapper approach. This has the advantage that it's agnostic to the type it wraps, which means 1) it doesn't actually need to be an Eigen type and 2) `CwiseBinaryOp` is supported! An example of this is included but I haven't implemented the algebra yet. I'll probably continue with this route.

Disadvantages:
1. Clunky and verbose. Dimensional analysis seems like an analogous problem and libraries like [std::chrono](https://en.cppreference.com/w/cpp/chrono) and [mp-units](https://github.com/mpusz/units) do a good job at keeping things clean for the user, so there are probably some lessons there I could apply.
2. As with any toy code, I won't know how well this works in practice until I use it in a project.
