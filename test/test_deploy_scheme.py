"""
Tests Connection Pools
"""

__author__ = "@bg"

try:
    from _case import DatabaseTestCase
    from _websql_context import WebSQLSetup, WebSQLContextBase
except ImportError:  # pragma: no cover
    from ._case import DatabaseTestCase
    from ._websql_context import WebSQLSetup, WebSQLContextBase

from websql.cluster import deploy
import warnings


class TestDeployScheme(DatabaseTestCase):
    @classmethod
    def get_context(cls):
        return WebSQLContextBase(WebSQLSetup())

    def test_statement_iterator(self):
        statements = [b"CREATE TABLE IF NOT EXISTS `test` (",
                      b"`id` BIGINT NOT NULL AUTO_INCREMENT,",
                      b" PRIMARY KEY (`id`) ",
                      b")  ENGINE=InnoDB;",
                      b"-- COMMENT ---",
                      b"#COMMENT2",
                      b"DELIMITER $$",
                      b"DROP FUNCTION IF EXISTS `f1`$$",
                      b"CREATE FUNCTION `f1` () RETURNS INT DETERMINISTIC",
                      b"BEGIN",
                      b"RETURN 1;",
                      b"END$$",
                      b"DELIMITER ;",
                      b"SET A=1;SET B=2;"]

        statements = list(deploy._statement_iter(statements))
        self.assertEqual(b"CREATE TABLE IF NOT EXISTS `test` (\n"
                         b"`id` BIGINT NOT NULL AUTO_INCREMENT,\n"
                         b"PRIMARY KEY (`id`)\n"
                         b")  ENGINE=InnoDB", statements[0])

        self.assertEqual(b"DROP FUNCTION IF EXISTS `f1`", statements[1])
        self.assertEqual(b"\nCREATE FUNCTION `f1` () RETURNS INT DETERMINISTIC\n"
                         b"BEGIN\n"
                         b"RETURN 1;\n"
                         b"END", statements[2])

        self.assertEqual(b"SET A=1", statements[3])
        self.assertEqual(b"SET B=2", statements[4])

    def test_deploy_scheme_to_master(self):
        table = self.unique_name("table").encode("utf8")
        procedure = self.unique_name("procedure").encode("utf8")
        statements = [b"CREATE TABLE IF NOT EXISTS `" + table + b"` (",
                      b"`id` BIGINT NOT NULL AUTO_INCREMENT,",
                      b"PRIMARY KEY (`id`)",
                      b");",
                      b"DROP PROCEDURE IF EXISTS `" + procedure + b"`;",
                      b"DELIMITER $$",
                      b"CREATE PROCEDURE `" + procedure + b"` ()",
                      b"BEGIN\n",
                      b"SELECT 1;\n",
                      b"END$$",
                      b"DELIMITER ;"]

        connection_args = {"master": "%(host)s" % self._context.connect_kwargs}
        connection_args.update(self._context.connect_kwargs)
        warnings.filterwarnings('default')
        with warnings.catch_warnings(record=True):
            deploy.drop_scheme(connection_args, for_all=False)
            deploy.deploy_scheme(connection_args, statements, for_all=False, scheme_options=b"DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci")

        cursor = self._context.cursor()
        cursor.callproc(procedure)
        self.assertEqual([(1,)], cursor.fetchall())
        cursor.nextset()
        cursor.execute(b"INSERT INTO `" + table + b"` VALUES();")
        self.tables.append(table.decode("utf8"))
        self.procedures.append(procedure.decode("utf8"))

    def test_deploy_scheme_to_all(self):
        table = self.unique_name("table").encode("utf8")
        procedure = self.unique_name("procedure").encode("utf8")
        statements = [b"CREATE TABLE IF NOT EXISTS `" + table + b"` (",
                      b"`id` BIGINT NOT NULL AUTO_INCREMENT,",
                      b"PRIMARY KEY (`id`)",
                      b");",
                      b'SHOW TABLES;'
                      b"DROP PROCEDURE IF EXISTS `" + procedure + b"`;",
                      b"DELIMITER $$",
                      b"CREATE PROCEDURE `" + procedure + b"` ()",
                      b"BEGIN\n",
                      b"SELECT 1;\n",
                      b"END$$",
                      b"DELIMITER ;"]

        connection_args = {"master": "%(host)s,%(host)s" % self._context.connect_kwargs}
        connection_args.update(self._context.connect_kwargs)
        warnings.filterwarnings('default')
        with warnings.catch_warnings(record=True):
            deploy.drop_scheme(connection_args, for_all=True)
            deploy.deploy_scheme(connection_args, statements, for_all=True, scheme_options=b"DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci")

        cursor = self._context.cursor()
        cursor.callproc(procedure)
        self.assertEqual([(1,)], cursor.fetchall())
        cursor.nextset()
        cursor.execute(b"INSERT INTO `" + table + b"` VALUES();")
        self.tables.append(table.decode("utf8"))
        self.procedures.append(procedure.decode("utf8"))
