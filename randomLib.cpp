

#include <random>

#include <math.h>

#include "randomLib.h"

std::mt19937 engine;

int     SubGetRandom(int min_, int max_)
{
  std::uniform_int_distribution<int> range(min_, max_);
  return range(engine);
}
short   SubGetRandom(short min_, short max_)
{
  std::uniform_int_distribution<short> range(min_, max_);
  return range(engine);
}
double  SubGetRandom(double min_, double max_)
{
  std::uniform_real_distribution<double> range(min_, max_);
  return range(engine);
}

float  SubGetRandom(float min_, float max_)
{
  std::uniform_real_distribution<float> range(min_, max_);
  return range(engine);
}

//template <typename Type>  Type GetRandom(Type min_, Type max_){
//  int real_min = min(min_, max_);
//  int real_max = max(min_, max_);
//
//  return SubGetRandom(real_min, real_max);
//}


void InitRandom()
{
  //ハードウェアアーキテクチャからのランダム  
  std::random_device device;
  engine.seed(device());
}
