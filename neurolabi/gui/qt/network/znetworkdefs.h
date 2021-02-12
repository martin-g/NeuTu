#ifndef ZNETWORKDEFS_H
#define ZNETWORKDEFS_H

namespace znetwork {

enum class EOperation {
  NONE, READ, READ_PARTIAL, READ_HEAD, IS_READABLE, HAS_HEAD, HAS_OPTIONS, POST
};

}

#endif // ZNETWORKDEFS_H
