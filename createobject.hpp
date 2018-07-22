#ifndef CREATEOBJECT_HPP
#define CREATEOBJECT_HPP


template <typename T,typename U>
void createObject(T& object,U &&value)
{
  new (&object) T(value);
}


#endif /* CREATEOBJECT_HPP */
