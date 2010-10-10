#pragma once

class EffectBase
{
public:
  virtual ~EffectBase() {}
  virtual bool init() = 0;
  virtual bool close() = 0;
  // t is the running time for the first tick, a is the ratio between this and the next tick
  virtual bool update(float t, float dt, int num_ticks, float a) { return true; }
  virtual bool render() = 0;
private:
};