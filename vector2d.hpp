struct Vector2D {
  float x,y;

  Vector2D()
  : x(0), y(0)
  {
  }

  Vector2D(float x_arg,float y_arg)
  : x(x_arg), y(y_arg)
  {
  }

  bool operator==(const Vector2D &arg) const
  {
    return x==arg.x && y==arg.y;
  }

  bool operator!=(const Vector2D &arg) const
  {
    return !operator==(arg);
  }
};
