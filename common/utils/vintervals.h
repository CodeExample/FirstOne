#ifndef VINTERVALS_H
#define VINTERVALS_H

#include <QVector>

template<typename T>
struct vInterval
{
  vInterval() : t0(0), t1(0) {}
  vInterval(T _t0, T _t1) :
    t0(_t0 < _t1 ? _t0 : _t1),
    t1(_t0 < _t1 ? _t1 : _t0) {}

  vInterval(const vInterval& i) :
    t0(i.t0), t1(i.t1) {}

  inline bool isEmpty()
  {
    return t0 == t1;
  }

  inline vInterval& operator +=(T v)
  {
    if (v > t1)
      t1 = v;
    if (v < t0)
      t0 = v;
    return *this;
  }

  inline T length() const
  {
    T l = t1 - t0;
    return l >= 0 ? l : -l;
  }

  T t0;
  T t1;
};



template<typename T>
class vIntervals
{
public:
  vIntervals() {}

  vIntervals(int reserved)
  {
    pairs.reserve(reserved);
  }


  vIntervals(const vIntervals& i)
  {
    pairs = i.pairs;
  }

  vIntervals(const QVector<T> &p) : pairs(p)
  {
  }

  vIntervals(T t0, T t1)
  {
    pairs << t0 << t1;
  }


  vIntervals<T>& operator +=(const vInterval<T>&);
  vIntervals<T>& operator -=(const vInterval<T>&);
  vIntervals<T>& operator &=(const vInterval<T>&);

  vIntervals<T>& operator +=(const vIntervals<T>&);
  vIntervals<T>& operator -=(const vIntervals<T>&);
  vIntervals<T>& operator &=(const vIntervals<T>&);

  inline double operator [](int i) const
  {
    return pairs[i];
  }

  bool isEmpty() const
  {
    return pairs.isEmpty();
  }

  QVector<T> pairs;

  int findIndex(T d) const;

  int count() const
  {
    return pairs.count();
  }

  double first() const
  {
    return pairs.first();
  }

  double last() const
  {
    return pairs.last();
  }

  bool contains(T t) const;

  void shift(T d);
  void reverse(T t0, T t1);
  void expand(T);

  void mergePairs(T);

  vIntervals inverted() const;
  vIntervals scaled(T src_t0, T src_t1, T dst_t0, T dst_t1) const;
  vIntervals reversed(T t0, T t1) const;
  vIntervals shifted(T d) const;
};


template<typename T>
int vIntervals<T>::findIndex(T t) const
{
  if (pairs.isEmpty())
    return false;

  if (t <= pairs.first())
    return -1;

  if (t >= pairs.last())
    return pairs.count();

  int i0 = 0;
  int i1 = pairs.count()-1;
  while(t < pairs[i0] || t >= pairs[i0+1])
  {
    int t = i0 + i1/2 - i0/2;
    if (t < pairs[t])
      i1 = t;
    else
      i0 = t;
  }

  return i0;
}


template<typename T>
bool vIntervals<T>::contains(T t) const
{
  if (pairs.isEmpty())
    return false;

  if (t < pairs.first() || t > pairs.last())
    return false;

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i+1);

    if (t < t0)
      return false;

    if (t >= t0 && t <= t1)
      return true;
  }

  return false;
}


template<typename T>
vIntervals<T> vIntervals<T>::inverted() const
{
  vIntervals<T> intervals;
  intervals += vInterval<T>(pairs.first(), pairs.last());
  intervals -= *this;
  return intervals;
}


template<typename T>
void vIntervals<T>::shift(T d)
{
  for(int i=0; i < pairs.count(); ++i)
  {
    pairs[i] += d;
  }
}


template<typename T>
vIntervals<T> vIntervals<T>::shifted(T d) const
{
  QVector<T> p;
  for(int i=0; i < pairs.count(); ++i)
  {
    p << (pairs[i] + d);
  }
  return p;
}


template<typename T>
vIntervals<T>& vIntervals<T>::operator +=(const vInterval<T>& interval)
{
  T add_t0 = interval.t0;
  T add_t1 = interval.t1;

  if (pairs.isEmpty() || add_t0 > pairs.last())
  {
    pairs.append(add_t0);
    pairs.append(add_t1);
    return *this;
  }

  if (add_t1 < pairs.first())
  {
    pairs.push_front(add_t1);
    pairs.push_front(add_t0);
    return *this;
  }

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i+1);

    if (add_t0 >= t0 && add_t1 <= t1)
      return *this;

    if (t0 < add_t0 && t1 >= add_t0)
    {
      add_t0 = t0;
    }
    if (t1 > add_t1 && t0 <= add_t1)
    {
      add_t1 = t1;
    }
  }

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i+1);

    if (t0 >= add_t0 && t1 <= add_t1)
    {
      pairs.removeAt(i);
      pairs.removeAt(i);
      i -= 2;
    }
  }

  if (pairs.isEmpty())
  {
    pairs.push_back(add_t0);
    pairs.push_back(add_t1);
    return *this;
  }

  if (add_t1 < pairs.first())
  {
    pairs.push_front(add_t1);
    pairs.push_front(add_t0);
    return *this;
  }

  if (add_t0 > pairs.last())
  {
    pairs.push_back(add_t0);
    pairs.push_back(add_t1);
    return *this;
  }

  for(int i=0; i < pairs.count()-2; i += 2)
  {
    T t1 = pairs.at(i+1);
    T next_t0 = pairs.at(i+2);

    if (add_t0 > t1 && add_t1 < next_t0)
    {
      pairs.insert(i+2, add_t1);
      pairs.insert(i+2, add_t0);
      return *this;
    }
  }


  return *this;
}




template<typename T>
vIntervals<T>& vIntervals<T>::operator &=(const vInterval<T>& interval)
{
  if (pairs.isEmpty())
    return *this;

  T and_t0 = interval.t0;
  T and_t1 = interval.t1;

  if (and_t0 > pairs.last())
  {
    pairs.clear();
    return *this;
  }

  if (and_t1 < pairs.first())
  {
    pairs.clear();
    return *this;
  }

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i+1);

    if (t0 > and_t1 || t1 < and_t0)
    {
      pairs.removeAt(i);
      pairs.removeAt(i);
      i -= 2;
    }
  }

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i + 1);

    if (and_t0 >= t0 && and_t1 <= t1)
    {
      pairs[i] = and_t0;
      pairs[i+1] = and_t1;
      return *this;
    }

    if (and_t0 > t0 && and_t0 < t1)
    {
      pairs[i] = and_t0;
    }

    if (and_t1 > t0 && and_t1 < t1)
    {
      pairs[i+1] = and_t1;
      return *this;
    }

  }

  return *this;
}



template<typename T>
vIntervals<T>& vIntervals<T>::operator -=(const vInterval<T>& interval)
{
  if (pairs.isEmpty())
    return *this;

  T sub_t0 = interval.t0;
  T sub_t1 = interval.t1;

  if (sub_t0 > pairs.last())
    return *this;

  if (sub_t1 < pairs.first())
    return *this;

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i+1);

    if (t0 > sub_t0-0.01 && t1 < sub_t1+0.01)
    {
      pairs.removeAt(i);
      pairs.removeAt(i);
      i -= 2;
    }
  }

  for(int i=0; i < pairs.count(); i += 2)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i + 1);

    if (sub_t0 > t0 && sub_t1 < t1)
    {
      pairs.insert(i+1, sub_t1);
      pairs.insert(i+1, sub_t0);
      return *this;
    }

    if (sub_t0 > t0 && sub_t0 < t1)
    {
      pairs[i + 1] = sub_t0;
    }

    if (sub_t1 > t0 && sub_t1 < t1)
    {
      pairs[i] = sub_t1;
      return *this;
    }

  }

  return *this;
}


template<typename T>
vIntervals<T>& vIntervals<T>::operator +=(const vIntervals<T>& in_intervals)
{
  for(int i=0; i < in_intervals.count(); i += 2)
  {
    (*this) += vInterval<T>(in_intervals[i], in_intervals[i+1]);
  }
  return *this;
}


template<typename T>
vIntervals<T>& vIntervals<T>::operator -=(const vIntervals<T>& in_intervals)
{
  for(int i=0; i < in_intervals.count(); i += 2)
  {
    (*this) -= vInterval<T>(in_intervals[i], in_intervals[i+1]);
  }
  return *this;
}

template<typename T>
vIntervals<T>& vIntervals<T>::operator &=(const vIntervals<T>& in_intervals)
{
  vIntervals<T> res;
  for(int i=0; i < in_intervals.count(); i += 2)
  {
    vIntervals<T> c = *this;
    c &= vInterval<T>(in_intervals[i], in_intervals[i+1]);
    res += c;
  }
  pairs = res.pairs;
  return *this;
}


template<typename T>
vIntervals<T> vIntervals<T>::scaled(T src_t0, T src_t1, T dst_t0, T dst_t1) const
{
  vIntervals<T> dst_i;
  QVector<T> &dst_pairs = dst_i.pairs;
  double src_d = src_t1 - src_t0;
  double dst_d = dst_t1 - dst_t0;
  foreach(T src_t, pairs)
  {
    T dt = (src_t - src_t0)/src_d;
    T dst_t = dst_t0 + dst_d*dt;
    dst_pairs << dst_t;
  }
  return dst_i;
}


template<typename T>
vIntervals<T> vIntervals<T>::reversed(T t0, T t1) const
{
  QVector<T> p;
  for(T t : pairs)
  {
    p << (t0 + t1 - t);
  }
  return vIntervals<T>(p);
}

template<typename T>
void vIntervals<T>::reverse(T t0, T t1)
{
  for(T t : pairs)
  {
    t = (t0 + t1 - t);
  }
}

template<typename T>
void vIntervals<T>::mergePairs(T min_dist)
{
  for(int i=0; i < pairs.count()-1; ++i)
  {
    T t0 = pairs.at(i);
    T t1 = pairs.at(i+1);
    if (t1-t0 > min_dist)
      continue;

    pairs.removeAt(i+1);
    pairs.removeAt(i);
    if (i > 0)
      --i;
  }
}

template<typename T>
void vIntervals<T>::expand(T s)
{
  for(int i=0; i < pairs.count(); i += 2)
  {
    pairs[i] -= s;
    pairs[i+1] += s;
  }

  for(int i=1; i < pairs.count()-1;)
  {
    if (pairs[i-1] > pairs[i])
    {
      pairs.removeOne(i-1);
      pairs.removeOne(i-1);
    } else {
      i += 2;
    }
  }
}

#endif // VINTERVALS_H
