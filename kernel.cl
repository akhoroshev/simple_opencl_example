
__kernel void bitwise_and(const __global unsigned int *a,
                          const __global unsigned int *b,
                          __global unsigned int *c) {
  int i = get_global_id(0);

  c[i] = a[i] & b[i];
}
