#ifndef _MYSQL_PYTHON__TRACE_H_
#define _MYSQL_PYTHON__TRACE_H_

#ifdef ENABLE_TRACE
#include <stdio.h>
#define TRACE(msg) fprintf(stderr, "%s:%d %s " msg "\n", __FILE__, __LINE__, __FUNCTION__)
#define TRACE1(msg, arg0) fprintf(stderr, "%s:%d %s " msg "\n", __FILE__, __LINE__, __FUNCTION__, arg0)
#define TRACE2(msg, arg0, arg1) fprintf(stderr, "%s:%d %s " msg "\n", __FILE__, __LINE__, __FUNCTION__, arg0, arg1)
#define TRACE3(msg, arg0, arg1, arg3) fprintf(stderr, "%s:%d %s " msg "\n", __FILE__, __LINE__, __FUNCTION__, arg0, arg1, arg3)
#else
#define TRACE(msg)
#define TRACE1(msg, arg0)
#define TRACE2(msg, arg0, arg1)
#define TRACE3(msg, arg0, arg1, arg3)
#endif

#endif //_MYSQL_PYTHON__TRACE_H_