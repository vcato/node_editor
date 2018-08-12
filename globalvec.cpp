#include "globalvec.hpp"


Vector2D
  globalVec(const BodyLink &/*body_link*/,const Vector2D &local_vec)
{
  // Since we don't have rotations or scale yet, the global and local
  // vectors are the same.
  return local_vec;
}
