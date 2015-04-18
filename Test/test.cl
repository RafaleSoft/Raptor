__kernel void test(__global int* a,__global int *b,__global int *c)
{
	unsigned int id = get_global_id(0);
	c[id] = a[id] + b[id];
}
