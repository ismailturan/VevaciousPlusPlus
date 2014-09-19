/*
 * SplinePotential.hpp
 *
 *  Created on: Jun 10, 2014
 *      Author: Ben O'Leary (benjamin.oleary@gmail.com)
 */

#ifndef SPLINEPOTENTIAL_HPP_
#define SPLINEPOTENTIAL_HPP_

#include "CommonIncludes.hpp"
#include "PotentialEvaluation/PotentialFunction.hpp"
#include "PathParameterization/TunnelPath.hpp"

namespace VevaciousPlusPlus
{

  class SplinePotential
  {
  public:
    SplinePotential( PotentialFunction const& potentialFunction,
                     TunnelPath const& tunnelPath,
                     size_t const numberOfPotentialSegments );
    ~SplinePotential();


    // This is true if there was an energy barrier resolved at the given
    // resolution.
    bool EnergyBarrierResolved() const{ return energyBarrierResolved; }

    // This is true if the path false minimum (where the false vacuum rolls to
    // along the path based on the given resolution) has a higher potential
    // than the path true minimum.
    bool TrueVacuumLowerThanPathFalseMinimum() const
    { return trueVacuumLowerThanPathFalseMinimum; }

    // This returns the value of the potential at auxiliaryValue, by finding
    // the correct segment and then returning its value at that point.
    double operator()( double auxiliaryValue ) const;

    // This returns the value of the first derivative of the potential at
    // auxiliaryValue, by finding the correct segment and then returning its
    // slope at that point.
    double FirstDerivative( double const auxiliaryValue ) const;

    // This returns the value of the second derivative of the potential at
    // the false vacuum end of the path.
    double SecondDerivativeAtFalseVacuum() const
    { return ( firstSegmentQuadratic + firstSegmentQuadratic ); }

    // This returns the value of the first derivative of the potential at
    // (definiteOvershootAuxiliary + differenceFromMaximumAuxiliary), assuming
    // that it is in the implicit final segment.
    double FirstDerivativeNearPathPanic(
                            double const differenceFromMaximumAuxiliary ) const
    { return ( 2.0 * differenceFromMaximumAuxiliary * lastSegmentQuadratic ); }

    // This returns the value of the second derivative of the potential at
    // (definiteOvershootAuxiliary + differenceFromMaximumAuxiliary), assuming
    // that it is in the implicit final segment.
    double SecondDerivativeNearPathPanic() const
    { return lastSegmentQuadratic; }

    double DefiniteUndershootAuxiliary() const
    { return definiteUndershootAuxiliary; }

    double DefiniteOvershootAuxiliary() const
    { return definiteOvershootAuxiliary; }

    double StartOfFinalSegment() const{ return startOfFinalSegment; }

    double SizeOfFinalSegment() const{ return auxiliaryStep; }

    // This is for debugging.
    std::string AsDebuggingString() const;


  protected:
    // There are two flags for possible problems: if there did not seem to be
    // any energy barrier between the ends of the path, then
    // energyBarrierResolved will be set to false, and if the path false
    // minimum (where the false vacuum would roll to along the path at the
    // given resolution) is actually deeper than the given true vacuum,
    // trueVacuumLowerThanPathFalseMinimum is set to false.
    bool energyBarrierResolved;
    bool trueVacuumLowerThanPathFalseMinimum;
    double auxiliaryStep;
    double inverseOfAuxiliaryStep;
    // There are ( numberOfPotentialSegments - 2 ) normal segments, which are
    // taken as linear functions, with the false vacuum side potential of each
    // segment stored in potentialValues[ i - 2 ] and the slope in
    // firstDerivatives[ i - 2 ] for the ith segment (starting from 1). The
    // segment at the start and the segment at the end are taken as quadratics
    // without linear terms, parameterized by firstSegmentQuadratic for the
    // segment at the false vacuum and finalPotential and lastSegmentQuadratic
    // for the segment at the true vacuum, with that segment being treated as
    // being based at the end of the path (truncated at the first path panic
    // minimum).
    size_t numberOfNormalSegments;
    std::vector< double > potentialValues;
    std::vector< double > firstDerivatives;
    double firstSegmentQuadratic;
    double finalPotential;
    double lastSegmentQuadratic;
    double definiteUndershootAuxiliary;
    double definiteOvershootAuxiliary;
    double startOfFinalSegment;
  };




  // This returns the value of the first derivative of the potential at
  // auxiliaryValue, by finding the correct segment and then returning its
  // slope at that point.
  inline double
  SplinePotential::FirstDerivative( double const auxiliaryValue ) const
  {
    if( ( auxiliaryValue <= 0.0 )
        ||
        ( auxiliaryValue >= definiteOvershootAuxiliary ) )
    {
      return 0.0;
    }
    if( auxiliaryValue < auxiliaryStep )
    {
      return ( 2.0 * auxiliaryValue * firstSegmentQuadratic );
    }
    if( auxiliaryValue >= startOfFinalSegment )
    {
      return FirstDerivativeNearPathPanic( auxiliaryValue
                                           - definiteOvershootAuxiliary );
    }
    return
    firstDerivatives[ size_t( auxiliaryValue * inverseOfAuxiliaryStep ) - 1 ];
  }

} /* namespace VevaciousPlusPlus */
#endif /* SPLINEPOTENTIAL_HPP_ */
