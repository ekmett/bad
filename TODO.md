* [Dynamic Tensor Rematerialization](https://arxiv.org/pdf/2006.09616.pdf) provides a technique for automatic checkpointing, which seems inherently compatible with the approach to reverse mode I want.

* [Fast AD](https://github.com/JamesYang007/FastAD/tree/master/include/fastad_bits/reverse) shows overloading comma might be useful.

* Flatten the namespace? It is a bit overblown in its current form.

* A proper reverse mode

* All the missing operators

* Fully general einsum and einsum path solver

* Ensure einsum can provide an updatable view when applied to an expr with no repeated indices

* Autolifting when tensors only share common tails

* Named tensor operations? i'd have to pass str<> based names of each dimension as an optional 4th parameter, and add some template magic for the decltype noise.

* Am I using functions.hh at all?
