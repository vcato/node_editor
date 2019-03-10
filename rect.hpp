#include "point2d.hpp"
#include "size2d.hpp"


template <typename Tag>
struct TaggedRect {
  TaggedPoint2D<Tag> start, end;
  using Point2D = TaggedPoint2D<Tag>;
  using Size2D = TaggedSize2D<Tag>;

  bool contains(const Point2D &p)
  {
    return
      p.x >= start.x && p.x <= end.x &&
      p.y >= start.y && p.y <= end.y;
  }

  bool contains(const TaggedRect &r) const
  {
    if (r.start.x<start.x) return false;
    if (r.start.y<start.y) return false;
    if (r.end.x>end.x) return false;
    if (r.end.y>end.y) return false;
    return true;
  }

  Point2D center() const
  {
    float x = (start.x + end.x)/2;
    float y = (start.y + end.y)/2;
    return Point2D(x,y);
  }

  Size2D size() const
  {
    float x = end.x - start.x;
    float y = end.y - start.y;
    return Size2D(x,y);
  }
};


template <typename Tag>
TaggedRect<Tag>
  operator+(const TaggedRect<Tag> &rect,const TaggedVector2D<Tag> &offset)
{
  return {rect.start + offset, rect.end + offset};
}
