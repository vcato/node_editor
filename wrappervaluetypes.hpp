#ifndef WRAPPERVALUETYPES_HPP_
#define WRAPPERVALUETYPES_HPP_

#include <string>
#include <limits>
#include "numericvalue.hpp"


using StringValue = std::string;


inline NumericValue noNumericMinimum()
{
  return noMinimumNumericValue();
}


inline NumericValue noNumericMaximum()
{
  return noMaximumNumericValue();
}


#endif /* WRAPPERVALUETYPES_HPP_ */
