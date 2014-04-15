/*
 * BounceWithSplines.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: Ben O'Leary (benjamin.oleary@gmail.com)
 */

#include "../../include/VevaciousPlusPlus.hpp"

namespace VevaciousPlusPlus
{

  BounceWithSplines::BounceWithSplines( PotentialFunction& potentialFunction,
                                     TunnelingStrategy const tunnelingStrategy,
                                  double const survivalProbabilityThreshold ) :
    TunnelingCalculator( potentialFunction,
                         tunnelingStrategy,
                         survivalProbabilityThreshold )
  {
    // placeholder:
    /**/std::cout << std::endl
    << "Placeholder: "
    << "BounceWithSplines::BounceWithSplines( ... )";
    std::cout << std::endl;/**/
  }

  BounceWithSplines::~BounceWithSplines()
  {
    // This does nothing.
  }

} /* namespace VevaciousPlusPlus */
