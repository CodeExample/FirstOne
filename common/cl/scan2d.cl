/*
 *
 * OpenCl procedures
 *
 */
#define GROUP_SIZE 64
#define MAX_UINT   0xffffffff
#define NULL_THICK 0.00001

static float2 unitVector(float2 p, float d)
{
  float len = sqrt(p.x*p.x + p.y*p.y);
  return (float2)(d*p.x/len, d*p.y/len);
}


static float2 normalVector(float2 p0, float2 p1, float d)
{
  float dx = d*(p1.x - p0.x);
  float dy = d*(p1.y - p0.y);
  return (float2)(dy, -dx);
}


static float fixPxByThreshold(float px, const float threshold)
{
  if (px < threshold)
    return 0;

  return (px - threshold)/(1 - threshold);
}


static uint4 pixelAt(__read_only image2d_t image, float2 p)
{
  sampler_t smp = CLK_ADDRESS_REPEAT;
  uint4 rgba = read_imageui(image, smp, p);
  return rgba;
}


static int maskAt(__read_only image2d_t image, float2 p)
{
  sampler_t smp = CLK_ADDRESS_CLAMP_TO_EDGE;
  uint4 rgba = read_imageui(image, smp, p);
  return rgba.r;
}

static float2 mapTx(const __global float *tx, float2 p)
{
  return (float2)(p.x*tx[0] + p.y*tx[1] + tx[2],
        p.x*tx[3] + p.y*tx[4] + tx[5]);
}

static uint pixelAtUi(__read_only image2d_t image, float2 p)
{
  sampler_t smp = CLK_ADDRESS_REPEAT;// |  CLK_FILTER_LINEAR;
  uint4 rgba = read_imageui(image, smp, p);
  return rgba[0] + (rgba[1] << 8) + (rgba[2] << 16) + (rgba[3] << 24);
}


static bool contains(__read_only image2d_t image, float2 p)
{
  int w = get_image_width(image);
  int h = get_image_height(image);
  return p.x >= 0 && p.x < w &&
      p.y >= 0 && p.y < h;
}


static int intersect(float2 t_pt1, float2 t_pt2, float2 l_pt1, float2 l_pt2)
{
    const float2 a = t_pt2 - t_pt1;
    const float2 b = l_pt1 - l_pt2;
    const float2 c = t_pt1 - l_pt1;

    const float denominator = a.y * b.x - a.x * b.y;
    if (denominator == 0 || !isfinite(denominator))
        return 0;//NoIntersection;

    const float reciprocal = 1 / denominator;
    const float na = (b.y * c.x - b.x * c.y) * reciprocal;
    //if (intersectionPoint)
    //    *intersectionPoint = pt1 + a * na;

    if (na < 0 || na > 1)
        return 0;//UnboundedIntersection;

    const float nb = (a.x * c.y - a.y * c.x) * reciprocal;
    if (nb < 0 || nb > 1)
        return 0;//UnboundedIntersection;

    return 1;//BoundedIntersection;
}


__kernel void createStroke2d(const __global float2 *in_track,
                             __global float2 *out_track,
                             __global float2 *out_normals,
                             __read_only image2d_t image,
                             __global float *tx,
                             const uint size,
                             const float thick,
                             const float bsrlf,
                             const float threshold,
                             const float thick_base)
{
  uint global_id = get_global_id(0);

  float2 in_norm = out_normals[global_id];
  float2 curr = in_track[global_id];

  if (fast_length(in_norm) == 0)
  {
    out_track[global_id] = curr;
    return;
  }

  float2 prev = global_id > 0 ? in_track[global_id - 1] : curr;
  float2 next = global_id < size-1 ? in_track[global_id + 1] : curr;

  float2 prev_d = global_id > 0 && fast_length(prev) != 0
      ? curr - prev : next - curr;

  float2 next_d = global_id < size-1 && fast_length(next) != 0
      ? next - curr : curr - prev;

  float2 norm_0 = (float2)(-prev_d.y, prev_d.x);
  float2 norm_1 = (float2)(-next_d.y, next_d.x);

  float2 norm = norm_0 + norm_1;
  norm /= 2;
  float2 u_norm = unitVector(norm, 1);
  float2 t_norm = curr + u_norm;
  float2 b_norm = curr - u_norm;

  float2 curr_img = mapTx(tx, curr);

  uint4 c_rgba = pixelAt(image, curr_img);
  float c_px = fixPxByThreshold((255. - (c_rgba.x + c_rgba.y + c_rgba.z)/3.)/255., threshold);

  if (bsrlf != 0)
  {
    float a=1;
    float m_px = c_px;//0.5 - c_px;
    curr += u_norm*bsrlf*m_px*a;
    c_rgba = pixelAt(image, curr_img);
    c_px = fixPxByThreshold((255. - (c_rgba.x + c_rgba.y + c_rgba.z)/3.)/255., threshold);
  }

  float2 t_norm_img = {t_norm.x*tx[0] + t_norm.y*tx[1] + tx[2],
                       t_norm.x*tx[3] + t_norm.y*tx[4] + tx[5]};
  float2 b_norm_img = {b_norm.x*tx[0] + b_norm.y*tx[1] + tx[2],
                       b_norm.x*tx[3] + b_norm.y*tx[4] + tx[5]};

  uint4 t_rgba = pixelAt(image, t_norm_img);
  float t_px = c_px + fixPxByThreshold((255. - (t_rgba.x + t_rgba.y + t_rgba.z)/3.)/255., threshold);
  t_px /= 2.;

  uint4 b_rgba = pixelAt(image, b_norm_img);
  float b_px = c_px + fixPxByThreshold((255. - (b_rgba.x + b_rgba.y + b_rgba.z)/3.)/255., threshold);
  b_px /= 2.;

  float2 t_pnt = curr + u_norm*t_px;
  float2 b_pnt = curr - u_norm*b_px;

  out_track[global_id] = (t_pnt + b_pnt)/2;
  float2 w_norm = u_norm*distance(t_pnt, b_pnt)/2;
  out_normals[global_id] = (float2)(fabs(w_norm.x), fabs(w_norm.y));
}


__kernel void smoothStroke(
    const __global float2 *track,
    const __global float2 *normals,
    __global float2 *out_track,
    __global float2 *out_normals,
    const uint size, const uint sub_size)
{
  uint global_id = get_global_id(0);
  float2 track_pnt = track[global_id];
  float2 u_norm = normals[global_id];

  if (fast_length(u_norm)==0)
  {
    out_track[global_id] = track_pnt;
    out_normals[global_id] = u_norm;
    return;
  }

  if (sub_size > 0)
  {
    uint i0 = global_id;
    uint i1 = global_id;
    uint min_i0 = global_id < sub_size ? 0 : global_id - sub_size;
    uint max_i1 = global_id >= size - sub_size ? size - 1 : global_id + sub_size;

    while(fast_length(normals[i0 - 1]) != 0 && (i0 > min_i0))
    {
      --i0;
    }
    while(fast_length(normals[i1 + 1]) != 0 && (i1 < max_i1))
    {
      ++i1;
    }

    uint l = global_id - i0;
    uint r = i1 - global_id;
    uint m = l < r ? l : r;
    if (m > 1)
    {
      i0 = global_id - m;
      i1 = global_id + m;

      float2 p_sum=(float2)(0,0);
      float2 u_sum=(float2)(0,0);
      float nn=0;
      for(uint i=i0; i<=i1; ++i)
      {
        ++nn;
        p_sum += track[i];
        u_sum += normals[i];
      }

      if (nn > 0)
      {
        track_pnt = p_sum/nn;
        u_norm = u_sum/nn;
      }
    }
  }

  out_track[global_id] = track_pnt;
  out_normals[global_id] = u_norm;
}


__kernel void createVBOBuffer2d(
    const __global float2 *track,
    const __global float2 *normals,
    __global float *vbo_buff,
    const float thick,
    const float thick_base,
    const uint size)
{
  uint global_id = get_global_id(0);
  float2 track_pnt = track[global_id];
  float2 u_norm = normals[global_id];
  float l = length(u_norm);
  if (l < thick_base && l > NULL_THICK)
  {
    u_norm.x = thick_base*u_norm.x/l;
    u_norm.y = thick_base*u_norm.y/l;
  }
  u_norm *= thick;

  vbo_buff[6*global_id +0] = track_pnt.x + u_norm.x;
  vbo_buff[6*global_id +1] = track_pnt.y + u_norm.y;
  vbo_buff[6*global_id +2] = 0;
  vbo_buff[6*global_id +3] = track_pnt.x - u_norm.x;
  vbo_buff[6*global_id +4] = track_pnt.y - u_norm.y;
  vbo_buff[6*global_id +5] = 0;
}



__kernel void createRoundCups(
    const __global uint *in_intervals,
    const __global float2 *track,
    const __global float2 *normals,
    __global float *vbo_buff,
    const float thick,
    const float thick_base,
    const uint size
    )
{
  uint global_id = get_global_id(0);
  if (global_id >= size)
    return;

  uint index = in_intervals[global_id];

  float2 t_pnt = track[index];
  float2 norm = normals[index];
  float l = length(norm);
  if (l < thick_base)
  {
    l = thick_base;
  }

  l *= thick;


  for(int i=0; i<37; ++i)
  {
    float a = 3.14*float(i)/18.;
    float2 c = (float2)(sin(a), cos(a));
    vbo_buff[6*(37*global_id+i) +0] = t_pnt.x + l*c.x;
    vbo_buff[6*(37*global_id+i) +1] = t_pnt.y + l*c.y;
    vbo_buff[6*(37*global_id+i) +2] = 0;
    vbo_buff[6*(37*global_id+i) +3] = t_pnt.x;
    vbo_buff[6*(37*global_id+i) +4] = t_pnt.y;
    vbo_buff[6*(37*global_id+i) +5] = 0;
  }
}



__kernel void fetchIntervals(
    const __global float2 *normals,
    __global uint *out_intervals,
    __global int *interval_ix,
    const uint size, const float quad_th
    )
{
  uint global_id = get_global_id(0);

  float prev_w = quad_th+1;
  if (global_id > 0)
  {
    prev_w = fast_length(normals[global_id-1]);
  }

  double curr_w=fast_length(normals[global_id]);
  if ((prev_w >= quad_th && curr_w < quad_th) ||
      (prev_w < quad_th && curr_w > quad_th))
  {
    int interval_i = atomic_inc(&interval_ix[0]);
    out_intervals[interval_i] = global_id;
  }
}



__kernel void fetchIntervalsNull(
    const __global float2 *normals,
    __global uint *out_intervals,
    __global int *interval_ix,
    const uint size
    )
{
  uint global_id = get_global_id(0);

  float prev_w = 1;
  if (global_id > 0)
  {
    prev_w = fast_length(normals[global_id-1]);
  }
  float next_w = 1;
  if (global_id < size-1)
  {
    next_w = fast_length(normals[global_id+1]);
  }

  float curr_w=fast_length(normals[global_id]);

  if (prev_w == 0 && curr_w > 0)
  {
    int interval_i = atomic_inc(&interval_ix[0]);
    out_intervals[interval_i] = global_id;
  }
  if (curr_w > 0 && next_w == 0)
  {
    int interval_i = atomic_inc(&interval_ix[0]);
    out_intervals[interval_i] = global_id-1;
  }
}



__kernel void createLinearTrack(
    __global float2 *out_track,
    __global float2 *out_normals,
    __global uint * indexes,
    const float interval_px,
    const int steps_on_px,
    const int width,
    const int height)
{
  uint global_id = get_global_id(0);

  uint horz_cnt = width*steps_on_px + 1;

  uint row = global_id/horz_cnt;
  uint col = global_id%horz_cnt;

  float dx = 1/float(steps_on_px);
  float x = col*dx;
  float y = row*interval_px + sin(x/20);
  out_track[global_id] = (float2)(x, y);
  if (global_id%horz_cnt < horz_cnt-1)
  {
    out_normals[global_id] = (float2)(1,0);
    indexes[global_id] = global_id/horz_cnt;
  } else {
    out_normals[global_id] = (float2)(0,0);
    indexes[global_id] = MAX_UINT;
  }

  }



static float2 point_on_line(float2 p0, float2 p1, float t)
{
  float2 dp = (float2)(p0.x + (p1.x - p0.x)*t,
                       p0.y + (p1.y - p0.y)*t);
  return dp;
}


__kernel void createLinearCellsTrack(
    const __global float2 *in_track,
    __global float2 *out_cells_track,
    __global float2 *out_cells_normals,
    const uint size,
    const int steps_on_px,
    const int width_px,
    const int cell_width_px,
    const int cell_italic_px,
    const int cell_h,
    const int shorting)
{
  uint global_id = get_global_id(0);

  out_cells_track[global_id] = (float2)(0, 0);
  out_cells_normals[global_id] = (float2)(0, 0);

  int horz_length = width_px*steps_on_px + 1;
  int cells_horz_cnt = width_px*steps_on_px/cell_width_px + 2;
  int cells_horz_length = cells_horz_cnt*cell_h;

  int cell_row = global_id/cells_horz_length;
  int cell_col = global_id%cells_horz_length;

  float cell_i = cell_col%cell_h;
  if (cell_i < shorting || cell_i >= cell_h-shorting)
    return;

  float perc_pos = cell_i/float(cell_h-2);

  int italic_dx_on_row = (cell_italic_px*cell_row)%cell_width_px;
  out_cells_track[global_id] = (float2)(cell_italic_px, italic_dx_on_row);

  int cell_i0_px = cell_width_px*int(cell_col/cell_h) + italic_dx_on_row - cell_width_px;

  cell_i = cell_i0_px + float(cell_italic_px)*perc_pos;

  if (cell_i < 0 || cell_i >= horz_length-2)
    return;
  out_cells_track[global_id] = (float2)(0, 2);

  int i0 = int(cell_i);
  int i1 = (i0 + 1);

  float t = cell_i - int(cell_i);

  i0 += cell_row*horz_length;
  i1 += cell_row*horz_length;

  if (uint(i1 + horz_length) >= size)
    return;
  out_cells_track[global_id] = (float2)(0, 3);

  float2 tp = point_on_line(in_track[i0], in_track[i1], t);
  float2 bp = point_on_line(in_track[i0+horz_length], in_track[i1+horz_length], t);
  float2 cp = point_on_line(tp, bp, perc_pos);

  out_cells_track[global_id] = cp;
  out_cells_normals[global_id] = (float2)(1, 0);
}


__kernel void applyEdgesToStrokes(const __global float2 *smooth_track,
                                  __global float2 *smooth_normals,
                                  __global uint *indexes,
                                  const __global float2 *above_edges,
                                  const __global uint *edges_indexes,
                                  const uint edges_size,
                                  __read_only image2d_t edges_map,
                                  const float2 tl)

{
  uint track_ix = get_global_id(0);

  if (track_ix == 0)
    return;

  float2 norm0 = smooth_normals[track_ix];
  float2 norm1 = smooth_normals[track_ix-1];
  if (norm0.y == 0 || norm1.y == 0)
    return;

  float2 t0 = smooth_track[track_ix-1];
  float2 t1 = smooth_track[track_ix];

  uint px0 = 0xffffffff;
  uint px1 = 0xffffffff;

  float2 s0_xy = t0;
  float2 e0_xy = s0_xy + (float2)(5,5);

  if (!contains(edges_map, e0_xy-tl))
    return;

  px0 = pixelAtUi(edges_map, e0_xy-tl);

  float2 s1_xy = t1;
  float2 e1_xy = s1_xy + (float2)(5,5);
  if (!contains(edges_map, e1_xy-tl))
    return;

  px1 = pixelAtUi(edges_map, e1_xy-tl);

 if (px0 == 0xffffffff && px1 == 0xffffffff)
    return;

  uint px = px0 < px1 ? px0 : px1;
  uint s=15;
  uint edge_ix0 = px < s+1 ? 1 : px-s;
  uint edge_ix1 = px > edges_size-1-s ? edges_size-1 : px+s;

  for(uint edge_ix = edge_ix0; edge_ix <= edge_ix1; ++edge_ix)
  {
    float2 e0_xy=(float2)(above_edges[edge_ix-1].x, above_edges[edge_ix-1].y);
    float2 e1_xy=(float2)(above_edges[edge_ix].x, above_edges[edge_ix].y);

    if (intersect(s0_xy, s1_xy, e0_xy, e1_xy))
    {
      uint edge_row_ix = 1 + edges_indexes[edge_ix];
      uint track_row_ix = 1 + indexes[track_ix];
      smooth_normals[track_ix].x = -(float)(edge_row_ix);
      smooth_normals[track_ix].y = -(float)(track_row_ix);
      break;
    }
  }
}


__kernel void clearCells(__global float2 *smooth_normals)
{
  uint track_ix = get_global_id(0);

  if (track_ix == 0)
    return;

  if (smooth_normals[track_ix].x >= 0 &&
      smooth_normals[track_ix].y >= 0)
    return;

  uint max_track_ix = get_work_dim();
  uint d = 1;
  uint track_ix0 = track_ix < d ? 0 : track_ix-d;
  uint track_ix1 = track_ix > max_track_ix-1-d ? max_track_ix-d : track_ix+d;
  for(uint t_ix=track_ix0; t_ix <= track_ix1; ++t_ix)
  {
    smooth_normals[t_ix] = (float2)(0,0);
  }
}


__kernel void applyCellsToStrokes(const __global uint8 * cells_map,
                                  __global float2 *smooth_track,
                                  __global float2 *smooth_normals,
                                  const uint cells_size)


{
  uint cell_ix = get_global_id(0);

  if (cell_ix >= cells_size)
    return;

  uint8 c = cells_map[cell_ix];
  if (!c.s0 || !c.s1)
    return;

  bool swap = c.s0 > c.s1;

  uint i0 = swap ? c.s1-1 : c.s0-1;
  uint i1 = swap ? c.s0-1 : c.s1-1;

  float2 p0 = smooth_track[i0];
  float2 p1 = smooth_track[i1];

  float2 p2,p3,p4,p5;
  bool has_p23=false;
  bool has_p45=false;
  if (c.s2 && c.s3)
  {
    has_p23 = true;
    uint i2 = swap ? c.s3-1 : c.s2-1;
    uint i3 = swap ? c.s2-1 : c.s3-1;
    p2 = smooth_track[i2];
    p3 = smooth_track[i3];
    p2 = p0 + (p2 - p0)/2;
    p3 = p1 + (p3 - p1)/2;
  }

  if (c.s4 && c.s5)
  {
    has_p45 = true;
    uint i4 = swap ? c.s5-1 : c.s4-1;
    uint i5 = swap ? c.s4-1 : c.s5-1;
    p4 = smooth_track[i4];
    p5 = smooth_track[i5];
    p4 = p0 + (p4 - p0)/2;
    p5 = p1 + (p5 - p1)/2;
  }

  if (!has_p23 && !has_p45)
    return;

  if (has_p23 && !has_p45)
  {
    p4 = p0 - (p2 - p0);
    p5 = p1 - (p3 - p1);
  }

  if (!has_p23 && has_p45)
  {
    p2 = p0 - (p4 - p0);
    p3 = p1 - (p5 - p1);
  }

  float track_d = i1 - i0;

  float2 pp0,pp1;
  if (fast_length(p2 - p5) > fast_length(p3 - p4))
  {
    pp0 = p5;
    pp1 = p2;
  } else {
    pp0 = p4;
    pp1 = p3;
  }

  float2 dp = (pp1 - pp0)/track_d;

  for(uint track_ix=i0; track_ix <= i1; ++track_ix)
  {
    float2 norm=smooth_normals[track_ix];
    float l = fast_length(norm);

    float z = track_ix - i0;
    float2 p = pp0 + dp*z;

    if (track_ix > i0 && track_ix < i1)
    {
      smooth_track[track_ix] = p;
    }

    if (z < 3*track_d/7 || z > 4*track_d/7)
    {
      smooth_normals[track_ix] = (float2)(0,0);
    } else {
      if (track_ix == i0)
      {
        norm = normalVector(smooth_track[track_ix], smooth_track[track_ix+1], 1);
      } else {
        norm = normalVector(smooth_track[track_ix-1], smooth_track[track_ix], 1);
      }
      smooth_normals[track_ix] = unitVector(norm, l);
    }
  }
}

typedef struct
{
  float2 p0, p1, p2;
} Bicubic;



static float2 bic_at(float t, float2 p0, float2 p1, float2 p2)
{
  float t0 = (1-t)*(1-t);
  float t1 = 2*t*(1-t);
  float t2 = t*t;

  return (float2)(t0*p0.x + t1*p1.x + t2*p2.x,
                  t0*p0.y + t1*p1.y + t2*p2.y);
}




__kernel void applyCellsDottedLine(
    const __global uint *in_intervals,
    __global float2 *normals,
    const float threshold,
    const float cell_size)
{
  uint global_id = get_global_id(0);

  if (global_id%2 != 0)
    return;

  uint intrv_ix0 = in_intervals[global_id];
  uint intrv_ix1 = in_intervals[global_id+1];

  if (intrv_ix0 >= intrv_ix1)
    return;

  for(uint i=intrv_ix0; i<=intrv_ix1;)
  {
    uint i0=i;
    float c_sum=0;
    while(i <= intrv_ix1)
    {
      float2 n = normals[++i];
      float d = length(n);
      if (d == 0)
        break;
      c_sum += d;
    }

    uint i1=i-1;

    if (i1 - i0 > 2)
    {
      c_sum /= i1 - i0-2;
    }

    if (c_sum <= threshold)
    {
      int d = 0.5*(i1 - i0)*cell_size;
      if (d > 0)
      {
        for(int j=0; j <= d; ++j)
        {
          normals[i0 - j] = float2(0,0);
          normals[i0 + j] = float2(0,0);
          normals[i1 - j] = float2(0,0);
          normals[i1 + j] = float2(0,0);
        }
      }
    }
  }

}



__kernel void applyDottedLine(
    const __global uint *in_intervals,
    __global float2 *normals,
    const float threshold,
    const float white,
    const float black,
    const int steps_on_px,
    Bicubic bic)
{
  uint global_id = get_global_id(0);

  if (global_id%2 != 0)
    return;

  uint intrv_ix0 = in_intervals[global_id];
  uint intrv_ix1 = in_intervals[global_id+1];

  if (intrv_ix0 >= intrv_ix1)
    return;

  float c_sum=0;
  for(uint i=intrv_ix0; i <= intrv_ix1; )
  {
    while(c_sum < threshold && i <= intrv_ix1)
    {
      float2 n = normals[i++];
      float d = length(n);
      float2 dot = bic_at(1. - d, bic.p0, bic.p1, bic.p2);
      c_sum += black*dot.x*dot.x;
    }
    while(c_sum > 0 && i <= intrv_ix1)
    {
      float2 n = normals[i];
      float d = length(n);
      float2 dot = bic_at(d, bic.p0, bic.p1, bic.p2);
      c_sum -= white*dot.y*dot.y;
      n = unitVector(n, 1);
      normals[i++] = float2(n.x*NULL_THICK*0.1, n.y*NULL_THICK*0.1);
    }
  }
}



__kernel void applyMask(const __global float2 *in_track,
                             __global float2 * normals,
                             __read_only image2d_t mask_image,
                             __global float *tx,
                             const int steps_on_px,
                             const uint size)
{
  uint global_id = get_global_id(0);

  float2 in_norm = normals[global_id];
  if (fast_length(in_norm) == 0)
    return;

  float2 curr_xy = mapTx(tx, in_track[global_id]);

  int mask_px = maskAt(mask_image, curr_xy);

  double m = mask_px;
  m = (255. - m)/255.;

  double d = double(steps_on_px+2)*m + 0.5;
  uint m_steps = d;

  uint i0 = global_id < m_steps ? 0 : global_id - m_steps;
  uint i1 = global_id > size - m_steps ? size-1 : global_id + m_steps;

  bool success = false;
  for(uint i=i0; i <= i1; ++i)
  {
    float2 prev_xy = mapTx(tx, in_track[i]);
    int prev_px = maskAt(mask_image, prev_xy);
    if (prev_px < 1)
    {
      success = true;
      break;
    }
  }

  if (!success)
    normals[global_id] = float2(0,0);
}



__kernel void makeRoundCups(
    const __global uint *in_intervals,
    const __global float2 *track,
    __global float2 *normals,
    const float thick,
    const float thick_base,
    const uint intervals_size,
    const uint normals_size,
    const uint in_round_steps
    )
{
  uint round_steps = in_round_steps;

  uint global_id = get_global_id(0);

  uint global_i0 = global_id > 0 ? global_id-1 : 0;
  uint global_i1 = global_id < normals_size-1 ? global_id + 1 : normals_size-1;

  uint l_d = (in_intervals[global_id] - in_intervals[global_i0])/2;
  uint r_d = (in_intervals[global_i1] - in_intervals[global_id])/2;

  uint lr_d = r_d - l_d;

  if (lr_d < round_steps)
  {
    round_steps = lr_d;
  }

  uint index = in_intervals[global_id];

  uint index0;
  uint index1;

  if (l_d > round_steps/2 && r_d > round_steps/2)
  {
    index0 = index - round_steps/2;
  } else {
    if (l_d < round_steps)
    {
      index0 = index - l_d;
    } else {
      index0 = index + r_d - round_steps;
    }
  }

  index1 = index0 + round_steps;

  uint nn = index1 - index0;
  float s = 1./float(nn);
  float2 norm = normals[index];


  float2 curr = normals[index];
  float2 prev = normals[index-1];
  float l_curr = length(curr);
  float l_prev = length(prev);

  // ---||||
  if (l_prev == 0 && l_curr != 0)
  {
    for(uint i=0; i < nn; ++i)
    {
      float d = s*float(i);
      normals[index0 + i] = (float2)(norm.x*d, norm.y*d);
    }
  }
}



__kernel void breakBasrelief(
    __global float2 *normals,
    const uint size)
{
  uint global_id = get_global_id(0);

  if (global_id <= 2)
    return;

  if (global_id >= size-3)
    return;

  uint i0 = global_id - 2;
  uint i1 = global_id + 2;

  float l_norm = length(normals[global_id]);
  float l_prev = length(normals[i0]);
  float l_next = length(normals[i1]);

  if (l_prev > 0)
  {
    float d = l_norm - l_prev;
    if (fabs(d) > 0.5)
    {
      normals[global_id] = float2(0,0);
      return;
    }
  }

  if (l_next > 0)
  {
    float d = l_norm - l_next;
    if (fabs(d) > 0.5)
    {
      normals[global_id] = float2(0,0);
      return;
    }
  }

}

