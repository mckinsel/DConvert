#ifndef DCONVERT_TRIMMER_PRIVATE_H_
#define DCONVERT_TRIMMER_PRIVATE_H_

#include "Trimmer.h"

enum class TerminationDirection { FROMTHELEFT, FROMTHERIGHT };
class TerminationInterval {
public:
  int start;
  int end;
  TerminationDirection direction;
  bool operator==(const TerminationInterval& other) const
  {return start==other.start && end==other.end && direction==other.direction;}
};

bool terminates_from_left(const proto::Overlap& overlap);
bool terminates_from_right(const proto::Overlap& overlap);
bool extends_to_end(const proto::Overlap& overlap, TerminationDirection direction);

void trim_overlap_to_interval(proto::Overlap* overlap, TerminationInterval interval,
                              TerminationDirection direction);

std::vector<int> identify_terminating_overlaps(const std::vector<proto::Overlap>& overlaps,
                                               TerminationDirection direction);

std::vector<TerminationInterval> create_termination_intervals(
      const std::vector<int>& termination_positions,
      TerminationDirection direction,
      int agglomeration_distance,
      int termination_count_threshold);

void trim_terminating_overlaps(
      std::vector<proto::Overlap>* overlaps,
      const std::vector<TerminationInterval>& termination_intervals);

#endif