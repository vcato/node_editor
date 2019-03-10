#ifndef EUCLIDEANPAIR_HPP_
#define EUCLIDEANPAIR_HPP_


template <typename Component>
struct EuclideanPair {
  Component x{},y{};

  EuclideanPair(Component x_arg,Component y_arg)
  : x(x_arg), y(y_arg)
  {
  }
};


#endif /* EUCLIDEANPAIR_HPP_ */
