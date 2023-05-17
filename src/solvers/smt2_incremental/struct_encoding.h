// Author: Diffblue Ltd.

#ifndef CBMC_STRUCT_ENCODINGT_H
#define CBMC_STRUCT_ENCODINGT_H

#include <util/std_expr.h>

#include <memory>

class namespacet;
class boolbv_widtht;

/// Encodes struct types/values into non-struct expressions/types.
class struct_encodingt final
{
public:
  explicit struct_encodingt(const namespacet &ns);
  struct_encodingt(const struct_encodingt &other);
  ~struct_encodingt();

  typet encode(typet type) const;
  exprt encode(exprt expr) const;

private:
  std::unique_ptr<boolbv_widtht> boolbv_width;
};

#endif // CBMC_STRUCT_ENCODINGT_H
