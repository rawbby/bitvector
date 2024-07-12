import math
from random import randint
from typing import Union, Tuple, List, AnyStr


def generate(num_operations: Union[int, Tuple[int, int]], len_bit_vector: Union[int, Tuple[int, int]],
             probability: Union[int, Tuple[int, int]] = 50) -> List[AnyStr]:
    if isinstance(num_operations, Tuple):
        num_operations = randint(*num_operations)
    if isinstance(len_bit_vector, Tuple):
        len_bit_vector = randint(*len_bit_vector)
    if isinstance(probability, Tuple):
        probability = randint(*probability)

    # noinspection SpellCheckingInspection
    def randbit(p) -> int:
        bit = randint(0, 100)
        while bit == p:
            bit = randint(0, 100)
        return 1 if p > bit else 0

    buffer: [str] = []

    def write(m: str):
        buffer.append(m)

    write(str(num_operations) + '\n')

    bitvector = [randbit(probability) for _ in range(len_bit_vector)]
    num_ones = sum(bitvector)
    num_zeroes = len(bitvector) - num_ones

    if num_ones == 0:
        bitvector[randint(0, len(bitvector) - 1)] = 1
        num_ones = 1
    if num_zeroes == 0:
        bitvector[randint(0, len(bitvector) - 1)] = 0
        num_zeroes = 1

    [write(str(b)) for b in bitvector]
    write('\n')

    write(f"rank 0 0\n")
    write(f"rank 0 {len(bitvector) - 1}\n")
    write(f"rank 1 0\n")
    write(f"rank 1 {len(bitvector) - 1}\n")
    write(f"select 0 1\n")
    write(f"select 0 {num_zeroes}\n")
    write(f"select 1 1\n")
    write(f"select 1 {num_ones}\n")
    write(f"access 0\n")
    write(f"access {len(bitvector) - 1}\n")

    for _ in range(max(10, num_operations)):
        op = randint(0, 4)
        if op == 1:
            write(f"rank 0 {randint(0, len(bitvector) - 1)}\n")
        elif op == 2:
            write(f"rank 1 {randint(0, len(bitvector) - 1)}\n")
        elif op == 3:
            write(f"select 0 {randint(1, num_zeroes)}\n")
        elif op == 4:
            write(f"select 1 {randint(1, num_ones)}\n")
        else:
            write(f"access {randint(0, len(bitvector) - 1)}\n")

    return buffer


def log2(x: int) -> int:
    return int(math.log2(float(x)))


def bit_width(x: int) -> int:
    return log2(x) + 1


def log2half(x: int) -> int:
    return log2(x) >> 1


def log2pow2(x: int) -> int:
    return log2(x) ** 2


def ceil(x: float) -> int:
    return int(math.ceil(x))


def floor(x: float) -> int:
    return int(math.floor(x))


def ceil_div(a: int, b: int) -> int:
    return ceil(float(a) / float(b))


def floor_div(a: int, b: int) -> int:
    return floor(float(a) / float(b))


def floor_x(a: int, b: int) -> int:
    return floor_div(a, b) * b


def ceil_x(a: int, b: int) -> int:
    return floor_x(a, b) + (b if a % b else 0)


def next_pow2(x: int) -> int:
    x -= 1
    x |= x >> 1
    x |= x >> 2
    x |= x >> 4
    x |= x >> 8
    x |= x >> 16
    x |= x >> 32
    x += 1
    return x


def nearest_pow2(x: int) -> int:
    next_ = next_pow2(x)
    prev_ = next_ >> 1

    if next_ - x > x - prev_:
        return prev_
    else:
        return next_


def steps_kb():
    kb_bits = 1024 * 8

    def range_(lower, upper):  # range with kb_bits steps
        return range(lower, upper, max(1, int((upper - lower) / kb_bits)))

    for i in range_(1, kb_bits):
        yield i


def steps_mb():
    kb_bits = 1024 * 8
    mb_bits = 1024 * kb_bits

    def range_(lower, upper):  # range with kb_bits steps
        return range(lower, upper, max(1, int((upper - lower) / kb_bits)))

    for i in range_(1, kb_bits):
        yield i
    for i in range_(kb_bits, mb_bits):
        yield i


def steps_gb():
    kb_bits = 1024 * 8
    mb_bits = 1024 * kb_bits
    gb_bits = 1024 * mb_bits

    def range_(lower, upper):  # range with kb_bits steps
        return range(lower, upper, max(1, int((upper - lower) / kb_bits)))

    for i in range_(1, kb_bits):
        yield i
    for i in range_(kb_bits, mb_bits):
        yield i
    for i in range_(mb_bits, gb_bits):
        yield i


def steps_tb():
    kb_bits = 1024 * 8
    mb_bits = 1024 * kb_bits
    gb_bits = 1024 * mb_bits
    tb_bits = 1024 * gb_bits

    def range_(lower, upper):  # range with kb_bits steps
        return range(lower, upper, max(1, int((upper - lower) / kb_bits)))

    for i in range_(1, kb_bits):
        yield i
    for i in range_(kb_bits, mb_bits):
        yield i
    for i in range_(mb_bits, gb_bits):
        yield i
    for i in range_(gb_bits, tb_bits):
        yield i
