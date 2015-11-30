/*
 * SimpleLhaParser.hpp
 *
 *  Created on: Nov 26, 2015
 *      Author: Ben O'Leary (benjamin.oleary@gmail.com)
 */

#ifndef SIMPLELHAPARSER_HPP_
#define SIMPLELHAPARSER_HPP_

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <utility>
#include <list>
#include "ParsingUtilities.hpp"

namespace LHPC
{

  class LhaBlockAtSingleScale
  {
  public:
    LhaBlockAtSingleScale( bool const hasExplicitScale,
                           double const scaleValue = 0.0 ) :
      hasExplicitScale( hasExplicitScale ),
      scaleValue( scaleValue ),
      contentLines() {}

    LhaBlockAtSingleScale( LhaBlockAtSingleScale const& copySource ) :
      hasExplicitScale( copySource.hasExplicitScale ),
      scaleValue( copySource.scaleValue ),
      contentLines( copySource.contentLines ) {}

    ~LhaBlockAtSingleScale() {}

    bool HasExplicitScale() const { return hasExplicitScale; }

    double ScaleValue() const { return scaleValue; }

    std::vector< std::string > const& ContentLines() const
    { return contentLines; }

    void AddLine( std::string const& newLine )
    { contentLines.push_back( newLine ); }

    // This examines each line to see if it starts with the given indices, and
    // then returns the rest of the first line which matches the indices
    // (without the indices). If no match is found, and empty string is
    // returned.
    std::string MatchingEntry( std::vector< int > const& entryIndices ) const;


  protected:
    bool hasExplicitScale;
    double scaleValue;
    std::vector< std::string > contentLines;
  };


  class LhaBlockSet
  {
  public:
    LhaBlockSet( std::string const& uppercaseName ) :
      uppercaseName( uppercaseName ),
      blocksInReadOrder(),
      noExplicitScales( true ),
      highestBlockScale( -1.0 ),
      lowestBlockScale( -1.0 ) {}

    LhaBlockSet( LhaBlockSet const& copySource ) :
      uppercaseName( copySource.uppercaseName ),
      blocksInReadOrder( copySource.blocksInReadOrder ),
      noExplicitScales( copySource.noExplicitScales ),
      highestBlockScale( copySource.highestBlockScale ),
      lowestBlockScale( copySource.lowestBlockScale ) {}

    ~LhaBlockSet() {}


    std::string const& UppercaseName() const { return uppercaseName; }

    // This returns the blocks of the same name in the order in which they were
    // read.
    std::vector< LhaBlockAtSingleScale > const& BlocksInReadOrder() const
    { return blocksInReadOrder; }

    // This returns the highest scale given for any block with this set's name.
    double HighestBlockScale() const { return highestBlockScale; }

    // This returns the lowest scale given for any block with this set's name.
    double LowestBlockScale() const { return lowestBlockScale; }

    // This adds a LhaBlockAtSingleScale to blocksInReadOrder without an
    // explicit scale and returns a pointer to it.
    LhaBlockAtSingleScale* NewBlock();

    // This adds a LhaBlockAtSingleScale to blocksInReadOrder with an explicit
    // scale and returns a pointer to it.
    LhaBlockAtSingleScale* NewBlock( double const explicitScale );

    // This fills entriesAtScales based on entryIndices. Every block is
    // examined to see if it has an entry matching the indices in entryIndices
    // (if no there are no indices, then the first line of the block is
    // considered to be the entry). If an entry has matching indices, then it
    // is paired with the scale of the block in which it was found, and this
    // pair is added to entriesAtScales. If the block has no explicit scale, it
    // is ignored if onlyWithExplicitScale is true, or else its entry is paired
    // with implicitScale.
    void AddEntries( std::vector< int > const& entryIndices,
                std::list< std::pair< std::string, double > >& entriesAtScales,
                     bool const onlyWithExplicitScale,
                     double const implicitScale ) const;


  protected:
    std::string uppercaseName;
    std::vector< LhaBlockAtSingleScale > blocksInReadOrder;
    bool noExplicitScales;
    double highestBlockScale;
    double lowestBlockScale;
  };


  class SimpleLhaParser
  {
  public:
    // This splits a string in the format of block name followed by whitespace
    // or an opening bracket character into the substring for just the name
    // paired with the indices as a vector of integers.
    static std::pair< std::string, std::vector< int > >
    ParseBlockNameAndIndices( std::string const& blockNameThenIndices );


    SimpleLhaParser() : blocksInFirstInstanceReadOrder(),
                        blockNamesToIndices(),
                        currentBlockSet( NULL ),
                        currentBlock( NULL ),
                        noExplicitScales( true ),
                        highestBlockScale( -1.0 ),
                        lowestBlockScale( -1.0 ) {}

    ~SimpleLhaParser() {}


    // This opens the file with name fileName and parses it into blocks.
    void ReadFile( std::string const& fileName );

    // This returns the LhaBlockSet objects in the order in which the first
    // block of each LhaBlockSet was read.
    std::vector< LhaBlockSet > const& BlocksInFirstInstanceReadOrder() const
    { return blocksInFirstInstanceReadOrder; }

    // This returns the highest scale given with any block of any name.
    double HighestBlockScale() const { return highestBlockScale; }

    // This returns the lowest scale given with any block of any name.
    double LowestBlockScale() const { return lowestBlockScale; }

    // This returns the set of blocks with name which matches blockName
    // ignoring case. If no blocks match, then NULL is returned.
    LhaBlockSet const* BlocksWithName( std::string blockName ) const;

    // This fills entriesAtScales based on blockName and entryIndices. Every
    // block which matches blockName is examined to see if it has an entry
    // matching the indices in entryIndices (if no there are no indices, then
    // the first line of the block is considered to be the entry). If an entry
    // has matching indices, then it is paired with the scale of the block in
    // which it was found, and this pair is added to entriesAtScales. If the
    // block has no explicit scale, it is ignored if onlyWithExplicitScale is
    // true, or else its entry is paired with implicitScale.
    void operator()( std::string const& blockName,
                     std::vector< int > const& entryIndices,
                std::list< std::pair< std::string, double > >& entriesAtScales,
                     bool const onlyWithExplicitScale = false,
                     double const implicitScale = 0.0 ) const;

    // This fills entriesAtScales as operator() above, but unbundling a pair of
    // arguments.
    void operator()(
      std::pair< std::string, std::vector< int > > const& blockNameThenIndices,
                std::list< std::pair< std::string, double > >& entriesAtScales,
                     bool const onlyWithExplicitScale = false,
                     double const implicitScale = 0.0 ) const
    { return operator()( blockNameThenIndices.first,
                         blockNameThenIndices.second,
                         entriesAtScales,
                         onlyWithExplicitScale,
                         implicitScale ); }

    // This fills entriesAtScales as operator() above, but first parsing
    // blockNameThenIndices.
    void operator()( std::string const& blockNameThenIndices,
                std::list< std::pair< std::string, double > >& entriesAtScales,
                     bool const onlyWithExplicitScale = false,
                     double const implicitScale = 0.0 ) const
    { return operator()( ParseBlockNameAndIndices( blockNameThenIndices ),
                         entriesAtScales,
                         onlyWithExplicitScale,
                         implicitScale ); }


  protected:
    // This returns false if there is no room for "BLOCK " plus a block name of
    // at least 1 character, or if the substring is not "BLOCK" followed by a
    // whitespace or tab character.
    static bool IsBlockHeader( std::string const& trimmedLine )
    { return ( ( trimmedLine.size() > 6 )
               &&
               ( std::toupper( trimmedLine[ 0 ] ) == 'B' )
               &&
               ( std::toupper( trimmedLine[ 1 ] ) == 'L' )
               &&
               ( std::toupper( trimmedLine[ 2 ] ) == 'O' )
               &&
               ( std::toupper( trimmedLine[ 3 ] ) == 'C' )
               &&
               ( std::toupper( trimmedLine[ 4 ] ) == 'K' )
               &&
               ( ( trimmedLine[ 5 ] == ' ' )
                 ||
                 ( trimmedLine[ 5 ] == '\t' ) ) ); }

    // This returns true if the line begins with "DECAY" then a whitespace
    // character.
    static bool IsDecayHeader( std::string const& trimmedLine )
    { return ( ( trimmedLine.size() > 6 )
               &&
               ( std::toupper( trimmedLine[ 0 ] ) == 'D' )
               &&
               ( std::toupper( trimmedLine[ 1 ] ) == 'E' )
               &&
               ( std::toupper( trimmedLine[ 2 ] ) == 'C' )
               &&
               ( std::toupper( trimmedLine[ 3 ] ) == 'A' )
               &&
               ( std::toupper( trimmedLine[ 4 ] ) == 'Y' )
               &&
               ( ( trimmedLine[ 5 ] == ' ' )
                 ||
                 ( trimmedLine[ 5 ] == '\t' ) ) ); }

    // This returns the double interpreted from the substring following the '='
    // (or throws an exception if any other characters come between the 'Q'/'q'
    // and the '=').
    static double ParseScale( std::string const& headerLine,
                              size_t const positionOfQ );

    // This puts the block name from trimmedLine into blockName and returns the
    // position of the first character in trimmedLine after the end of the
    // name.
    static size_t ParseBlockName( std::string const& trimmedLine,
                                  std::string& blockName );


    std::vector< LhaBlockSet > blocksInFirstInstanceReadOrder;
    std::map< std::string, size_t > blockNamesToIndices;
    std::map< std::string, size_t >::const_iterator blockNameToIndex;
    LhaBlockSet* currentBlockSet;
    LhaBlockAtSingleScale* currentBlock;
    bool noExplicitScales;
    double highestBlockScale;
    double lowestBlockScale;


    // This trims leading whitespace, any comments ('#' and all following
    // characters to the end of the line), and any trailing whitespace after
    // removing comments, then passes the trimmed line to ParseContent(...).
    void ParseLine( std::string const& readLine );

    // This either parses the line denoting a new block, or adds the line to
    // the block which is currently being read, if any.
    void ParseContent( std::string const& trimmedLine );

    // This parses the block's name and scale, if given, and starts a new block
    // in the appropriate LhaBlockSet.
    void ReadBlockHeader( std::string const& trimmedLine );

    // This returns a pointer to the block set with (uppercase) name which
    // matches uppercaseBlockName. If none exists already, a new LhaBlockSet
    // with this name is appended to blocksInFirstInstanceReadOrder and a
    // pointer to this new block set is returned.
    LhaBlockSet* BlockSetForName( std::string const& uppercaseBlockName );

    // This updates the highest and lowest recorded scales and adds a new block
    // to currentBlockSet, returning a pointer to the newly-created block.
    LhaBlockAtSingleScale*
    AddNewBlockWithExplicitScale( double explicitScale );
  };





  // This examines each line to see if it starts with the given indices, and
  // then returns the rest of the first line which matches the indices
  // (without the indices). If no match is found, and empty string is
  // returned.
  inline std::string LhaBlockAtSingleScale::MatchingEntry(
                                 std::vector< int > const& entryIndices ) const
  {
    size_t contentStart( std::string::npos );
    for( std::vector< std::string >::const_iterator
         contentLine( contentLines.begin() );
         contentLine != contentLines.end();
         ++contentLine )
    {
      contentStart = ParsingUtilities::StartOfMatchedContent( *contentLine,
                                                              entryIndices );
      if( contentStart != std::string::npos )
      {
        return contentLine->substr( contentStart );
      }
    }
    return "";
  }


  // This adds a LhaBlockAtSingleScale to blocksInReadOrder without an
  // explicit scale and returns a pointer to it.
  inline LhaBlockAtSingleScale* LhaBlockSet::NewBlock()
  {
    blocksInReadOrder.push_back( LhaBlockAtSingleScale( false ) );
    return &(blocksInReadOrder.back());
  }

  // This adds a LhaBlockAtSingleScale to blocksInReadOrder with an explicit
  // scale and returns a pointer to it.
  inline LhaBlockAtSingleScale*
  LhaBlockSet::NewBlock( double const explicitScale )
  {
    if( noExplicitScales )
    {
      highestBlockScale = lowestBlockScale = explicitScale;
      noExplicitScales = false;
    }
    else
    {
      highestBlockScale = std::max( explicitScale, highestBlockScale );
      lowestBlockScale = std::min( explicitScale, lowestBlockScale );
    }
    blocksInReadOrder.push_back( LhaBlockAtSingleScale( true,
                                                        explicitScale ) );
    return &(blocksInReadOrder.back());
  }

  // This fills entriesAtScales based on entryIndices. Every block is
  // examined to see if it has an entry matching the indices in entryIndices
  // (if no there are no indices, then the first line of the block is
  // considered to be the entry). If an entry has matching indices, then it
  // is paired with the scale of the block in which it was found, and this
  // pair is added to entriesAtScales. If the block has no explicit scale, it
  // is ignored if onlyWithExplicitScale is true, or else its entry is paired
  // with implicitScale.
  inline void LhaBlockSet::AddEntries( std::vector< int > const& entryIndices,
                std::list< std::pair< std::string, double > >& entriesAtScales,
                                       bool const onlyWithExplicitScale,
                                       double const implicitScale ) const
  {
    for( std::vector< LhaBlockAtSingleScale >::const_iterator
         blockAtSingleScale( blocksInReadOrder.begin() );
         blockAtSingleScale != blocksInReadOrder.end();
         ++blockAtSingleScale )
    {
      if( !onlyWithExplicitScale
          ||
          ( blockAtSingleScale->HasExplicitScale() ) )
      {
        entriesAtScales.push_back( std::pair< std::string, double >(
                           blockAtSingleScale->MatchingEntry( entryIndices ),
                                   ( blockAtSingleScale->HasExplicitScale() ?
                                     blockAtSingleScale->ScaleValue() :
                                     implicitScale ) ) );
      }
    }
  }


  // This splits a string in the format of block name followed by whitespace
  // or an opening bracket character into the substring for just the name
  // paired with the indices as a vector of integers.
  inline std::pair< std::string, std::vector< int > >
  SimpleLhaParser::ParseBlockNameAndIndices(
                                      std::string const& blockNameThenIndices )
  {
    size_t const
    blockNameEndPlusOne( blockNameThenIndices.find_first_of( " \t([{," ) );
    if( blockNameEndPlusOne != std::string::npos )
    {
      return std::pair< std::string, std::vector< int > >(
                                                blockNameThenIndices.substr( 0,
                                                         blockNameEndPlusOne ),
                                                ParsingUtilities::ParseIndices(
                       blockNameThenIndices.substr( blockNameEndPlusOne ) ) );
    }
    else
    {
      return
      std::pair< std::string, std::vector< int > >( blockNameThenIndices,
                                                    std::vector< int >() );
    }
  }

  // This opens the file with name fileName and parses it into blocks.
  inline void SimpleLhaParser::ReadFile( std::string const& fileName )
  {
    std::string readLine( "" );
    std::ifstream fileStream( fileName.c_str() );
    if( !(fileStream.is_open()) )
    {
      std::stringstream errorBuilder;
      errorBuilder << "Could not open file named \"" << fileName << "\".";
      throw std::runtime_error( errorBuilder.str() );
    }
    while( std::getline( fileStream, readLine ) )
    {
      ParseLine( readLine );
    }
    fileStream.close();
  }

  // This returns the set of blocks with name which matches blockName
  // ignoring case. If no blocks match, then NULL is returned.
  inline LhaBlockSet const*
  SimpleLhaParser::BlocksWithName( std::string blockName ) const
  {
    ParsingUtilities::TransformToUppercase( blockName );
    std::map< std::string, size_t >::const_iterator
    nameToIndex( blockNamesToIndices.find( blockName ) );
    if( nameToIndex == blockNamesToIndices.end() )
    {
      return NULL;
    }
    return &(blocksInFirstInstanceReadOrder[ nameToIndex->second ]);
  }

  // This fills entriesAtScales based on blockName and entryIndices. Every
  // block which matches blockName is examined to see if it has an entry
  // matching the indices in entryIndices (if no there are no indices, then
  // the first line of the block is considered to be the entry). If an entry
  // has matching indices, then it is paired with the scale of the block in
  // which it was found, and this pair is added to entriesAtScales. If the
  // block has no explicit scale, it is ignored if onlyWithExplicitScale is
  // true, or else its entry is paired with implicitScale.
  inline void SimpleLhaParser::operator()( std::string const& blockName,
                                        std::vector< int > const& entryIndices,
                std::list< std::pair< std::string, double > >& entriesAtScales,
                                           bool const onlyWithExplicitScale,
                                           double const implicitScale ) const
  {
    LhaBlockSet const* blockSet( BlocksWithName( blockName ) );
    if( blockSet != NULL )
    {
      blockSet->AddEntries( entryIndices,
                            entriesAtScales,
                            onlyWithExplicitScale,
                            implicitScale );
    }
  }

  // This returns the double interpreted from the substring following the '='
  // (or throws an exception if any other characters come between the 'Q'/'q'
  // and the '=').
  inline double SimpleLhaParser::ParseScale( std::string const& headerLine,
                                             size_t const positionOfQ )
  {
    size_t const positionForEquals( headerLine.find_first_not_of(
                                         ParsingUtilities::WhitespaceChars(),
                                                       ( positionOfQ + 1 ) ) );
    if( ( positionForEquals == std::string::npos )
        ||
        headerLine[ positionForEquals ] != '=' )
    {
      std::stringstream errorBuilder;
      errorBuilder
      << "Could not parse scale for block out of \"" << headerLine
      << " \" (needs \"=\" after Q).";
      throw std::runtime_error( errorBuilder.str() );
    }
    return std::atof( headerLine.substr( positionForEquals + 1 ).c_str() );
  }

  // This trims leading whitespace, any comments ('#' and all following
  // characters to the end of the line), and any trailing whitespace after
  // removing comments, then passes the trimmed line to ParseContent(...).
  inline void SimpleLhaParser::ParseLine( std::string const& readLine )
  {
    size_t const startPosition( readLine.find_first_not_of(
                                       ParsingUtilities::WhitespaceChars() ) );
    if( startPosition == std::string::npos )
    {
      return;
    }
    size_t endPosition( readLine.find( '#',
                                       startPosition ) );
    if( endPosition == startPosition )
    {
      return;
    }
    endPosition
    = readLine.find_last_not_of( ParsingUtilities::WhitespaceChars(),
                                 ( endPosition - 1 ) );
    ParseContent( readLine.substr( startPosition,
                                   ( endPosition - startPosition + 1 ) ) );
  }

  // This either parses the line denoting a new block, or adds the line to
  // the block which is currently being read, if any.
  inline void SimpleLhaParser::ParseContent( std::string const& trimmedLine )
  {
    if( IsBlockHeader( trimmedLine ) )
    {
      ReadBlockHeader( trimmedLine );
    }
    else if( IsDecayHeader( trimmedLine ) )
    {
      // This parser does not record decay entries.
      currentBlock = NULL;
    }
    else if( currentBlock != NULL )
    {
      currentBlock->AddLine( trimmedLine );
    }
  }

  // This parses the block's name and scale, if given, and starts a new block
  // in the appropriate LhaBlockSet.
  inline void
  SimpleLhaParser::ReadBlockHeader( std::string const& trimmedLine )
  {
    std::string blockName;
    size_t const blockNameEndPlusOne( ParseBlockName( trimmedLine,
                                                      blockName ) );
    currentBlockSet = BlockSetForName( blockName );
    size_t const positionOfQ( trimmedLine.find_first_of( "Qq",
                                               ( blockNameEndPlusOne + 1 ) ) );
    if( positionOfQ != std::string::npos )
    {
      currentBlock = AddNewBlockWithExplicitScale( ParseScale( trimmedLine,
                                                               positionOfQ ) );
    }
    else
    {
      currentBlock = currentBlockSet->NewBlock();
    }
  }

  // This puts the block name from trimmedLine into blockName and returns the
  // position of the first character in trimmedLine after the end of the
  // name.
  inline size_t
  SimpleLhaParser::ParseBlockName( std::string const& trimmedLine,
                                   std::string& blockName )
  {
    // Characters 0 to 4 are "BLOCK" and character 5 is whitespace.
    size_t const blockNameStart(
            trimmedLine.find_first_not_of( ParsingUtilities::WhitespaceChars(),
                                           6 ) );
    size_t const blockNameEndPlusOne(
                trimmedLine.find_first_of( ParsingUtilities::WhitespaceChars(),
                                           blockNameStart ) );
    size_t const nameLength( ( blockNameEndPlusOne == std::string::npos ) ?
                             ( trimmedLine.size() - blockNameStart ) :
                             ( blockNameEndPlusOne - blockNameStart ) );
    blockName.resize( nameLength );
    for( size_t nameIndex( 0 );
         nameIndex < nameLength;
         ++nameIndex )
    {
      blockName[ nameIndex ]
      = std::toupper( trimmedLine[ nameIndex + blockNameStart ] );
    }
    return blockNameEndPlusOne;
  }

  // This returns a pointer to the block set with (uppercase) name which
  // matches uppercaseBlockName. If none exists already, a new LhaBlockSet
  // with this name is appended to blocksInFirstInstanceReadOrder and a
  // pointer to this new block set is returned.
  inline LhaBlockSet*
  SimpleLhaParser::BlockSetForName( std::string const& uppercaseBlockName )
  {
    blockNameToIndex = blockNamesToIndices.find( uppercaseBlockName );
    if( blockNameToIndex != blockNamesToIndices.end() )
    {
      return &(blocksInFirstInstanceReadOrder[ blockNameToIndex->second ]);
    }
    blockNamesToIndices[ uppercaseBlockName ]
    = blocksInFirstInstanceReadOrder.size();
    blocksInFirstInstanceReadOrder.push_back(
                                           LhaBlockSet( uppercaseBlockName ) );
    return &(blocksInFirstInstanceReadOrder.back());
  }

  // This updates the highest and lowest recorded scales and adds a new block
  // to currentBlockSet, returning a pointer to the newly-created block.
  inline LhaBlockAtSingleScale*
  SimpleLhaParser::AddNewBlockWithExplicitScale( double explicitScale )
  {
    if( noExplicitScales )
    {
      highestBlockScale = lowestBlockScale = explicitScale;
      noExplicitScales = false;
    }
    else
    {
      highestBlockScale = std::max( explicitScale, highestBlockScale );
      lowestBlockScale = std::min( explicitScale, lowestBlockScale );
    }
    return currentBlockSet->NewBlock( explicitScale );
  }

} /* namespace LHPC */

#endif /* SIMPLELHAPARSER_HPP_ */
