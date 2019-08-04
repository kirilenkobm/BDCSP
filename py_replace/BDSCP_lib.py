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
        print("# Str_len: {}; actual: {}; str_num: {}".format(self.str_len,
                                                              self.act_col_num,
                                                              self.str_num))
        self.k = k
        self.answer = None
        self.template = tuple(0 for _ in range(self.str_num))
        self.__get_fmax_and_sizes()
        self.__make_pattern_to_id()
        self.__get_negatives()
        self.__get_naive_sup_and_inf()
        self.__get_not_intersect()
        self.grid = Grid(self.str_num, self.act_col_num, self.f_max, self.pattern_to_size,
                         self.position_to_patterns, self.pattern_id_to_positions)
        self.points_num = self.grid.points_num
        self.max_comb_len = self.grid.max_comb_len
        self.points_gen = self.grid.base_points_generator()

    def __get_fmax_and_sizes(self):
        """Get f_max and pattern sizes."""
        self.pattern_to_size = [0 for _ in range(len(self.id_to_pattern))]
        self.size_to_pattern = defaultdict(set)
        for id_, pat in enumerate(self.id_to_pattern):
            pat_size = sum(pat)
            self.pattern_to_size[id_] = pat_size
            self.size_to_pattern[pat_size].add(id_)
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

    def __comb_sum(self, comb):
        """Compute combination sum."""
        if not comb:
            # special case, return empty sum them
            return self.template
        patterns = [self.id_to_pattern[p] for p in comb]
        return tuple(sum((p[i] for p in patterns)) for i in range(self.str_num))

    def __get_naive_sup_and_inf(self):
        """Get sup and inf."""
        self.to_cover = self.act_col_num - self.k
        print("# With K = {} positions to cover: {}".format(self.k, self.to_cover))
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
    
    def __get_not_intersect(self):
        """Return a dict of non-intersecting patterns."""
        self.non_intersects = defaultdict(set)
        pat_len = len(self.id_to_pattern)
        for i in range(pat_len):
            i_pat = self.id_to_pattern[i]
            for j in range(i + 1, pat_len):
                j_pat = self.id_to_pattern[j]
                intersect = any(i_pat[k] == j_pat[k] == 1 for k in range(self.str_num))
                if intersect:
                    continue
                self.non_intersects[i].add(j)
        # remove complementary patterns
        for i in range(pat_len):
            self.non_intersects[i].discard(self.pat_neg[i])

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

    def __add_1_tails(self, chain):
        """Add patterns of size 1."""
        answer = []
        for pre_comb, _ in chain:
            pre_comb_sum = self.__comb_sum(pre_comb)
            zeros_at = [i for i, v in enumerate(pre_comb_sum) if v == 0]
            zeros_num = len(zeros_at)
            patterns_to_find = [tuple(0 if j != i else 1 for j in range(self.str_num))
                            for i in zeros_at]
            found_ids = tuple(self.pattern_to_id.get(pat) for pat in patterns_to_find)
            if any(found_ids[i] is None for i in range(zeros_num)):
                continue
            ans = pre_comb + found_ids
            answer.append(ans)
        return answer

    def __process_case_2(self, s_path):
        """Resolve case of ro = 1/N."""
        chain = [((x,), self.non_intersects[x]) for x in self.size_to_pattern[s_path[0]]]
        path_left = s_path[1:]
        left_len = len(path_left)
        for size in path_left:
            if size == 1:
                tailed = self.__add_1_tails(chain)
                return tailed
            size_fit = self.size_to_pattern[size]
            new_chain_ = []
            for path, n_inter in chain:
                inter_compat = size_fit.intersection(n_inter)
                for c_ in inter_compat:
                    added_c_ = path + (c_, )
                    added_inter = n_inter.intersection(self.non_intersects[c_])
                    new_chain_.append((added_c_, added_inter))
            if not new_chain_:
                return []
            chain = new_chain_
        answer = [c[0] for c in chain]
        return answer

    def __inv_comb(self, comb):
        """Return inverted comb."""
        return tuple(self.pat_neg[p] for p in comb)

    def _find_comb(self, s_path, point_class):
        """Find combinations for the size path and point class given."""
        if point_class == 1:
            return self.__process_case_1(s_path)
        elif point_class == 2:
            direct = self.__process_case_2(s_path)
            inv = [self.__inv_comb(c) for c in direct]
            return direct + inv
        return []

    def __make_combs_index(self):
        """Make combinations index."""
        self.comb_index = defaultdict(list)
        self.comb_to_id = {}
        self.comb_id_to_ro = {}
        self.pat_to_combs = defaultdict(set)
        self.ro_to_ind = defaultdict(list)
        for comb in self.combs:
            comb_len = len(comb)
            comb_sum = self.__comb_sum(comb)
            comb_ro = comb_sum[0] / comb_len
            cur_ind = len(self.comb_index[comb_len])
            self.comb_index[comb_len].append(comb)
            comb_id = (comb_len, cur_ind)
            for p_ in comb:
                self.pat_to_combs[p_].add(comb_id)
            self.ro_to_ind[comb_ro].append(comb_id)
            self.comb_to_id[comb] = comb_id
            self.comb_id_to_ro[comb_id] = comb_ro

    def __comb_compat(self):
        """Create comb compatibility matrix."""
        self.comb_id_compat_with = defaultdict(set)
        for i in range(self.base_combs_num):
            for j in range(self.base_combs_num):
                print(i, j)

    def solve(self):
        """Return True if reachable, False otherwise."""
        self.combs = []
        if self.answer is not None:
            return self.answer
        for s_path, _, point_class in self.points_gen:
            if not s_path:
                break
            combs = self._find_comb(s_path, point_class)
            if not combs:
                continue
            self.combs.extend(combs)
        self.base_combs_num = len(self.combs)
        self.__make_combs_index()
        # check if basepoints are enough to get an answer
        self.__comb_compat()
        return False

if __name__ == "__main__":
    pass
