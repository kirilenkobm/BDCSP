#!/usr/bin/env python3
"""Temporary python replacement for shared library.
To be implemented first, and rewritten in C later.
"""
from collections import defaultdict
from collections import Counter
from functools import reduce
from operator import or_
from py_replace.Grid_lib import Grid
from py_replace.static import flatten


class BDCSP_colver:
    """Temporary python replacement!"""
    def __init__(self, id_to_pattern, pat_id_to_pos, pos_to_pat,
                 all_pos, str_len, str_num, act_col_num, k):
        """Entry point."""
        self.id_to_pattern = id_to_pattern
        self.pat_num = len(self.id_to_pattern)
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
        self._got_non_trivial = False
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
        print("# Inf: {0:.4f} average_RO: {1:.4f} Sup: {2:.4f}".format(self.inf,
                                                                       self.exp_ave_ro,
                                                                       self.sup))
        # get rid of obvious cases
        if self.exp_ave_ro > self.sup:
            print("# Required density to satisfy K given is too high")
            self.answer = False
        elif self.exp_ave_ro <= self.inf:
            print("# Required density is lower that potential minimal.")
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
        comb_ro = [self.__comb_sum(comb)[0] / len(comb) for comb in self.combs]
        comb_and_ro = sorted(zip(self.combs, comb_ro), key=lambda x: x[1], reverse=True)
        self.combs = [x[0] for x in comb_and_ro]
        self._ro = [x[1] for x in comb_and_ro]
        comb_dens = [x[1] for x in comb_and_ro]
        self.comb_index = defaultdict(list)
        self.comb_to_id = {}
        self.comb_id_to_ro = {}
        self.pat_to_combs = defaultdict(set)
        self.pat_to_combs_ctr = defaultdict(Counter)
        self.ro_to_ind = defaultdict(list)
        for i, comb in enumerate(self.combs):
            comb_len = len(comb)
            comb_ro = comb_dens[i]
            cur_ind = len(self.comb_index[comb_len])
            self.comb_index[comb_len].append(comb)
            comb_id = (comb_len, cur_ind)
            for p_ in comb:
                self.pat_to_combs[p_].add(comb_id)
            self.ro_to_ind[comb_ro].append(comb_id)
            self.comb_to_id[comb] = comb_id
            self.comb_id_to_ro[comb_id] = comb_ro
        for k, v in self.pat_to_combs.items():
            self.pat_to_combs_ctr[k] = Counter(v)
        print("# Extracted and indexed {} combinations".format(self.base_combs_num))

    def __comb_compat(self):
        """Create comb compatibility matrix."""
        self.comb_id_compat_with = defaultdict(set)
        for i in range(self.base_combs_num):
            i_comb = self.combs[i]
            i_pos = [tuple(self.pattern_id_to_positions[p]) for p in i_comb]
            i_id = self.comb_to_id[i_comb]
            # self-compatibility is allowed
            for j in range(i, self.base_combs_num):
                j_comb = self.combs[j]
                j_pos = [tuple(self.pattern_id_to_positions[p]) for p in j_comb]
                j_id = self.comb_to_id[j_comb]
                to_check = Counter(i_pos + j_pos)
                # very simple compatibility check
                # if some positions tuple appears more times than it's lenght
                # then combination is impossible
                if any(len(k) < v for k, v in to_check.items()):
                    continue
                self.comb_id_compat_with[i_id].add(j_id)
                self.comb_id_compat_with[j_id].add(i_id)

    def __check_path_ok(self, chain):
        """Check if chains path is compatible."""
        combs = [self.comb_index[c[0]][c[1]] for c in chain]
        comb_united = flatten(combs)
        comb_pos_count = Counter(tuple(self.pattern_id_to_positions[p])
                                       for p in comb_united)
        if any(len(k) < v for k, v in comb_pos_count.items()):
            return False
        return True

    def __generate_base_cliques(self, skip_short=True):
        """Cliques generator."""
        for start in range(self.base_combs_num):
            start_id = self.comb_to_id[self.combs[start]]
            chain = [start_id, ]
            compat_with = self.comb_id_compat_with[start_id]
            if not compat_with:
                yield chain
                continue
            iter_collection = [iter(compat_with), ]
            pointer = 0
            flag = True
            while pointer >= 0:
                chain_compat = reduce(or_, (self.comb_id_compat_with[c] for c in chain))
                next_elem = next(iter_collection[pointer], None)
                if next_elem:
                    flag = True
                    next_chain = chain + [next_elem, ]
                    ok_ = self.__check_path_ok(next_chain)
                    if ok_ is False:
                        continue
                    next_lvl = iter(self.comb_id_compat_with[next_elem]\
                        .intersection(chain_compat))
                    iter_collection.append(next_lvl)
                    chain.append(next_elem)
                    pointer += 1
                    continue
                yield chain
                if skip_short:
                    break
                pointer -= 1
                del chain[-1]
                if flag is True:
                    yield chain
                    pointer -= 1
                    del chain[-1]
                    flag = False
                else:
                    pointer -= 1
                    del chain[-1]

    def __get_pos_left(self, pos_list):
        """Create a list of positions that left."""
        occupied_ = set()
        for positions in pos_list:
            not_occ = positions.difference(occupied_)
            first_elem_= next(iter(not_occ))
            occupied_.add(first_elem_)
        not_occ = set(self.all_positions).difference(occupied_)
        return not_occ

    def __get_tail_from_positions(self, positions):
        """For a list of positions given extract the best possible combination."""
        pats = [self.position_to_patterns[p] for p in positions]
        pos_len = len(positions)
        indexes = [0 for _ in range(pos_len)]
        switched = [0 for _ in range(pos_len)]
        switch_infl = [0 for _ in range(pos_len)]
        comb = [pats[i][indexes[i]] for i in range(pos_len)]
        comb_k = min(self.__comb_sum(comb))
        excluded = set()
        possible = True
        while possible:
            for i in range(pos_len):
                if switched[i] >= 3:
                    excluded.add(i)
            infl_round = {1: [], 0: [], -1: []}
            for n_p in range(pos_len):
                if n_p in excluded:
                    continue
                switched = indexes.copy()
                switched[n_p] =  1 if switched[n_p] == 0 else 0
                i_comb = [pats[i][switched[i]] for i in range(pos_len)]
                i_comb_k = min(self.__comb_sum(i_comb))
                infl = i_comb_k - comb_k
                switch_infl[n_p] = infl
                infl_round[infl].append(n_p)
            if infl_round[1]:
                to_swicth = infl_round[1][0]
                switched[to_swicth] += 1
                indexes[to_swicth] = 1 if indexes[to_swicth] == 0 else 0
                comb_k += 1
                continue
            elif infl_round[0]:
                to_swicth = infl_round[0][0]
                switched[to_swicth] += 1
                indexes[to_swicth] = 1 if indexes[to_swicth] == 0 else 0
                continue
            if not infl_round[1] and not infl_round[0]:
                break
            elif min(switched) == 2:
                break
        result_comb = [pats[i][indexes[i]] for i in range(pos_len)]
        return min(self.__comb_sum(result_comb))

    def __check_enough(self):
        """Check, it trivial cobinations are enough to make a decision."""
        self.sup = self._ro[0]
        print("# Redefined sup: {}".format(self.sup))
        if self.exp_ave_ro > self.sup:
            # abort in this case
            print("# Redefined sup < average expected density")
            self.answer = False
            return
        cliques_gen = self.__generate_base_cliques()
        for cliq in cliques_gen:
            cliq_pats = flatten([self.comb_index[c[0]][c[1]] for c in cliq])
            cliq_pos = [set(self.pattern_id_to_positions[p]) for p in cliq_pats]
            not_occ = self.__get_pos_left(cliq_pos)
            # cliq_len = len(cliq_pats)
            # pos_left = self.act_col_num - cliq_len
            cliq_cov = min(self.__comb_sum(cliq_pats))
            left_on_the_rest = self.__get_tail_from_positions(not_occ)
            total_cover = left_on_the_rest + cliq_cov
            if total_cover >= self.to_cover:
                print("# Basepoints cover enoung positions: {}".format(total_cover))
                self.answer = True
                return

    @staticmethod
    def __can_include(ctr_1, ctr_2):
        """Check if we can add this pattern."""
        check_counter = ctr_1 + ctr_2 if ctr_2 else ctr_1
        if any(len(k) < v for k, v in check_counter.items()):
            return False
        return True

    def __get_next(self, i):
        """Return the next index if possible."""
        if i + 1 == self.pat_num:
            return None
        return i + 1

    def __build_chain(self, prev_path):
        """Build a chain."""
        exclude = set(self.pat_neg[i] for i in prev_path)
        chain = prev_path.copy()
        ctr = Counter()
        for elem in prev_path:
            ctr += self.pat_to_combs_ctr[elem]
        if not self.__can_include(ctr, None):
            return None
        last_elem = prev_path[-1]
        ptr = self.__get_next(last_elem)
        possible = True
        while possible:
            if ptr is None:
                break
            elif ptr in exclude:
                ptr = self.__get_next(ptr)
                continue
            ptr_ctr = self.pat_to_combs_ctr[ptr]
            includeable = self.__can_include(ctr, ptr_ctr)
            if not includeable:
                ptr = self.__get_next(ptr)
                continue
            chain.append(ptr)
            ctr += ptr_ctr
            exclude.add(self.pat_neg[ptr])
            ptr = self.__get_next(ptr)
        return chain
    
    def __split_non_tri(self, chain):
        """Slices to get non-trivial combinations."""
        sum_ = [0 for _ in range(self.str_num)]
        for i, pat_id in enumerate(chain):
            pat = self.id_to_pattern[pat_id]


    def __get_non_trivial_points(self):
        """Final part, get non_trivial points."""
        self.non_trivial = []
        # build initial chain
        chain = self.__build_chain([0, ])
        col_num = len(chain) - 1
        possible = True
        while possible:
            # move pointer left if no way to decrease ID
            # right if a new chain appeared
            cur_val = chain[col_num]
            decr_val = self.__get_next(cur_val)
            if decr_val is None:
                col_num -= 1
                continue
            path = chain[: col_num]
            path.append(decr_val)
            chain = self.__build_chain(path)
            non_tri_splits = self.__split_non_tri(chain)
            break

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
        self.__check_enough()
        if self.answer is not None:
            return self.answer
        self.__get_non_trivial_points()
        return None


if __name__ == "__main__":
    pass
