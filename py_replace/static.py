"""Functions that might be static. To avoid overloading the main script."""

def parts(lst, n):
    """Split iterable in a list of iterables of size n."""
    return [lst[i: i + n] for i in iter(range(0, len(lst), n))]


def accumulate_sum(lst):
    """Return accumulated sum list."""
    if len(lst) == 1:
        return lst
    accumulated_sum = [lst[0]]
    for i in range(1, len(lst)):
        accumulated_sum.append(accumulated_sum[i - 1] + lst[i])
    return accumulated_sum


def flatten(lst):
    """Flatten a list of lists into a list."""
    return [item for sublist in lst for item in sublist]


def flat_set(lst):
    """Flatten a list of lists into a set."""
    return set(item for sublist in lst for item in sublist)
