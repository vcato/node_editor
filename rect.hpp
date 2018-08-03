struct Rect {
  Point2D start, end;

  bool contains(const Point2D &p)
  {
    return
      p.x >= start.x && p.x <= end.x &&
      p.y >= start.y && p.y <= end.y;
  }

  Point2D center() const
  {
    float x = (start.x + end.x)/2;
    float y = (start.y + end.y)/2;

    return Point2D(x,y);
  }
};
