/*
 * Hom4ps2AndMinuit.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: Ben O'Leary (benjamin.oleary@gmail.com)
 */

#include "../../include/VevaciousPlusPlus.hpp"

namespace VevaciousPlusPlus
{

  Hom4ps2AndMinuit::Hom4ps2AndMinuit(
                      HomotopyContinuationReadyPolynomial& polynomialPotential,
                                      std::string const& pathToHom4ps2,
                                      std::string const homotopyType ) :
    HomotopyContinuationAndGradient( polynomialPotential ),
    polynomialPotential( polynomialPotential ),
    pathToHom4ps2( pathToHom4ps2 ),
    homotopyType( homotopyType ),
    variableNamer( 4,
                   '0',
                   6,
                   2,
                   "v" ),
    complexSolutions(),
    variableNames(),
    purelyRealSolutionSets()
  {
    // placeholder:
    /**/std::cout << std::endl
    << "Placeholder: "
    << "Hom4ps2AndMinuit::Hom4ps2AndMinuit( ..., \"" << pathToHom4ps2
    << "\", \"" << homotopyType << "\" )";
    std::cout << std::endl;/**/
  }

  Hom4ps2AndMinuit::~Hom4ps2AndMinuit()
  {
    // placeholder:
    /**/std::cout << std::endl
    << "Placeholder: "
    << "Hom4ps2AndMinuit::~Hom4ps2AndMinuit()";
    std::cout << std::endl;/**/
  }



  // This should find all the minima of the potential evaluated at a
  // temperature given by temperatureInGev, and record them in foundMinima.
  // It should also set dsbVacuum, and record the minima lower than dsbVacuum
  // in panicVacua, and of those, it should set panicVacuum to be the prime
  // candidate for tunneling out of dsbVacuum (by default, taken to be the
  // minimum in panicVacua closest to dsbVacuum).
  void Hom4ps2AndMinuit::FindMinima( double const temperatureInGev )
  {
    // placeholder:
    /**/std::cout << std::endl
    << "Placeholder: "
    << "Hom4ps2AndMinuit::FindMinima( " << temperatureInGev << " )";
    std::cout << std::endl;/**/

    char originalWorkingDirectory[ PATH_MAX ];
    if( NULL == getcwd( originalWorkingDirectory,
                        PATH_MAX ) )
    {
      std::cout
      << std::endl
      << "Error! unable to determine current working directory! (necessary,"
      << " since this program needs to change directory to the directory where"
      << " the hom4ps2 executable is, since unfortunately HOM4PS2 runs with"
      << " relative paths; this program returns to where it was called though,"
      << " to make batch calls easier.)";
      std::cout << std::endl;
      throw std::runtime_error(
                             "could not determine current working directory" );
    }
    int directoryChangeSuccess( chdir( pathToHom4ps2.c_str() ) );
    if( 0 != directoryChangeSuccess )
    {
      throw std::runtime_error(
                           "could not change directory to HOM4PS2 directory" );
    }
    std::string hom4ps2InputFilename( "./VevaciousHomotopyContinuation.txt" );

    WriteHom4p2Input( hom4ps2InputFilename );

    std::string systemCommand( "rm ./bin/input.num" );
    BOL::UsefulStuff::runSystemCommand( systemCommand );
    systemCommand.assign( "/bin/bash -c \"./hom4ps2 " );
    systemCommand.append( hom4ps2InputFilename );
    systemCommand.append(  " <<< " );
    systemCommand.append( homotopyType );
    systemCommand.append( "\"" );
    BOL::UsefulStuff::runSystemCommand( systemCommand );

    // at this point, we are in the directory with hom4ps2 & data.roots.
    ParseHom4ps2Output( "./data.roots" );

    directoryChangeSuccess = chdir( originalWorkingDirectory );
    if( 0 != directoryChangeSuccess )
    {
      throw std::runtime_error(
                      "could not change directory back to initial directory" );
    }
  }

  // This should find the minimum at temperature temperatureInGev nearest to
  // minimumToAdjust (which is assumed to be a minimum of the potential at a
  // different temperature).
  PotentialMinimum Hom4ps2AndMinuit::AdjustMinimumForTemperature(
                                       PotentialMinimum const& minimumToAdjust,
                                                double const temperatureInGev )
  {
    // placeholder:
    /**/std::cout << std::endl
    << "Placeholder: "
    << "Hom4ps2AndMinuit::AdjustMinimumForTemperature( ..., "
    << temperatureInGev << " )";
    std::cout << std::endl;
    return PotentialMinimum( minimumToAdjust );/**/
  }


  void Hom4ps2AndMinuit::WriteHom4p2Input(
                                      std::string const& hom4ps2InputFilename )
  {
    polynomialPotential.PreparePolynomialHomotopyContinuation();
    std::vector< PolynomialSum > const&
    targetSystem( polynomialPotential.TargetPolynomialGradient() );

    variableNames.clear();
    for( unsigned int whichVariable( 0 );
         whichVariable < targetSystem.size();
         ++whichVariable )
    {
      variableNames.push_back( variableNamer.intToString( whichVariable ) );
      nameToIndexMap[ variableNames.back() ] = whichVariable;
    }

    std::ofstream hom4ps2Input( hom4ps2InputFilename.c_str() );
    hom4ps2Input << "{\n";
    for( std::vector< PolynomialSum >::const_iterator
         whichConstraint( targetSystem.begin() );
         whichConstraint < targetSystem.end();
         ++whichConstraint )
    {
      hom4ps2Input
      << whichConstraint->AsStringAtCurrentScale( variableNames ) << ";\n";
    }
    hom4ps2Input << "}\n";
    hom4ps2Input.close();
  }

  void Hom4ps2AndMinuit::ParseHom4ps2Output(
                                     std::string const& hom4ps2OutputFilename )
  {
    complexSolutions.clear();
    purelyRealSolutionSets.clear();
    BOL::CommentedTextParser tadpoleSolutionsFile( "###",
                                                   false );
    bool successfulOperation( tadpoleSolutionsFile.openFile(
                                                     hom4ps2OutputFilename ) );
    if( !successfulOperation )
    {
      throw std::runtime_error( "could not open " + hom4ps2OutputFilename );
    }
    // First we pick out lines corresponding to solutions until we find
    // "The order of variables :" which comes after all solutions have been
    // printed.
    std::string lineString( "" );
    std::complex< long double > currentComplexNumber( 0.0L,
                                                      0.0L );
    std::stringstream parsingStream;
    while( tadpoleSolutionsFile.readNextNonEmptyLineOfFileWithoutComment(
                                                                 lineString ) )
    {
      if( '(' == lineString[ 0 ] )
      {
        // If the above conditions are satisfied, lineString now contains the
        // root as a complex number, in the form where zero is
        // "(  0.0000000000000000E+00 ,  0.0000000000000000E+00)"
        BOL::StringParser::substituteCharacterWith( lineString,
                                                    ',',
                                                    ' ' );
        parsingStream.clear();
        parsingStream.str( lineString.substr( 1,
                                              ( lineString.size() - 2 ) ) );
        parsingStream
        >> currentComplexNumber.real() >> currentComplexNumber.imag();
        complexSolutions.push_back( currentComplexNumber );
      }
      else if( 0 == lineString.compare( "The order of variables :" ) )
      {
        break;
      }
    }
    // At this point, the line "The order of variables :" should have been
    // found. If it hasn't, the file is malformed, but we carry on regardless,
    // looking for the variables in order:
    std::vector< unsigned int > indexOrder( variableNames.size() );
    unsigned int whichVariable( 0 );
    while( tadpoleSolutionsFile.readNextNonEmptyLineOfFileWithoutComment(
                                                                 lineString ) )
    {
      if( 0 == lineString.compare(
                         "===============>   HOM4PS-2.0   <===============" ) )
      {
        break;
      }
      indexOrder[ whichVariable ]
      = nameToIndexMap[ BOL::StringParser::trimFromFrontAndBack(
                                                                lineString ) ];
      ++whichVariable;
    }

    // debugging:
    /**/std::cout << std::endl << "debugging:"
    << std::endl
    << "indexOrder = ";
    for( std::vector< unsigned int >::iterator
         whichIndex( indexOrder.begin() );
         whichIndex < indexOrder.end();
         ++whichIndex )
    {
      std::cout << std::endl << *whichIndex;
    }
    std::cout << std::endl;
    std::cout << std::endl;/**/

    // placeholder:
    /**/std::cout << std::endl
    << "Placeholder: "
    << "still need to sort out purelyRealSolutionSets!";
    std::cout << std::endl;/**/
  }


} /* namespace VevaciousPlusPlus */
