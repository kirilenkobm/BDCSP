#!/usr/bin/env python3
"""Temporary python replacement for shared library.
To be implemented first, and rewritten in C later.
"""


class BDCSP_colver:
    """Temporary python replacement!"""
    def __init__(self, id_to_pattern, pat_id_to_pos, pos_to_pat,
                 all_pos, str_len, str_num, act_col_num, k):
        """Entry point."""
        self.id_to_pattern = id_to_pattern
        self.pattern_id_to_positions = pat_id_to_pos
        self.position_to_patterns = pos_to_pat
        self.all_positions = all_pos
        self.str_len = str_len
        self.str_num = str_num
        self.act_col_num = act_col_num
        self.k = k
        self.answer = None
        self.__make_pattern_to_id()
        self.__get_negatives()
        self.__get_naive_sup_and_inf()

    def __make_pattern_to_id(self):
        """Create pattern to ID dict."""
        self.pattern_to_id = {}
        for id_, pattern in enumerate(self.id_to_pattern):
            self.pattern_to_id[pattern] = id_

    def __get_negatives(self):
        """Get mutually negative patterns."""
        self.pat_neg = {}
        for id_, pat in enumerate(self.id_to_pattern):
            neg_pat = self._not_pat(pat)
            neg_pat_id = self.pattern_to_id[neg_pat]
            self.pat_neg[id_] = neg_pat_id
            self.pat_neg[neg_pat_id] = id_

    def _not_pat(self, pattern):
        """Return not-pattern."""
        return tuple(1 if pattern[i] == 0 else 0 for i in range(self.str_num))

    def __get_naive_sup_and_inf(self):
        """Get sup and inf."""
        self.to_cover = self.act_col_num - self.k
        self.exp_ave_ro = self.to_cover / self.act_col_num
        self.inf = 1 / self.str_num
        self.sup = (self.str_num - 1) / self.str_num
        print("# Inf: {0:.4f} average_RO: {1:.4f} Max: {2:.4f}".format(self.inf, self.exp_ave_ro, self.sup))
        # get rid of obvious cases
        if self.exp_ave_ro > self.sup:
            self.answer = False
        elif self.exp_ave_ro <= self.inf:
            self.answer = True

    def solve(self):
        """Return True if reachable, False otherwise."""
        if self.answer is not None:
            return self.answer
        return False

if __name__ == "__main__":
    pass
