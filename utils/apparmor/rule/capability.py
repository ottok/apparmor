# ----------------------------------------------------------------------
#    Copyright (C) 2013 Kshitij Gupta <kgupta8592@gmail.com>
#    Copyright (C) 2014 Christian Boltz <apparmor@cboltz.de>
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

import re

from apparmor.common import AppArmorBug
from apparmor.regex import RE_PROFILE_CAP
from apparmor.rule import BaseRule, BaseRuleset, logprof_value_or_all, parse_modifiers
from apparmor.translations import init_translation

_ = init_translation()


class CapabilityRule(BaseRule):
    """Class to handle and store a single capability rule"""

    # Nothing external should reference this class, all external users
    # should reference the class field CapabilityRule.ALL
    class __CapabilityAll:
        pass

    ALL = __CapabilityAll

    rule_name = 'capability'
    _match_re = RE_PROFILE_CAP

    def __init__(self, cap_list, audit=False, deny=False, allow_keyword=False,
                 comment='', log_event=None):

        super().__init__(audit=audit, deny=deny, allow_keyword=allow_keyword,
                         comment=comment, log_event=log_event)
        # Because we support having multiple caps in one rule,
        # initializer needs to accept a list of caps.
        self.all_caps = False
        if cap_list == self.ALL:
            self.all_caps = True
            self.capability = set()
        else:
            if isinstance(cap_list, str):
                self.capability = {cap_list}
            elif isinstance(cap_list, list) and cap_list:
                self.capability = set(cap_list)
            else:
                raise AppArmorBug('Passed unknown object to %s: %s' % (type(self).__name__, str(cap_list)))
            # make sure none of the cap_list arguments are blank, in
            # case we decide to return one cap per output line
            for cap in self.capability:
                if not cap.strip():
                    raise AppArmorBug('Passed empty capability to %s: %s' % (type(self).__name__, str(cap_list)))

    @classmethod
    def _create_instance(cls, raw_rule, matches):
        """parse raw_rule and return instance of this class"""

        audit, deny, allow_keyword, comment = parse_modifiers(matches)

        if matches.group('capability'):
            capability = matches.group('capability').strip()
            capability = re.split("[ \t]+", capability)
        else:
            capability = cls.ALL

        return cls(capability, audit=audit, deny=deny,
                   allow_keyword=allow_keyword,
                   comment=comment)

    def get_clean(self, depth=0):
        """return rule (in clean/default formatting)"""

        space = '  ' * depth
        if self.all_caps:
            return ('%s%scapability,%s' % (space, self.modifiers_str(), self.comment))
        else:
            caps = ' '.join(self.capability).strip()  # XXX return multiple lines, one for each capability, instead?
            if caps:
                return ('%s%scapability %s,%s' % (space, self.modifiers_str(), ' '.join(sorted(self.capability)), self.comment))
            else:
                raise AppArmorBug("Empty capability rule")

    def _is_covered_localvars(self, other_rule):
        """check if other_rule is covered by this rule object"""

        if not self._is_covered_list(self.capability, self.all_caps, other_rule.capability, other_rule.all_caps, 'capability'):
            return False

        # still here? -> then it is covered
        return True

    def _is_equal_localvars(self, rule_obj, strict):
        """compare if rule-specific variables are equal"""

        if (self.capability != rule_obj.capability
                or self.all_caps != rule_obj.all_caps):
            return False

        return True

    def severity(self, sev_db):
        if self.all_caps:
            severity = sev_db.rank_capability('__ALL__')
        else:
            severity = -1
            for cap in self.capability:
                sev = sev_db.rank_capability(cap)
                if isinstance(sev, int):  # type check avoids breakage caused by 'unknown'
                    severity = max(severity, sev)

        if severity == -1:
            severity = sev  # effectively 'unknown'

        return severity

    def _logprof_header_localvars(self):
        cap_txt = logprof_value_or_all(self.capability, self.all_caps)

        return _('Capability'), cap_txt


class CapabilityRuleset(BaseRuleset):
    """Class to handle and store a collection of capability rules"""

    def get_glob(self, path_or_rule):
        """Return the next possible glob. For capability rules, that's always "capability," (all capabilities)"""
        return 'capability,'
