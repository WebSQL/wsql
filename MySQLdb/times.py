"""
times module
------------

WARNING: The doctests only pass if you're in the right timezone and
daylight savings time setting. XXX

This module provides some help functions for dealing with MySQL data.
Most of these you will not have to use directly.

Uses Python datetime module to handle time-releated columns."""

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
    
        This is a standard DB-API constructor.
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
    
        This is a standard DB-API constructor.
    """
    return datetime(*localtime(ticks)[:6])

TimestampFromTicks = timestamp_from_ticks


if __name__ == "__main__":
    import doctest
    doctest.testmod()
