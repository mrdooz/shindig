#pragma once

class EffectBase
{
public:
  virtual ~EffectBase() {}
  virtual bool init() = 0;
  virtual bool close() = 0;
  virtual bool render() = 0;
private:
};