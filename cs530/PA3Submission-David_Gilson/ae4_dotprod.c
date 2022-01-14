//Dot Product Function unrolled by a factor of 4 with accumulator expansion
float dotprod(float x[], float y[], int n)
{
   int i;
   float sum1, sum2, sum3, sum4, sum5;
   //Always initialize values!!!
   sum1 = 0.0;
   sum2 = 0.0;
   sum3 = 0.0;
   sum4 = 0.0;
   sum5 = 0.0;
   
   for (i = 0; i < n % 4; i++)
      sum1 += x[i] * y[i];
  
   for (; i < n; i += 4){
      sum2 += x[i] * y[i];
	  sum3 += x[i+1] * y[i+1];
	  sum4 += x[i+2] * y[i+2];
	  sum5 += x[i+3] * y[i+3];
   }
   
   return sum1+sum2+sum3+sum4+sum5;
}