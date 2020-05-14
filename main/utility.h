// -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil; mode: c++ -*-
#pragma once

namespace std
{

template< class T >
void swap(T& a, T& b)
{
  T tmp = a;
  a = b;
  b = tmp;
}

}
