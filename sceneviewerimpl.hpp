namespace scene_viewer {

inline ViewportRect bodyBox(const ViewportPoint &p1)
{
  float x2 = p1.x + 10;
  float y2 = p1.y + 10;
  ViewportPoint p2{x2,y2};
  return ViewportRect{p1,p2};
}


template <typename Function>
void forEachSceneBodyRect(const Scene &scene,const Function &f)
{
  auto callFWithBodyBox =
    [&](const Scene::Body &body,const Point2D &global_position){
      ViewportPoint p{global_position.x,global_position.y};
      f(body,bodyBox(p));
    };

  forEachSceneBodyPosition(scene,callFWithBodyBox);
}

}
