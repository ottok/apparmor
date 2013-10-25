#!/usr/bin/env python
# ------------------------------------------------------------------
#
#   Copyright (C) 2013 Canonical Ltd.
#   Author: Steve Beattie <steve@nxnw.org>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of version 2 of the GNU General Public
#   License published by the Free Software Foundation.
#
# ------------------------------------------------------------------

# TODO
# - finish adding suppressions for valgrind false positives

from argparse import ArgumentParser  # requires python 2.7 or newer
import os
import tempfile
import unittest
import testlib

DEFAULT_TESTDIR = "./simple_tests/vars"
VALGRIND_ERROR_CODE = 151
VALGRIND_ARGS = ['--leak-check=full', '--error-exitcode=%d' % (VALGRIND_ERROR_CODE)]

VALGRIND_SUPPRESSIONS = '''
{
    valgrind-add_search_dir-obsessive-overreads
    Memcheck:Addr4
    fun:_Z*add_search_dir*
    fun:_Z*process_arg*
    fun:main
}

{
    valgrind-yylex-obsessive-overreads
    Memcheck:Addr4
    fun:_Z?yylex?
    fun:_Z*yyparse*
    fun:_Z*process_profile*
    fun:main
}

{
    valgrind-serialize_profile-obsessive-overreads
    Memcheck:Addr4
    fun:_Z*sd_serialize_profile*
    fun:_Z*sd_serialize_codomain*
    fun:_Z*load_codomain*
    fun:_Z*__load_flattened_hat*
    ...
    fun:twalk
    fun:_Z*load_flattened_hats*
    fun:_Z*sd_serialize_codomain*
    fun:_Z*load_codomain*
    fun:_Z*__load_policy*
}'''


class AAParserValgrindTests(testlib.AATestTemplate):
    def setUp(self):
        # REPORT ALL THE OUTPUT
        self.maxDiff = None

    def _runtest(self, testname, config):
        parser_args = ['-Q', '-I', config.testdir]
        failure_rc = [VALGRIND_ERROR_CODE, testlib.TIMEOUT_ERROR_CODE]
        command = ['valgrind']
        command.extend(VALGRIND_ARGS)
        command.append(config.parser)
        command.extend(parser_args)
        command.append(testname)
        rc, output = self.run_cmd(command, timeout=120)
        self.assertNotIn(rc, failure_rc,
                    "valgrind returned error code %d, gave the following output\n%s\ncommand run: %s" % (rc, output, " ".join(command)))


def find_testcases(testdir):
    '''dig testcases out of passed directory'''

    for (fdir, direntries, files) in os.walk(testdir):
        for f in files:
            if f.endswith(".sd"):
                yield os.path.join(fdir, f)


def create_suppressions():
    '''generate valgrind suppressions file'''

    handle, name = tempfile.mkstemp(suffix='.suppressions', prefix='aa-parser-valgrind')
    os.close(handle)
    handle = open(name,"w+")
    handle.write(VALGRIND_SUPPRESSIONS)
    handle.close()
    return name

def main():
    rc = 0
    p = ArgumentParser()
    p.add_argument('-p', '--parser', default=testlib.DEFAULT_PARSER, action="store", dest='parser')
    p.add_argument('-v', '--verbose', action="store_true", dest="verbose")
    p.add_argument('-s', '--skip-suppressions', action="store_true", dest="skip_suppressions",
                   help="Don't use valgrind suppressions to skip false positives")
    p.add_argument('--dump-suppressions', action="store_true", dest="dump_suppressions",
                   help="Dump valgrind suppressions to stdout")
    p.add_argument('testdir', action="store", default=DEFAULT_TESTDIR, nargs='?', metavar='dir',
                   help="run tests on alternate directory [default = %(default)s]")
    config = p.parse_args()

    if config.dump_suppressions:
        print(VALGRIND_SUPPRESSIONS)
        return rc

    verbosity = 1
    if config.verbose:
        verbosity = 2

    if not config.skip_suppressions:
        suppression_file = create_suppressions()
        VALGRIND_ARGS.append('--suppressions=%s' % (suppression_file))

    for f in find_testcases(config.testdir):
        def stub_test(self, testname=f):
            self._runtest(testname, config)
        stub_test.__doc__ = "test %s" % (f)
        setattr(AAParserValgrindTests, 'test_%s' % (f), stub_test)
    test_suite = unittest.TestSuite()
    test_suite.addTest(unittest.TestLoader().loadTestsFromTestCase(AAParserValgrindTests))

    try:
        result = unittest.TextTestRunner(verbosity=verbosity).run(test_suite)
        if not result.wasSuccessful():
            rc = 1
    except:
        rc = 1
    finally:
        os.remove(suppression_file)

    return rc

if __name__ == "__main__":
    rc = main()
    exit(rc)
