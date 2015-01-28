WebSQL
======
.. image:: https://travis-ci.org/WebSQL/websql.svg?branch=master
    :target: https://travis-ci.org/WebSQL/websql

.. image:: https://coveralls.io/repos/WebSQL/websql/badge.png?branch=master
    :target: https://coveralls.io/r/WebSQL/websql?branch=master


WebSQL is an asynchronous interface to the popular MySQL_ database server for Python based on webscalesql.
The design goals are:
    - Compatibility with Python3 asyncio package
    - Compliance with Python database API version 2.0 [PEP-0249]_
    - Thread-safety
    - Thread-friendliness (threads will not block each other)

MySQL-5.5 and newer and Python-3.4 and newer are currently supported.
WebSQL is `Free Software`_.
.. _MySQL: http://www.mysql.com/
.. _`Free Software`: http://www.gnu.org/
.. [PEP-0249] http://www.python.org/peps/pep-0249.html
