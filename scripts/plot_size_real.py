import sys
from subprocess import run, PIPE
from tempfile import mktemp
from os.path import abspath, dirname, join, exists
from random import randint
import matplotlib.pyplot as plt
import numpy as np


def main(use_variation=0):
    run(['cmake', '--build', abspath(join(dirname(__file__), 'cmake-build-release'))], check=True)

    xpoints = []
    ypoints = []

    def gen_exp(precision):
        for p in range(precision):
            yield pow(p / precision, 1.7)

    def gen():
        minimum = 2
        precision = 128
        max_exponent = 18

        for i, k in enumerate([_ for _ in gen_exp(precision)]):
            p = minimum + (k * (2 ** max_exponent))
            yield int(p)

    for i__ in sorted(list(set([_ for _ in gen()]))):
        if i__ <= 1:
            i__ = 2

        num_operations = 2
        len_bit_vector = i__

        probability = 50

        test_path = mktemp('.txt')
        result_linear = mktemp('.txt')
        result_const = mktemp('.txt')

        if not exists(test_path):
            with open(test_path, 'w') as f:
                f.write(str(num_operations) + '\n')

                num_ones = 0
                num_zeroes = 0
                while num_ones == 0 or num_zeroes == 0:
                    bitvector = [1 if probability > randint(0, 100) else 0 for __ in range(len_bit_vector)]
                    num_ones = sum(bitvector)
                    num_zeroes = len(bitvector) - num_ones

                for b in bitvector:
                    f.write(str(b))
                f.write('\n')

                for __ in range(num_operations):
                    op = randint(0, 4)
                    if op == 1:
                        f.write(f"rank 0 {str(randint(0, len(bitvector) - 1))}\n")
                    elif op == 2:
                        f.write(f"rank 1 {str(randint(0, len(bitvector) - 1))}\n")
                    elif op == 3:
                        f.write(f"select 0 {str(randint(0, num_zeroes - 1))}\n")
                    elif op == 4:
                        f.write(f"select 1 {str(randint(0, num_ones - 1))}\n")
                    else:
                        f.write(f"access {str(randint(0, len(bitvector) - 1))}\n")

        result = run(['cmake-build-release/bitvector', test_path, result_const, *[str(_) for _ in range(use_variation)]],
                     check=True, encoding='utf-8', stdout=PIPE)

        space = int(result.stdout[result.stdout.find('space=') + 6:-1])
        xpoints.append(i__)
        ypoints.append(space)

    plt.plot(np.array(xpoints), np.array(ypoints))


if __name__ == '__main__':
    plt.title("BitVector Size Development")
    plt.xlabel("BitVector Length")
    plt.ylabel("Total Bits Allocated")

    [main(_) for _ in range(3)]

    plt.legend(['ConstRank LinearSelect', 'LinearRank LinearSelect', 'ConstRank ConstSelectPart'])
    plt.savefig('bitvector-sizes-all.png')
