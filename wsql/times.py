"""
WSQL
====
An asynchronous DB API v2.0 compatible interface to MySQL
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
"""

from time import localtime
from datetime import date, datetime, time, timedelta

__all__ = ["Date", "Time", "TimeDelta", "Timestamp",
           "DateFromTicks", "TimeFromTicks", "TimestampFromTicks"]

# These are required for DB-API (PEP-249)
Date = date
Time = time
TimeDelta = timedelta
Timestamp = datetime


def date_from_ticks(ticks):
    """
    Convert UNIX ticks into a date instance.
    >>> date_from_ticks(1172466380)
    datetime.date(2007, 2, 26)
    >>> date_from_ticks(0)
    datetime.date(1970, 1, 1)
    >>> date_from_ticks(2 ** 31 - 1)
    datetime.date(2038, 1, 19)

    This is a standard DB-API constructor.
    """
    return date(*localtime(ticks)[:3])


DateFromTicks = date_from_ticks


def time_from_ticks(ticks):
    """
    Convert UNIX ticks into a time instance.

    >>> time_from_ticks(1172466380)
    datetime.time(8, 6, 20)
    >>> time_from_ticks(0)
    datetime.time(3, 0)
    >>> time_from_ticks(2 ** 31 - 1)
    datetime.time(6, 14, 7)
    """
    return time(*localtime(ticks)[3:6])


TimeFromTicks = time_from_ticks


def timestamp_from_ticks(ticks):
    """
    Convert UNIX ticks into a datetime instance.

    >>> timestamp_from_ticks(1172466380)
    datetime.datetime(2007, 2, 26, 8, 6, 20)
    >>> timestamp_from_ticks(0)
    datetime.datetime(1970, 1, 1, 3, 0)
    >>> timestamp_from_ticks(2 ** 31 - 1)
    datetime.datetime(2038, 1, 19, 6, 14, 7)
    """
    return datetime(*localtime(ticks)[:6])


TimestampFromTicks = timestamp_from_ticks
