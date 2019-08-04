#!/usr/bin/env python3
"""Temporary python replacement for shared library.
To be implemented first, and rewritten in C later.
"""
from collections import defaultdict
from py_replace.Grid_lib import Grid


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
        self.__get_fmax_and_sizes()
        self.__make_pattern_to_id()
        self.__get_negatives()
        self.__get_naive_sup_and_inf()
        self.grid = Grid(self.str_num, self.act_col_num, self.f_max, self.pattern_to_size,
                         self.position_to_patterns, self.pattern_id_to_positions)
        self.points_num = self.grid.points_num
        self.max_comb_len = self.grid.max_comb_len
        self.points_gen = self.grid.base_points_generator()

    def __get_fmax_and_sizes(self):
        """Get f_max and pattern sizes."""
        self.pattern_to_size = [0 for _ in range(len(self.id_to_pattern))]
        self.size_to_pattern = defaultdict(list)
        for id_, pat in enumerate(self.id_to_pattern):
            pat_size = sum(pat)
            self.pattern_to_size[id_] = pat_size
            self.size_to_pattern[pat_size].append(id_)
        self.f_max = sorted(self.pattern_to_size, reverse=True)

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
        print("# Inf: {0:.4f} average_RO: {1:.4f} Max: {2:.4f}".format(self.inf,
                                                                       self.exp_ave_ro,
                                                                       self.sup))
        # get rid of obvious cases
        if self.exp_ave_ro > self.sup:
            self.answer = False
        elif self.exp_ave_ro <= self.inf:
            self.answer = True
    
    def __process_case_0(self, s_path):
        """Process the case of ro > 1/2."""
        answer = []
        # NO NEED TO COMPUTE THIS
        return answer

    def __process_case_1(self, s_path):
        """Resolve case 1."""
        answer = []
        f_size = s_path[0]
        patterns_of_f_size = self.size_to_pattern[f_size]
        # get negative
        neg_patterns_to_f = (self.pat_neg[x] for x in patterns_of_f_size)
        pos_of_f_patterns = (self.pattern_id_to_positions[x] for x in patterns_of_f_size)
        for pair_pos in zip(patterns_of_f_size, neg_patterns_to_f, pos_of_f_patterns):
            if len(pair_pos[2]) == 1:
                continue
            answer.append(pair_pos[:2])
        return answer

    def __process_case_2(self, s_path):
        """Resolve case of ro = 1/N."""
        answer = []

        return answer

    def _find_comb(self, s_path, point_class):
        """Find combinations for the size path and point class given."""
        if point_class == 0:
            return self.__process_case_0(s_path)
        elif point_class == 1:
            return self.__process_case_1(s_path)
        elif point_class == 2:
            return self.__process_case_2(s_path)
        return []

    def solve(self):
        """Return True if reachable, False otherwise."""
        self.point_to_size = {}
        self.comb_index = {}
        if self.answer is not None:
            return self.answer
        for s_path, point_num, point_class in self.points_gen:
            self.point_to_size[point_num] = len(s_path)
            if not s_path:
                break
            # print(s_path, point_num, point_class)
            combs = self._find_comb(s_path, point_class)
            # print(combs)
        return False

if __name__ == "__main__":
    pass
