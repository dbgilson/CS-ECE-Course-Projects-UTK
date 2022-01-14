#include <emmintrin.h>

//Dot Product function using SIMD instructions
float dotprod(float x[], float y[], int n)
{
	int i;
	//Create properly aligned floating pointers.
	float *p1 __attribute__ ((aligned(16)));
	float *p2 __attribute__ ((aligned(16)));
	
	//Initialize 128 bit vector registers (4 FP values in each)
	__m128 mul_vec = {0.0, 0.0, 0.0, 0.0};
	__m128 tmpx = {0.0, 0.0, 0.0, 0.0};
	__m128 tmpy = {0.0, 0.0, 0.0, 0.0};
	
	//Value to return
	float final_sum = 0.0;
	
	for(i = 0; i < n; i += 4){
		//Initializing the sum vector
		__m128 sum_vec = {0.0, 0.0, 0.0, 0.0};
		//Get pointers to vectors and load them
		p1 = x + i;
		p2 = y + i;
		tmpx = _mm_load_ps(p1);
		tmpy = _mm_load_ps(p2);
		
		//Multiply vectors then sum
		mul_vec = _mm_mul_ps(tmpx, tmpy);
		sum_vec = _mm_add_ps(sum_vec, mul_vec);
		
		//Store sum vec in temporary float array alligned to 16
		float tmp_arr[4] __attribute__ ((aligned(16))) = {0.0, 0.0, 0.0, 0.0};
		_mm_store_ps(tmp_arr, sum_vec);
		
		//Add to final sum value
		final_sum += tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3];
	}
	return final_sum;
}
