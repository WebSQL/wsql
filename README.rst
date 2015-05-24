WSQL
====
.. image:: https://travis-ci.org/WebSQL/wsql.svg?branch=master
    :target: https://travis-ci.org/WebSQL/wsql

.. image:: https://coveralls.io/repos/WebSQL/wsql/badge.png?branch=master
    :target: https://coveralls.io/r/WebSQL/wsql?branch=master


WSQL is an asynchronous python connector to the popular `MySQL`_ database server based on `WebScaleSQL`_.

The design goals are:
 - Compatibility with Python3 asyncio package
 - Compliance with Python database API version 2.0 `[PEP-0249]`_
 - Thread-safety
 - Thread-friendliness (threads will not block each other)
 - Support mysql-clusters like: `Percona-XtraDB-Cluster`_, `GaleraCluster`_
    

.. code:: python

    import wsql.cluster.connect
    # master:master(active:active)
    connection = wsql.cluster.connect("master=localhost:3305#2,localhost:3306#2", database="test")
    # master:slave(active:passive)
    connection = wsql.cluster.connect("master=localhost;slave=localhost:3307#4", database="test")
    # read-only
    connection = wsql.cluster.connect("slave=localhost:3307#4;database=test;")


MySQL-5.5 and newer and Python-3.4 and newer are currently supported.

WSQL is `Free Software`_.

`Documentation`_.

.. _`MySQL`: http://www.mysql.com/
.. _`Free Software`: http://www.gnu.org/
.. _`WebScaleSQL`: http://webscalesql.org/
.. _`[PEP-0249]`: http://www.python.org/peps/pep-0249.html
.. _`Percona-XtraDB-Cluster`: http://www.percona.com/software/percona-xtradb-cluster
.. _`GaleraCluster`: http://galeracluster.com/products/
.. _`Documentation`: http://websql-websql.rhcloud.com/
