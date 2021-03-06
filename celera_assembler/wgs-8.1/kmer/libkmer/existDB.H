#ifndef EXISTDB_H
#define EXISTDB_H

//  Used by wgs-assembler, to determine if a rather serious bug was patched.
#define EXISTDB_H_VERSION 1960

#include "bio++.H"

//  Takes as input a list of mers (in a file) and builds a searchable
//  structure listing those mers.  Duplicate mers are not removed and
//  will be stored multiple times.
//
//  Using a compressed hash is allowed, but somewhat useless -- it is
//  really slow and doesn't save that much.
//
//  If existDBcanonical is requested, this will store only the
//  canonical mer.  It is up to the client to be sure that is
//  appropriate!  See positionDB.H for more.

//#define STATS

typedef u32bit existDBflags;
const existDBflags  existDBnoFlags         = 0x0000;
const existDBflags  existDBcompressHash    = 0x0001;
const existDBflags  existDBcompressBuckets = 0x0002;
const existDBflags  existDBcompressCounts  = 0x0004;
const existDBflags  existDBcanonical       = 0x0008;
const existDBflags  existDBforward         = 0x0010;
const existDBflags  existDBcounts          = 0x0020;

class existDB {
public:

  //  Read state from an existDB file
  existDB(char const  *filename,
          bool         loadData=true);

  //  Load mers from an existing existDB file, a fastafile, or a meryl database
  existDB(char const    *filename,
          u32bit         merSize,
          existDBflags   flags,
          u32bit         lo,
          u32bit         hi);

  //  Load mers from a character string
  existDB(char const    *sequence,
          u32bit         merSize,
          existDBflags   flags);

  ~existDB();

  void        saveState(char const *filename);

  void        printState(FILE *stream);

  bool        isForward(void)    { return(_isForward);   };
  bool        isCanonical(void)  { return(_isCanonical); };

  bool        exists(u64bit mer);
  u64bit      count(u64bit mer);

private:
  bool        loadState(char const *filename, bool beNoisy=false, bool loadData=true);
  bool        createFromFastA(char const  *filename,
                              u32bit       merSize,
                              u32bit       flags);
  bool        createFromMeryl(char const  *filename,
                              u32bit       merSize,
                              u32bit       lo,
                              u32bit       hi,
                              u32bit       flags);
  bool        createFromSequence(char const  *sequence,
                                 u32bit       merSize,
                                 u32bit       flags);

  u64bit       HASH(u64bit k) {
    return(((k >> _shift1) ^ (k >> _shift2) ^ k) & _mask1);
  };

  u64bit       CHECK(u64bit k) {
    return(k & _mask2);
  };

  void         insertMer(u64bit hsh, u64bit chk, u64bit cnt, u64bit *countingTable) {

    //  If the mer is already here, just update the count.  This only
    //  works if not _compressedBucket, and only makes sense for loading from
    //  fasta or sequence.

    if ((_compressedBucket == false) &&
        (_searchForDupe)) {
      u64bit st = _hashTable[hsh];
      u64bit ed = countingTable[hsh];

      for (; st<ed; st++) {
        if (_buckets[st] == chk) {
          if (_counts)
            _counts[st] += cnt;
          return;
        }
      }
    }

    if (_compressedBucket)
      setDecodedValue(_buckets, countingTable[hsh] * _chkWidth, _chkWidth, chk);
    else
      _buckets[countingTable[hsh]] = chk;

    if (_counts) {
      if (_compressedCounts) {
        setDecodedValue(_counts, countingTable[hsh] * _cntWidth, _cntWidth, cnt);
      } else {
        _counts[countingTable[hsh]] = cnt;
      }
    }

    countingTable[hsh]++;
  };

  bool        _compressedHash;
  bool        _compressedBucket;
  bool        _compressedCounts;
  bool        _isForward;
  bool        _isCanonical;

  bool        _searchForDupe;

  u32bit      _merSizeInBases;

  u32bit      _shift1;
  u32bit      _shift2;
  u64bit      _mask1;
  u64bit      _mask2;

  u32bit      _hshWidth;  //  Only for the compressed hash
  u32bit      _chkWidth;  //  Only for the compressed bucket
  u32bit      _cntWidth;  //  Only for the compressed counts

  u64bit      _hashTableWords;
  u64bit      _bucketsWords;
  u64bit      _countsWords;

  u64bit     *_hashTable;
  u64bit     *_buckets;
  u64bit     *_counts;

  void clear(void) {
  };
};

#endif  //  EXISTDB_H
