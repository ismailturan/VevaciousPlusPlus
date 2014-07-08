/*
 * LinearSplinePathThroughNodes.cpp
 *
 *  Created on: Jul 8, 2014
 *      Author: Ben O'Leary (benjamin.oleary@gmail.com)
 */

#include "TunnelingCalculation/BounceActionTunneling/LinearSplinePathThroughNodes.hpp"

namespace VevaciousPlusPlus
{

  LinearSplinePathThroughNodes::LinearSplinePathThroughNodes(
                         std::vector< std::vector< double > > const& pathNodes,
                                               double const pathTemperature ) :
    TunnelPath( pathNodes.front().size(),
                pathTemperature ),
    pathSegments( pathNodes.size() - 1 )
  {
    std::vector< double > segmentLengths( pathSegments.size() );
    double totalLength( 0.0 );
    for( size_t segmentIndex( 0 );
         segmentIndex < pathSegments.size();
         ++segmentIndex )
    {
      double squaredLength( 0.0 );
      std::vector< double > const& startNode( pathNodes[ segmentIndex ] );
      std::vector< double > const& endNode( pathNodes[ segmentIndex + 1 ] );
      for( size_t fieldIndex( 0 );
           fieldIndex < numberOfFields;
           ++fieldIndex )
      {
        double const
        fieldDifference( endNode[ fieldIndex ] - startNode[ fieldIndex ] );
        squaredLength += ( fieldDifference * fieldDifference );
      }
      segmentLengths[ segmentIndex ] = sqrt( squaredLength );
      totalLength += segmentLengths[ segmentIndex ];
    }
    double const inverseTotalLength( 1.0 / totalLength );
    for( size_t segmentIndex( 0 );
         segmentIndex < pathSegments.size();
         ++segmentIndex )
    {
      pathSegments[ segmentIndex ]
      = QuadraticSplinePathSegment( pathNodes[ segmentIndex ],
                                    pathNodes[ segmentIndex + 1 ],
                     ( segmentLengths[ segmentIndex ] * inverseTotalLength ) );
    }
  }

  LinearSplinePathThroughNodes::~LinearSplinePathThroughNodes()
  {
    // TODO Auto-generated destructor stub
  }

} /* namespace VevaciousPlusPlus */
