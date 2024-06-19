import sys
from subprocess import run
from tempfile import mktemp
from os.path import abspath, dirname, join

from util import *

if __name__ == '__main__':
    source_dir = abspath(join(dirname(__file__), '..'))
    binary_dir = join(source_dir, 'cmake-build-debug')
    executable = join(binary_dir, 'bitvector')

    run(['cmake', '-S', source_dir, '-B', binary_dir, '-DINCLUDE_PASTA_BIT_VECTOR=ON'], check=True)
    run(['cmake', '--build', binary_dir], check=True)

    for _ in range(48):
        buffer = generate((32768, 65536), (65536, 262144), (25, 75))

        tp = mktemp('.txt')
        with open(tp, 'w') as f:
            [f.write(it) for it in buffer]

        print('pasta: ', end='')
        rp = mktemp('.txt')
        run([executable, tp, rp, 'pasta'], check=True)

        print('const: ', end='')
        cp = mktemp('.txt')
        run([executable, tp, cp, 'const'], check=True)

        with open(tp, 'r') as tf:
            tf.readline()
            tf.readline()

            with open(rp, 'r') as rf, open(cp, 'r') as cf:
                tl = tf.readline()
                rl = rf.readline()
                cl = cf.readline()
                if rl != cl:
                    print(f"{tl} should be {rl} but is {cl}!", file=sys.stderr)
