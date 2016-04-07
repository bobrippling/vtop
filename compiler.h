#ifndef COMPILER_H
#define COMPILER_H

#ifdef __GNUC__
#  define attr_wur __attribute__((warn_unused_result))
#else
#  define attr_wur
#endif

#endif
