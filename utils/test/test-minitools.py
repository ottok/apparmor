# ----------------------------------------------------------------------
#    Copyright (C) 2013 Kshitij Gupta <kgupta8592@gmail.com>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of version 2 of the GNU General Public
#    License as published by the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
# ----------------------------------------------------------------------
import os
import shutil
import subprocess
import unittest

import apparmor.aa as apparmor
from common_test import AATest, read_file, setup_aa, setup_all_loops

python_interpreter = 'python3'


class MinitoolsTest(AATest):

    def AASetup(self):
        self.createTmpdir()

        # copy the local profiles to the test directory
        # Should be the set of cleanprofile
        self.profile_dir = self.tmpdir + '/profiles'
        shutil.copytree('../../profiles/apparmor.d/', self.profile_dir, symlinks=True)

        apparmor.profile_dir = self.profile_dir

        # Path for the program
        self.test_path = '/usr/sbin/winbindd'
        # Path for the target file containing profile
        self.local_profilename = self.profile_dir + '/usr.sbin.winbindd'

    def test_audit(self):
        # Set test profile to audit mode and check if it was correctly set
        subprocess.check_output(
            '{} ./../aa-audit --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            'audit',
            'Audit flag could not be set in profile ' + self.local_profilename)

        # Remove audit mode from test profile and check if it was correctly removed
        subprocess.check_output(
            '{} ./../aa-audit --no-reload -d {} -r {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path), shell=True)

        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            None,
            'Audit flag could not be removed in profile ' + self.local_profilename)

    def test_complain(self):
        # Set test profile to complain mode and check if it was correctly set
        subprocess.check_output(
            '{} ./../aa-complain --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

        # "manually" create a force-complain symlink (will be deleted by aa-enforce later)
        force_complain_dir = self.profile_dir + '/force-complain'
        if not os.path.isdir(force_complain_dir):
            os.mkdir(force_complain_dir)
        os.symlink(
            self.local_profilename,
            '{}/{}'.format(force_complain_dir, os.path.basename(self.local_profilename)))

        self.assertEqual(
            os.path.islink('{}/{}'.format(force_complain_dir, os.path.basename(self.local_profilename))),
            True,
            'Failed to create a symlink for {} in force-complain'.format(self.local_profilename))
        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            'complain',
            'Complain flag could not be set in profile ' + self.local_profilename)

        # Set test profile to enforce mode and check if it was correctly set
        subprocess.check_output(
            '{} ./../aa-enforce --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

        self.assertEqual(
            os.path.islink('{}/{}'.format(force_complain_dir, os.path.basename(self.local_profilename))),
            False,
            'Failed to remove symlink for {} from force-complain'.format(self.local_profilename))
        self.assertEqual(
            os.path.islink('{}/disable/{}'.format(self.profile_dir, os.path.basename(self.local_profilename))),
            False,
            'Failed to remove symlink for {} from disable'.format(self.local_profilename))
        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            None,
            'Complain flag could not be removed in profile ' + self.local_profilename)

        # Set audit flag and then complain flag in a profile
        subprocess.check_output(
            '{} ./../aa-audit --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)
        subprocess.check_output(
            '{} ./../aa-complain --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)
        # "manually" create a force-complain symlink (will be deleted by aa-enforce later)
        os.symlink(
            self.local_profilename,
            '{}/{}'.format(force_complain_dir, os.path.basename(self.local_profilename)))

        self.assertEqual(
            os.path.islink('{}/{}'.format(force_complain_dir, os.path.basename(self.local_profilename))),
            True,
            'Failed to create a symlink for {} in force-complain'.format(self.local_profilename))
        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            'audit, complain',
            'Complain flag could not be set in profile ' + self.local_profilename)

        # Remove complain flag first i.e. set to enforce mode
        subprocess.check_output(
            '{} ./../aa-enforce --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

        self.assertEqual(
            os.path.islink('{}/{}'.format(force_complain_dir, os.path.basename(self.local_profilename))),
            False,
            'Failed to remove symlink for {} from force-complain'.format(self.local_profilename))
        self.assertEqual(
            os.path.islink('{}/disable/{}'.format(self.profile_dir, os.path.basename(self.local_profilename))),
            False,
            'Failed to remove symlink for {} from disable'.format(self.local_profilename))
        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            'audit',
            'Complain flag could not be removed in profile ' + self.local_profilename)

        # Remove audit flag
        subprocess.check_output(
            '{} ./../aa-audit --no-reload -d {} -r {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

    def test_enforce(self):
        # Set test profile to enforce mode and check if it was correctly set
        subprocess.check_output(
            '{} ./../aa-enforce --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

        self.assertEqual(
            os.path.islink('{}/force-complain/{}'.format(self.profile_dir, os.path.basename(self.local_profilename))),
            False,
            'Failed to remove symlink for {} from force-complain'.format(self.local_profilename))
        self.assertEqual(
            os.path.islink('{}/disable/{}'.format(self.profile_dir, os.path.basename(self.local_profilename))),
            False,
            'Failed to remove symlink for {} from disable'.format(self.local_profilename))
        self.assertEqual(
            apparmor.get_profile_flags(self.local_profilename, self.test_path),
            None,
            'Complain flag could not be removed in profile {}'.format(self.local_profilename))

    def test_disable(self):
        # Disable the test profile and check if it was correctly disabled
        subprocess.check_output(
            '{} ./../aa-disable --no-reload -d {} {} --configdir ./'.format(
                python_interpreter, self.profile_dir, self.test_path),
            shell=True)

        self.assertEqual(
            os.path.islink('{}/disable/{}'.format(self.profile_dir, os.path.basename(self.local_profilename))),
            True,
            'Failed to create a symlink for {} in disable'.format(self.local_profilename))

    def test_autodep(self):
        pass

    @unittest.skipIf(apparmor.check_for_apparmor() is None, "Securityfs not mounted or doesn't have the apparmor directory.")
    def test_unconfined(self):
        output = subprocess.check_output(
            python_interpreter + ' ./../aa-unconfined --configdir ./', shell=True)

        output_force = subprocess.check_output(
            python_interpreter + ' ./../aa-unconfined --paranoid --configdir ./', shell=True)

        self.assertIsNot(output, '', 'Failed to run aa-unconfined')

        self.assertIsNot(output_force, '', 'Failed to run aa-unconfined in paranoid mode')

    def test_cleanprof(self):
        input_file = 'cleanprof_test.in'
        output_file = 'cleanprof_test.out'
        # We position the local testfile
        shutil.copy('./' + input_file, self.profile_dir)
        # Our silly test program whose profile we wish to clean
        cleanprof_test = '/usr/bin/a/simple/cleanprof/test/profile'

        subprocess.check_output(
            '{} ./../aa-cleanprof  --no-reload -d {} -s {} --configdir ./'.format(
                python_interpreter, self.profile_dir, cleanprof_test),
            shell=True)

        # Strip off the first line (#modified line)
        subprocess.check_output('sed -i 1d {}/{}'.format(self.profile_dir, input_file), shell=True)

        exp_content = read_file('./' + output_file)
        real_content = read_file('{}/{}'.format(self.profile_dir, input_file))
        self.maxDiff = None
        self.assertEqual(exp_content, real_content, 'Failed to cleanup profile properly')


setup_aa(apparmor)
setup_all_loops(__name__)
if __name__ == '__main__':
    unittest.main(verbosity=1)
