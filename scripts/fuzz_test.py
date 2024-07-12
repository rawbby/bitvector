import sys
from os.path import abspath, dirname, join
from subprocess import run
from tempfile import mktemp

from util import *

if __name__ == '__main__':
    source_dir = abspath(join(dirname(__file__), '..'))
    binary_dir = join(source_dir, 'cmake-build-debug')
    executable = join(binary_dir, 'bitvector')

    run(['cmake', '-S', source_dir, '-B', binary_dir, '-DINCLUDE_PASTA_BIT_VECTOR=ON'], check=True)
    run(['cmake', '--build', binary_dir], check=True)

    for i in range(1, 49):
        i_ramp = i / 48.0

        num_operations_min = int(i_ramp * 32768)
        num_operations_max = int(i_ramp * 65536)
        num_operations = (num_operations_min, num_operations_max)

        len_bit_vector_min = int(i_ramp * 65536)
        len_bit_vector_max = int(i_ramp * 262144)
        len_bit_vector = (len_bit_vector_min, len_bit_vector_max)

        buffer = generate(num_operations, len_bit_vector, 0 if i == 24 else 1 if i == 25 else (0, 100))

        tp = mktemp('.txt')
        with open(tp, 'w') as f:
            [f.write(it) for it in buffer]

        print(f"{i}. pasta: ", end='')
        rp = mktemp('.txt')
        run([executable, tp, rp, 'pasta'], check=True)

        print(f"{i}. const: ", end='')
        cp = mktemp('.txt')
        run([executable, tp, cp, 'const'], check=True)

        errors = []

        with open(tp, 'r') as tf:
            tf.readline()
            tf.readline()

            with open(rp, 'r') as rf, open(cp, 'r') as cf:
                while True:
                    tl = tf.readline()
                    rl = rf.readline()
                    cl = cf.readline()
                    if len(rl) == 0 and len(cl) == 0:
                        break
                    if rl != cl:
                        errors.append(f"{tl[:-1]} should be {rl[:-1]} but is {cl[:-1]}!")

        if len(errors) > 5:
            [print(errors[i], file=sys.stderr) for i in range(5)]
            print(f"{len(errors) - 5} and more errors ...", file=sys.stderr)
        else:
            [print(error, file=sys.stderr) for error in errors]
        if len(errors) > 0:
            print(tp, cp)
