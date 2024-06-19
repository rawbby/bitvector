import matplotlib.pyplot as plt
import numpy as np

from util import *


def bitvector_bits(n: int) -> int:
    return ceil_x(n, 64)


def const_rank_bits(n: int) -> int:
    block_bit_range = max(1, log2half(n))

    super_block_bit_range = log2pow2(n)
    super_block_bit_range = nearest_pow2(super_block_bit_range) - 1
    super_block_bit_range = max(block_bit_range << 1, super_block_bit_range)
    super_block_bit_range = floor_x(super_block_bit_range, block_bit_range)

    super_block_count = ceil_div(n, super_block_bit_range)
    block_count_total = ceil_div(n, block_bit_range)

    bits_per_block_element = bit_width(super_block_bit_range)
    bits_per_super_block_element = bit_width(n)

    # round up as stored in std::vector<std::uint64_t>
    blocks_vector_bits = ceil_x(block_count_total * bits_per_block_element, 64)
    super_blocks_vector_bits = ceil_x(super_block_count * bits_per_super_block_element, 64)

    return blocks_vector_bits + super_blocks_vector_bits


def const_select_bits(n: int) -> int:
    return 0


if __name__ == '__main__':
    plt.title("BitVector Size Development")
    plt.xlabel("BitVector Size")
    plt.ylabel("Total Bits Allocated")

    plt.legend(['ConstRankSelect', 'LinearRankSelect'])

    steps = np.array([s for s in steps_tb()])
    plt.plot(steps, np.array([bitvector_bits(s) for s in steps]))
    plt.plot(steps, np.array([bitvector_bits(s) + const_rank_bits(s) + const_select_bits(s) for s in steps]))

    ax = plt.gca()
    ax.set_xscale('log')
    ax.set_yscale('log')

    fig = plt.gcf()
    fig.set_size_inches(11.7, 8.3)
    fig.savefig('bitvector-sizes-all.png', dpi=192)
