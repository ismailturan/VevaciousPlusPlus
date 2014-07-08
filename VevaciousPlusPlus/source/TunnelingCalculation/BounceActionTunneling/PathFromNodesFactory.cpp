/*
 * PathFromNodesFactory.cpp
 *
 *  Created on: Jul 8, 2014
 *      Author: Ben O'Leary (benjamin.oleary@gmail.com)
 */

#include "TunnelingCalculation/BounceActionTunneling/PathFromNodesFactory.hpp"

namespace VevaciousPlusPlus
{

  PathFromNodesFactory::PathFromNodesFactory(
                                      std::vector< double > const& falseVacuum,
                                       std::vector< double > const& trueVacuum,
                                            std::string const& xmlArguments ) :
    TunnelPathFactory( trueVacuum.size() ),
    nodesFromParameterization( NULL )
  {
    BOL::AsciiXmlParser argumentParser;
    argumentParser.loadString( xmlArguments );
    std::string nodeParameterizationType( "" );
    size_t numberOfVaryingNodes( 0 );
    while( argumentParser.readNextElement() )
    {
      if( argumentParser.currentElementNameMatches(
                                                  "NodeParameterizationType") )
      {
        nodeParameterizationType.assign(
                            argumentParser.getTrimmedCurrentElementContent() );
      }
      else if( argumentParser.currentElementNameMatches(
                                                  "NodeParameterizationType") )
      {
        numberOfVaryingNodes = BOL::StringParser::stringToInt(
                            argumentParser.getTrimmedCurrentElementContent() );
      }
    }
    if( nodeParameterizationType.compare( "NodesOnParallelPlanes" ) == 0 )
    {
      nodesFromParameterization = new NodesOnParallelPlanes( falseVacuum,
                                                             trueVacuum,
                                                        numberOfVaryingNodes );
    }
    else if( nodeParameterizationType.compare( "NodesOnBisectingPlanes" )
             == 0 )
    {
      nodesFromParameterization = new NodesOnBisectingPlanes( falseVacuum,
                                                              trueVacuum,
                                                        numberOfVaryingNodes );
    }
    else
    {
      std::stringstream errorStream;
      errorStream
      << "<NodeParameterizationType> was not a recognized form! The only"
      << " types currently valid are \"NodesOnParallelPlanes\" and"
      << " \"NodesOnBisectingPlanes\".";
      throw std::runtime_error( errorStream.str() );
    }
  }

  PathFromNodesFactory::~PathFromNodesFactory()
  {
    delete nodesFromParameterization;
  }

} /* namespace VevaciousPlusPlus */
