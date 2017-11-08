#ifndef COMPILER_H
#define COMPILER_H

#ifdef __GNUC__
#  define attr_wur __attribute__((warn_unused_result))
#  define attr_printf(a, b) __attribute__((format(printf, a, b)))
#else
#  define attr_wur
#  define attr_printf(a, b)
#endif

#endif
