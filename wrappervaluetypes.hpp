#ifndef WRAPPERVALUETYPES_HPP_
#define WRAPPERVALUETYPES_HPP_

#include <string>
#include <limits>


using NumericValue = int;
using StringValue = std::string;


inline NumericValue noNumericMinimum()
{
  return std::numeric_limits<NumericValue>::min();
}


inline NumericValue noNumericMaximum()
{
  return std::numeric_limits<NumericValue>::max();
}


#endif /* WRAPPERVALUETYPES_HPP_ */
