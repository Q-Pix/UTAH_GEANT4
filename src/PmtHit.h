// ----------------------------------------------------------------------------
// nexus | PmtHit.h
//
// This class describes the charge detected by a photosensor.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef PMT_HIT_H
#define PMT_HIT_H 1

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>


class PmtHit: public G4VHit
{
public:
/// Default constructor
PmtHit();
/// Constructor providing the detector ID and position
PmtHit(G4int ID, const G4ThreeVector& position, G4double bin_size);
/// Copy-constructor
PmtHit(const PmtHit&);
/// Destructor
~PmtHit();

/// Assignement operator
const PmtHit& operator=(const PmtHit&);
/// Equality operator
G4int operator==(const PmtHit&) const;

/// Memory allocation
void* operator new(size_t);
/// Memory deallocation
    void operator delete(void*);

/// Returns the detector ID code
G4int GetPmtID() const;
/// Sets the detector ID code
void  SetPmtID(G4int);

/// Returns the position of the detector
G4ThreeVector GetPosition() const;
/// Sets the position of the detector
void SetPosition(const G4ThreeVector&);

/// Returns the bin size of the histogram
G4double GetBinSize() const;
/// Sets the bin size of the histogram. This can only be done
/// while the histogram is empty (rebinning is not supported).
void SetBinSize(G4double);

/// Adds counts to a given time bin
void Fill(G4double time, G4int counts=1);

const std::map<G4double, G4int>& GetHistogram() const;

private:
G4int pmt_id_;           ///< Detector ID number
G4double bin_size_;      ///< Size of time bin
G4ThreeVector position_; ///< Detector position

/// Sparse histogram with number of photons detected per time bin
std::map<G4double, G4int> histogram_;
};


typedef G4THitsCollection<PmtHit> PmtHitsCollection;
extern G4Allocator<PmtHit> PmtHitAllocator;


// INLINE DEFINITIONS ////////////////////////////////////////////////

inline void* PmtHit::operator new(size_t)
{ return ((void*) PmtHitAllocator.MallocSingle()); }

inline void PmtHit::operator delete(void* hit)
{ PmtHitAllocator.FreeSingle((PmtHit*) hit); }

inline G4int PmtHit::GetPmtID() const { return pmt_id_; }
inline void PmtHit::SetPmtID(G4int id) { pmt_id_ = id; }

inline G4double PmtHit::GetBinSize() const { return bin_size_; }

inline G4ThreeVector PmtHit::GetPosition() const { return position_; }
inline void PmtHit::SetPosition(const G4ThreeVector& p) { position_ = p; }

inline const std::map<G4double, G4int>& PmtHit::GetHistogram() const
{ return histogram_; }


#endif