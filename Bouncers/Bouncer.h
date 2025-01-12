// The Bouncer class contains functions and data that are used by both
// the Decider and the Verifier

#pragma once

#include <limits.h>
#include <vector>
#include "../TuringMachine.h"

#define MAX_PARTITIONS     8    // #43477769 has 4 partitions
#define MAX_RUNS           500  // #3957107 has 156 runs

#define TAPE_ANY   3
#define WRAPAROUND 2000

#define VERIF_INFO_MAX_LENGTH 500000 // 259075 has been seen (#3957107)

class Bouncer : public TuringMachine
  {
public:
  Bouncer (uint32_t  MachineStates, uint32_t SpaceLimit, bool TraceOutput)
  : TuringMachine (MachineStates, SpaceLimit)
  , InitialTape (this)
  , TraceOutput (TraceOutput)
    {
    nUnilateral = 0 ;
    nBilateral = 0 ;
    nTranslated = 0 ;
    nDouble = 0 ;
    nMultiple = 0 ;
    nPartitioned = 0 ;
    nBells = 0 ;
    nHalters = 0 ;

    nRunsMax = 0 ;
    MaxRepeaterPeriod = 0 ;

    MinStat = INT_MAX ;
    MaxStat = INT_MIN ;
    }

  enum class BouncerType : uint8_t
    {
    Unknown,
    Unilateral,
    Bilateral,
    Translated,
    Bell, // Not a bouncer, but we may as well count these
    } ;
  BouncerType Type ;

  uint32_t nRuns ;
  uint32_t nPartitions ;

  //
  // Verification data structures
  //

  // TapeDescriptor describes the contents of the tape as a sequence
  //
  //   Wall[0] RepeaterCount[0]*Repeater[0] Wall[1] RepeaterCount[1]*Repeater[1] ...
  //     ... RepeaterCount[nPartitions - 1]*Repeater[nPartitions - 1] Wall[nPartitions]
  //
  // together with the machine state and the position of the tape head (i.e.
  // which wall, and the offset within the wall)
  struct TapeDescriptor
    {
    TapeDescriptor (const Bouncer* B) : B (B) { }
    TapeDescriptor (const TapeDescriptor& TD) : TapeDescriptor (TD.B)
      {
      *this = TD ;
      }
    TapeDescriptor& operator= (const TapeDescriptor& TD) ;

    const Bouncer* B ;
  
    std::vector<uint8_t> Wall[MAX_PARTITIONS + 1] ;
    std::vector<uint8_t> Repeater[MAX_PARTITIONS] ;
    uint32_t RepeaterCount[MAX_PARTITIONS] ;
    uint8_t State ;
    int Leftmost ;
    int Rightmost ;
    uint32_t TapeHeadWall ;
    int TapeHeadOffset ;
    } ;

  TapeDescriptor InitialTape ;

  void CheckTapesEquivalent (const TapeDescriptor& TD0, const TapeDescriptor& TD1) const ;
  struct TapePosition
    {
    bool InWall ;
    bool Finished ;
    uint32_t Partition ;
    uint32_t Repeat ;
    uint32_t Offset ;
    int WallOffset ; // Offset of TapeHeadWall from leftmost tape cell
    } ;
  void InitTapePosition (const TapeDescriptor& TD, TapePosition& TP) const ;
  uint8_t NextCell (const TapeDescriptor& TD, TapePosition& TP, int TapeHeadOffset) const ;

  // Segment defines a tape segment, a state, and a tape head
  struct Segment 
    {
    std::vector<uint8_t> Tape ;
    uint8_t State ;
    int TapeHead ;
    } ;

  // SegmentTransition defines the initial and final configurations of a tape segment.
  // The final tape head may lie outside the tape segment.
  struct SegmentTransition
    {
    uint32_t nSteps ;
    Segment Initial ;
    Segment Final ;
    } ;

  // Verification functions
  void CheckFollowOn (const Segment& Seg1, const Segment& Seg2) ;
  void CheckTape (const TuringMachine* TM, const TapeDescriptor& TD) ;
  void CheckTransition (const SegmentTransition& Tr) const ;
  void CheckWallTransition (TapeDescriptor TD0,
    TapeDescriptor TD1, const SegmentTransition& Tr) ;
  void CheckRepeaterTransition (const TapeDescriptor& TD0,
    const TapeDescriptor& TD1, const SegmentTransition& Tr) ;
  void CheckLeftwardRepeater (TapeDescriptor TD0, TapeDescriptor TD1, const SegmentTransition& Tr) ;
  void CheckRightwardRepeater (TapeDescriptor TD0, TapeDescriptor TD1, const SegmentTransition& Tr) ;
  void CheckSegment (const TapeDescriptor& TD, const Segment& Seg, uint32_t Wall) ;
  void ExpandWallsLeftward (TapeDescriptor& TD0, TapeDescriptor& TD1,
    uint32_t Wall, int Amount) ;
  void ExpandWallsRightward (TapeDescriptor& TD0, TapeDescriptor& TD1,
    uint32_t Wall, int Amount) ;
  void ExpandTapeLeftward (TapeDescriptor& TD, int Amount) ;
  void ExpandTapeRightward (TapeDescriptor& TD, int Amount) ;

  bool TraceOutput = false ;

  // Statistics
  uint32_t nUnilateral ;
  uint32_t nBilateral ;
  uint32_t nTranslated ;
  uint32_t nDouble ;
  uint32_t nMultiple ;
  uint32_t nPartitioned ;
  uint32_t nBells ;
  uint32_t nHalters ;

  uint32_t MaxRepeaterPeriod ;
  uint32_t MaxRepeaterMachine ;

  uint32_t nRunsMax ;
  uint32_t nRunsMachine ;

  // Whatever we may want to know from time to time:
  int MaxStat ; uint32_t MaxStatMachine ;
  int MinStat ; uint32_t MinStatMachine ;
  } ;
