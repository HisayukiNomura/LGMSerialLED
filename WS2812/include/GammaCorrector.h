#pragma once

#include <iostream>
#include <cmath>
#include <unordered_map>
#include <cstdint>
#include <iomanip>

class GammaCorrector
{
  public:
	GammaCorrector(float gamma) :
		gamma_(gamma) {}

	uint32_t correct(uint32_t grbColor);

  private:
	float gamma_;
	std::unordered_map<uint32_t, uint32_t> cache_;

	uint8_t gammaCorrect(uint8_t value) const;
};
