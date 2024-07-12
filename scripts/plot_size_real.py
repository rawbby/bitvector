from os.path import abspath, dirname, join
from subprocess import run, PIPE
from tempfile import mktemp

import matplotlib.pyplot as plt
import numpy as np

from util import generate


def main(variations):
    source_dir = abspath(join(dirname(__file__), '..'))
    binary_dir = join(source_dir, 'cmake-build-debug')
    executable = join(binary_dir, 'bitvector')

    run(['cmake', '-S', source_dir, '-B', binary_dir, '-DINCLUDE_PASTA_BIT_VECTOR=ON'], check=True)
    run(['cmake', '--build', binary_dir], check=True)

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

    test_paths = []
    xpoints = []
    for i__ in sorted(list(set([_ for _ in gen()]))):
        i__ = max(2, i__)

        test_path = mktemp('.txt')

        num_operations = 2
        len_bit_vector = i__
        probability = 50

        bitvector = generate(num_operations, len_bit_vector, probability)
        with open(test_path, 'w') as f:
            [f.write(_) for _ in bitvector]

        test_paths.append(test_path)
        xpoints.append(i__)

    for variation in variations:
        result_path = mktemp('.txt')
        ypoints = []
        for test_path in test_paths:
            result = run([executable, test_path, result_path, variation],
                         check=True, encoding='utf-8', stdout=PIPE)

            space = int(result.stdout[result.stdout.find('space=') + 6:-1])
            ypoints.append(space)

        plt.plot(np.array(xpoints), np.array(ypoints))


if __name__ == '__main__':
    plt.title("BitVector Size Development")
    plt.xlabel("BitVector Length")
    plt.ylabel("Total Bits Allocated")

    main(['linear', 'const', 'pasta'])
    plt.legend(['LinearRank LinearSelect', 'ConstRank BetterSelect', 'PastaRank PastaSelect'])

    ax = plt.gca()
    ax.set_xscale('log')
    ax.set_yscale('log')

    fig = plt.gcf()
    fig.set_size_inches(11.7, 8.3)
    fig.savefig('bitvector-sizes-all.png', dpi=192)
