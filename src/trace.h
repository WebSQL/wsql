/*
WSQL
====
An asynchronous python interface to MySQL
---------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __WSQL_TRACE_H__
#define __WSQL_TRACE_H__

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

#endif // __WSQL_TRACE_H__