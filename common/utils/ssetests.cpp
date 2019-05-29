#include "ssetests.h"

#define SSE_ITER_N 100000000


#include <emmintrin.h>
#include <mmintrin.h>
#include <smmintrin.h>

void saxpy_simd4(float* S,
                        float _a,
                        const float* X,
                        const float *Y,
                        int n)
{
   __m128 a = _mm_set1_ps(_a);
   for(int i=0; i!=n ;i+=4)  // process 4 elements at a time
   {
       __m128 x = _mm_load_ps(X+i);
       __m128 y = _mm_load_ps(Y+i);
       __m128 s = _mm_add_ps(_mm_mul_ps(a,x),y);  // a*x + y
       _mm_store_ps(S+i, s );
   }
}

float hsum_ps_sse3(__m128 v) {
  const __m128 t = _mm_add_ps(v, _mm_movehl_ps(v, v));
  const __m128 sum = _mm_add_ss(t, _mm_shuffle_ps(t, t, 1));
  return _mm_cvtss_f32(sum);
}

float proc_ps_sse3(float *arr )
{
  double a = arr[0] + arr[1];
  double b = arr[2] + arr[3];
  double  c = a + b;
  return  c;
}


//inline QPointF calculateBezier(const QPointF &p0, const QPointF &p1, const QPointF &p2, const QPointF &p3, double t)
//  0  1   2  3   4  5   6  7
// x0,y0, x1,y1, x2,y2, x3,y3
//
// m0 = x0,y0, x1,y1,
// m1 = x1,y1, x2,y2,
//
inline void calculateBezier(float * p0, float * p1, double t, float * res_ptr)
{
  __m128 zero = _mm_set1_ps(0);
  __m128 tttt = _mm_set1_ps(t);
  __m128 x0_y0_x1_y1 = _mm_load_ps(&p0[0]);
  __m128 x1_y1_x2_y2 = _mm_load_ps(&p1[0]);
  //
  __m128 ax_ay_bx_by = _mm_add_ps(x0_y0_x1_y1,
                             _mm_mul_ps(
                                        _mm_sub_ps(x1_y1_x2_y2, x0_y0_x1_y1), tttt));
///    Bits [3:0] specify the values copied from operand \a a. \n
///    Bits [7:4] specify the values copied from operand \a b. \n

  // (b - a) => (bx - ax)(by - ay)
  __m128 b_sub_a = _mm_mul_ps(_mm_hsub_ps(zero, _mm_shuffle_ps(ax_ay_bx_by, ax_ay_bx_by, 0b00100111)), tttt);


/*
  QPointF a = p0 + (p1 - p0) * t;
  QPointF b = p1 + (p2 - p1) * t;
  QPointF p102 = a + (b - a) * t;
  return p102 + ((b + (p2 + (p3 - p2) * t - b) * t) - p102) * t;
*/

  _mm_store_ps(res_ptr, b_sub_a);
}



SSETests::SSETests()
{
}


void SSETests::process()
{
#ifdef sse_test
  // sse
  {
    float t = 0.5;
    //                  p0   p1   p2   p3
    float points0[8] = {1,22, 333,4, 55,66, 77,88};

    //                  p1   p2   p3   p0
    float points1[8] = {333,4, 55,66, 77,88, 11,22};

    float p10p12[4] = {0,0, 0,0};

    float sum_pc_t=0;
    QTime tm; tm.start();


    QPointF points[100];
    for(int i=0; i < 100; ++i)
    {
      points[i].setX(i);
      points[i].setY(i*2);
    }
    double * dbl = (double*)(&points[0]);
    for(int i=0; i < 10; ++i)
    {
      qDebug() << dbl[i];
    }
    // proc
    {
      QPointF p0(1,22);
      QPointF p1(333,4);
      QPointF p2(55,66);
      QPointF p3(77,88);
      QPointF sum_pc_t;
      QTime tm; tm.start();
      double t = 0;
      for(int i=0; i < SSE_ITER_N; ++i)
      {
        t += 1e-5;
        QPointF a = p0 + (p1 - p0) * t;
        QPointF b = p1 + (p2 - p1) * t;
        QPointF pb_sub_a = (b - a)*t;
        sum_pc_t += pb_sub_a;
      }
      cout << "sum_pc_t" << sum_pc_t << tm.elapsed();
    }


    float arr[444];
    for(int i=0; i < 444; ++i)
    {
      arr[i] = i%100;
    }
    QTime t;
    t.restart();
    float sse_a = 0;
    for(int i=0; i < 10000000; ++i)
    {
      __m128 x = _mm_load_ps(&arr[0]);
      sse_a += hsum_ps_sse3(x);
    }
    cout << " sse_a" << sse_a << t.elapsed();
    t.restart();

    float proc_a = 0;
    for(int i=0; i < 10000000; ++i)
    {
      proc_a += proc_ps_sse3(&arr[0]);
    }
    cout << " proc_a" << proc_a << t.elapsed();

    float _a =12;
    __m128 a = _mm_set1_ps(_a);
    for(int i=0; i < 1 ; ++i)  // process 4 elements at a time
    {
      __m128 x = _mm_load_ps(&arr[i]);
      __m128 y = _mm_load_ps(&arr[i]);
      __m128 s = _mm_add_ps(_mm_mul_ps(a,x),y);  // a*x + y
      _mm_store_ps(&arr[i], s );
    }

    float arr[4] = {1.1111,2.2222,3.3333,4.4444};

    //float a=1.1111, b=2.2222, c=3.3333, d=4.4444;
    //sseval = _mm_set_ps(a, b, c, d);  // make vector from [ a, b, c, d ]
    //sseval = _mm_setr_ps(a, b, c, d); // make vector from [ d, c, b, a ]
    __m128 sseval = _mm_load_ps(&arr[0]);         // ill-specified here - "a" not float[] ...


    __m128 sss = _mm_set1_ps(0.1234567);
    __m128 mul;
    for(int i=0; i < 10; i++)
    {
      mul = _mm_mul_ps(sseval, sss);
    }
    float result [4];
    _mm_store_ps (result, mul);

}
