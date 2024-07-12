from os.path import abspath, dirname, join
from subprocess import run, PIPE
from tempfile import mktemp

import matplotlib.pyplot as plt
import numpy as np

from util import generate


def main(variations, precision=128, max_exponent=18):
    source_dir = abspath(join(dirname(__file__), '..'))
    binary_dir = join(source_dir, 'cmake-build-debug')
    executable = join(binary_dir, 'bitvector')

    run(['cmake', '--build', binary_dir], check=True)

    def gen_exp():
        for p in range(precision):
            yield pow(p / precision, 1.7)

    def gen():
        for i, k in enumerate([_ for _ in gen_exp()]):
            p = 128 + (k * (2 ** max_exponent))
            yield int(p)

    points = sorted(list(set([_ for _ in gen() if _ > 1])))

    test_paths = []
    smaller_test_paths = []
    xpoints = []
    for i__ in points:
        len_bit_vector = i__

        test_path = mktemp('.txt')

        bitvector = generate(num_operations=32768, len_bit_vector=len_bit_vector, probability=(0, 100))
        with open(test_path, 'w') as f:
            [f.write(_) for _ in bitvector]

        test_paths.append(test_path)
        if i__ < 2 ** 15:
            smaller_test_paths.append(test_path)
        xpoints.append(i__)

    for variation in variations:
        result_path = mktemp('.txt')
        ypoints = []

        test_paths_ = test_paths if variation != 'linear' else smaller_test_paths
        for test_path in test_paths_:
            result = run([executable, test_path, result_path, variation],
                         check=True, encoding='utf-8', stdout=PIPE)

            result = result.stdout

            print(f"{variation}: {result}", end='')
            space = int(result[result.find('time=') + 5:result.find('space=') - 1])
            ypoints.append(space)

        plt.plot(np.array(xpoints[:len(ypoints)]), np.array(ypoints))


if __name__ == '__main__':
    plt.title("BitVector Time Development")
    plt.xlabel("BitVector Length")
    plt.ylabel("Total Bits Allocated")

    main(['linear', 'const', 'pasta'])
    plt.legend(['LinearRank LinearSelect', 'ConstRank BetterSelect', 'PastaRank PastaSelect'])

    ax = plt.gca()
    ax.set_xscale('log')
    ax.set_yscale('log')

    fig = plt.gcf()
    fig.set_size_inches(11.7, 8.3)
    fig.savefig('bitvector-times-all.png', dpi=192)
