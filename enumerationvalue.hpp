struct EnumerationValue
{
  std::string name;

  bool operator==(const EnumerationValue &arg) const
  {
    return name==arg.name;
  }
};
