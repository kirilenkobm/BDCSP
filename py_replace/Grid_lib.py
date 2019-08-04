#!/usr/bin/env python3
"""Grid class to generate points for combinations search.

Temporary Python replacement.
"""
import sys
import os
from datetime import datetime as dt
from collections import defaultdict
from collections import Counter
import ctypes
from py_replace.static import accumulate_sum
from py_replace.static import flatten
from py_replace.static import flat_set


class Grid:
    """Grid class."""
    def __init__(self, str_num, str_len, f_max, pattern_to_size,
                 positions_to_patterns, patterns_to_positions):
        """Initiate class."""
        self.str_num = str_num
        self.str_len = str_len
        self.f_max = [0] + f_max
        self.f_min = [0] + f_max[::-1]
        self.f_max_acc = accumulate_sum(self.f_max)
        self.f_min_acc = accumulate_sum(self.f_min)
        self.all_available_moves = Counter(self.f_max)
        self.all_numbers = sorted(set(f_max), reverse=True)
        self.max_comb_len = min(self.str_len, self.str_num, len(self.f_max))
        self.pattern_to_size = pattern_to_size
        self.positions_to_patterns = positions_to_patterns
        self.patterns_to_positions = patterns_to_positions
        self.all_positions = set(range(str_len))
        self.__precompute_grid()
        self.switched = False

    def counter_to_C_type(self, counter):
        """Convert counter to array."""
        empty_arr = [0 for _ in range(self.f_max[1] + 1)]
        for k, v in counter.items():
            empty_arr[k] = v
        c_arr = (ctypes.c_int * (self.f_max[1] + 2))()
        c_arr[:-1] = empty_arr
        return c_arr

    @staticmethod
    def __ordnung(points_sequence):
        """Rearrange point sequence.

        First ro = 1/2, then from 1/2 to (N - 1)/ N
        and finally 1/2 -> 1/N.
        """
        left_part, central_part, right_part = [], [], []
        for point in points_sequence:
            p_d = point["density"]
            if p_d > 0.5:
                left_part.append(point)
            elif p_d == 0.5:
                central_part.append(point)
            else:
                right_part.append(point)
        del points_sequence       
        return central_part + left_part[::-1] + right_part

    def __precompute_grid(self):
        """Precompute grid and points."""
        com_points = []  # to use right now
        # go throw all possible points
        for i in range(self.max_comb_len):
            char_num = i + 1
            sum_range = range(self.f_min_acc[char_num], self.f_max_acc[char_num] + 1)
            for p_sum in sum_range:
                lvl_full = p_sum // self.str_num
                if lvl_full == 0:
                    continue
                over = p_sum % self.str_num
                complete = over == 0
                if not complete:
                    continue
                density = lvl_full / char_num
                if lvl_full == char_num - 1 and lvl_full > 1:
                    point_class = 0  # ro =(N - 1) / N; N > 2
                elif lvl_full == 1 and char_num == 2:
                    point_class = 1  # ro = 1/2
                elif lvl_full == 1 and char_num > 2:
                    point_class = 2  # ro = 1 / N; N > 2
                else:  # undefined!
                    point_class = 3
                base = complete and (lvl_full == char_num - 1
                                     or lvl_full == 1)
                point = {"sum": p_sum,
                         "lvl": lvl_full,
                         "over": over,
                         "complete": complete,
                         "density": density,
                         "char_num": char_num,
                         "basepoint": base,
                         "class": point_class}
                # at the beginning we will operate
                # with complete points only
                com_points.append(point)
        c_points_sorted = sorted(com_points,
                                 key=lambda p: (p["density"],
                                                -1 * p["over"],
                                                -1 * p["char_num"]),
                                 reverse=True)
        # actually need a bit different configuration
        # self.c_points_sorted = self.__ordnung(c_points_sorted)
        self.c_points_sorted = c_points_sorted
        self.points_num = len(self.c_points_sorted)
        

    def request_tail(self, pat_length):
        """Return the last point. Consider already used combinations."""
        all_points = []
        # very rough filter, but excludes some incompatible cases
        sum_range = range(self.f_min_acc[pat_length], self.f_max_acc[pat_length] + 1)
        for p_sum in sum_range:
            point = {"sum": p_sum,
                     "char_num": pat_length}
            all_points.append(point)
        # already sorted, but we need to reverse this
        for point in all_points[::-1]:
            first_path = self.__get_s_path([], point)
            if not first_path:
                continue
            yield first_path
            extensions = self.__extend_paths(first_path, point)
            for elem in extensions:
                yield elem
        # if generator is empty, just put a random stuff

    def __get_next_size(self, current_size):
        """Get the next lesser size."""
        for item in self.all_numbers:
            # the list is sorted, so the first lesser
            # is what we are looking for
            if item < current_size:
                return item
        return None

    def __check_current(self, current, counter):
        """Check if the current value is legit."""
        # TODO: re-implement in C
        used = counter[current]
        available = self.all_available_moves[current]
        if available == used:
            # we used too many Ns, they are not available anymore
            return self.__get_next_size(current)
        # it's ok, we still have it
        if available < used:
            # should never happen, just in case
            err_msg = "Error! Size_path grabbed more {}'s ({}) "\
                      "that are available ({})!"\
                      "".format(current, used, available)
            raise OverflowError(err_msg)
        return current

    def __get_s_path(self, path, point, current=None):
        """Compute the first sizes path to the point."""
        # TODO: re-implement in C
        path_count = Counter(path)
        # get minimal available number
        current = self.all_numbers[0] if current is None else current
        target_sum = point["sum"]
        target_length = point["char_num"]
        pos_left = target_length - len(path)
        for i in range(pos_left):
            passed = False
            prev_sum = sum(path)
            current = self.__check_current(current, path_count)
            while not passed:
                if not current:
                    # next elem doesn't exist
                    return None
                intermed_val = prev_sum + current
                delta = target_sum - intermed_val
                points_left = pos_left - (i + 1)

                top_border = self.f_max_acc[points_left]
                bottom_border = self.f_min_acc[points_left]
                if delta > top_border:
                    # unreachable
                    break
                elif delta < bottom_border:
                    # too much
                    # we did't use a smaller size so
                    # it is available
                    current = self.__get_next_size(current)
                    continue
                passed = True
                path.append(current)
                path_count[current] += 1
        if sum(path) != target_sum:
            return None
        return path

    def __extend_paths(self, first_path, point):
        """Increase amount of paths if possible."""
        # extra_paths = []
        # target_sum = point["sum"]
        target_length = point["char_num"]
        # each time we used the biggest possible size
        # let's try a smaller one
        for pointer in range(target_length - 1, -1, -1):
            pointed = first_path[pointer]
            possible = True
            while possible:
                lower = self.__get_next_size(pointed)
                if not lower:
                    possible = False
                    break
                try_path = first_path[:pointer]
                try_path.append(lower)
                extra_path = self.__get_s_path(try_path, point, lower)
                pointed = lower
                if extra_path is None:
                    continue
                yield extra_path

    def base_points_generator(self):
        """Yield the best point at the moment of time."""
        point_num = 0
        for point in self.c_points_sorted:
            if point["basepoint"] is False:
                # non-trivial points!
                continue
            first_path = self.__get_s_path([], point)
            if not first_path:
                # not reachable
                continue
            # yield it right now
            yield first_path, point_num, point["class"]
            # let's try to find any other way
            extensions = self.__extend_paths(first_path, point)
            for elem in extensions:
                yield elem, point_num, point["class"]
            point_num += 1
        # final dummy
        yield [], 0, -1


if __name__ == "__main__":
    pass
