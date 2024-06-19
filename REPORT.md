Project Advanced Datastructures SS 2024
=======================================

Autor: Robert Andreas Fritsch
Date 12.06.2024

Utilities
---------

### Math Wrapper

In the header scr/util.h you can find wrapper around some basic math functionalities used all over the Project.

### BitVector

I designed a basic bit container ```BitVector``` with ```std::vector<std::uint64_t>```.
This container does not yet support rank and select queries but only access and manipulation.

Further more i designed a ```BitsVector``` that uses the Bit vector as a base to store n-bit elements.

Both of these classes support operations like access, push, size _(number of elements)_, bits _(number of bits
allocated)_, ...

### Rank and Select

To use rank and select queries a so called accessor is used. It takes implementations of specific Rank and Select
Accelerators to allow rank and select queries.
This Project has implemented the following Rank and Select Accelerators:

1. LinearRank
2. ConstRank
3. LinearSelect
4. ConstSelect (almost working)

### Main method

The main method parses the file1 and runs the operations with either ```BitVectorAccessor<LinearRank, LinearSelect>``` (
if a third parameter is given) or  ```BitVectorAccessor<ConstRank, LinearSelect>```. The result is written into file2
and statistics are printed to stdout.

Everything like described in the project description. It is to mention that the parsing is done without any checks.
It i certain, that the program will either not recognize a corrupt file or crash fatally without info.
This Program relies on non-corrupt input files.

### Implementation

#### LinearRank and LinearSelect

The ```LinearRank``` and ```LinearSelect``` are implemented trivially but for very small bit vectors (< ~20000 elements)
they are very efficient.

#### ConstRank

The ```ConstRank``` is the most complete advanced technique used in the project and result in high performant and
correct answers.
The specific tuning parameters try to be close to the theory but are chosen to be more practical in reality at some
point:

One fill sample of chosen parameters vs theory parameter:

```
theory bits               : 1.67772e+07
real   bits               : 16777216
theory s_block_bits       : 576
real   s_block_bits       : 504
theory s_block_count      : 29127.1
real   s_block_count      : 33289
theory s_block_elem_size  : 25
real   s_block_elem_size  : 25
theory block_bits         : 12
real   block_bits         : 12
theory block_count_per_sb : 48
real   block_count_per_sb : 42
theory block_count_total  : 1.3981e+06
real   block_count_total  : 1398138
theory block_elem_size    : 10.1699
real   block_elem_size    : 9
theory block_data_bits    : 1.42186e+07
real   block_data_bits    : 12583242
theory s_block_data_bits  : 728178
real   s_block_data_bits  : 832225
```

The exact choices are:

```
block_bits = Max(1, Log2Half(n));
s_block_bits = FloorX(Max(block_bits + 1, NearestPower2(Log2Pow2(n))) - 1, block_bits);

block_count_per_sb = s_block_bits / block_bits;
s_block_count = CeilDiv(n, s_block_bits);
block_count_total = s_block_count * block_count_per_sb;

block_elem_size = (Log2(s_block_bits) + 1);
s_block_elem_size = (Log2(n) + 1);

block_data_bits = block_elem_size * block_count_total;
s_block_data_bits = s_block_elem_size * s_block_count;
```

I did not have the time yet to check if the sizes are solid due to time constraints but for me personally the amount of
bits used by the accelerator are a bit high. Maybe I find the reason for that in the next days.

ConstRank constructs its Accelerator structures in linear time and is very fast.

#### ConstSelect

I have not finished ```ConstSelect```. It works for almost all queries but when doing fuzz testing and comparing the
results to the LinearSelect in few corner cases the result is wrong.
As ```ConstSelect``` is Work in progress select0 and select1 are not equally implemented.

If you have a look please focus on select0.

### Results

#### Size Overhead of the Accelerator Data Structures

![](bitvector-sizes-all.png)

We can see that the space overhead is in fact linear.

#### Performance Comparison between Variants

![](bitvector-times-all.png)

We can see that the speedup is greatly improved. As ConstSelect is not fully implemented I can not guaranty that it will
keep performance for bigger bit vectors nor that it is yet correct but till here it look pretty good.

### Observations

For BitVectors of length ```n < 2^16``` a L1 Block can never be sparse as:

```
|B| >= log2(n)^4 -> "B is sparse"
```

but as ```n >= |B|```, a sparse block can only exist if ```n >= log2(n)^4```.
For ```n < 2^16``` this is false.

Further more if ```n < 2^16``` all dense blocks need to store their offsets using ```log2(log2(n)^4) + 1``` bits.
For ```n < 2^16 -> n < log2(n)^4``` and as we do not expect input beyond out bitvector we can simply
us ```log2(n) + 1``` bits to store the offset. As the bits are also enough to store the absolute offset we can directly
use the L2 Blocks and ignore L1 Blocks completely.

To utilise this I want to implement runtime analysis, that checks the size of the bitvector and only conditionally use
L1 Blocks. To switch implementation at runtime we need function pointer (C Style) or virtual methods.
As virtual methods have not that big of an overhead but do make the codebase much cleaner I will use a Select Base class
and use the composite pattern to layer the logic of L1 and L2 Blocks.

To achieve this another helper construct may come in handy: A BitVectorView.
At the moment BitVectors are used by reference by the Rank and Select classes.
This is suboptimal as this does not handle lifetime very well. Smart-pointer seem to be the right choice over all.
But simply having a handle to the whole BitVector is to much at some points. If we want to handle a L2 case we might
only need a part of the BitVector and handling relative offsets might get messy. Therefor I want to implement a
BitVectorView.
This Construct gets a shared_pointer to the real BitVector and handles the relative offset.

This also allows the feature that rank and select queries could only operate on a specific range.
Even tho I don't see a use-case yet.

### Personal Thoughts

I know this report is not in the right format. But to be honest I did not had time to do it before today.
The Template was not intuitively to me. So I decided to write my Report in markdown to at least have some content. I
assume the content is more important but sorry for the inconvenience.

This project was a lot of fun. MinMaxing 'simple' problems and finding simple solutions to hard problems is one of the
things I like most in it. One of the downsides of this Project was time. Implementing benchmarks, tests and non-trivial
algorithms takes time. Even tho I would not have liked a different project, but I hope the tutors will keep that in
mind.

It is a shame that the ConstSelect is not fully implemented yet. It only supports layer 1 block offsets. 
