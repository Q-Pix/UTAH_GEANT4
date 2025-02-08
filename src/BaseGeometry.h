// ----------------------------------------------------------------------------
// nexus | BaseGeometry.h
//
// This is an abstract base class for encapsulation of geometries.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#ifndef BASE_GEOMETRY_H
#define BASE_GEOMETRY_H 1

#include <G4ThreeVector.hh>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4MaterialPropertiesTable.hh"
class G4LogicalVolume;
class G4PVPlacement;
using namespace CLHEP;

/// Abstract base class for encapsulation of detector geometries.

class BaseGeometry
{
public:
/// The volumes (solid, logical and physical) must be defined
/// in this method, which will be invoked during the detector
/// construction phase
virtual void Construct() = 0;

/// Returns the logical volume representing the geometry
G4LogicalVolume* GetLogicalVolume() const;




//////// Added by Ilker ///////
/// Returns the logical volume of Active Area
void SetActiveLogicalVolume(G4LogicalVolume* actlv) ;
G4LogicalVolume* GetActiveLogicalVolume() const;
void SetDetectorPosition(Hep3Vector offset);
Hep3Vector GetDetectorPosition() const;
G4LogicalVolume* GetDetLogicalVolume() const;
G4LogicalVolume* GetSourceHolderLogicalVolume() const;
void SetDetLogicalVolume(G4LogicalVolume*) ;
void SetSourceHolderLogicalVolume(G4LogicalVolume*) ;
G4LogicalVolume* GetWorldLogicalVolume() const;
void SetWorldLogicalVolume(G4LogicalVolume* worldlv);




//////////////////////
/// Returns the Optical Stuffs

//virtual  G4MaterialPropertiesTable * GetMyPhotOptSurf() ;


/// Returns a point within a given region of the geometry
virtual G4ThreeVector GenerateVertex(const G4String&) const;



/// Returns the span (maximum dimension) of the geometry
G4double GetSpan();

/// Returns true if geometry has a drift field
G4bool GetDrift() const;

// Getter for the starting point of EL generation in z
G4double GetELzCoord() const;

/// Setter for the starting point of EL generation in z
void SetELzCoord(G4double z);

/// Translates position to G4 global position
void CalculateGlobalPos(G4ThreeVector& vertex) const;

/// Destructor
virtual ~BaseGeometry();

protected:
/// Default constructor defined as protected so no instance of
/// this base class can be created.
BaseGeometry();

/// Sets the logical volume representing the geometry
void SetLogicalVolume(G4LogicalVolume*);



/// Sets the span (maximum dimension) of the geometry
void SetSpan(G4double);

/// Sets the drift variable to true if a drift field exists
void SetDrift(G4bool);

private:
/// Copy-constructor (hidden)
BaseGeometry(const BaseGeometry&);
/// Assignment operator (hidden)
const BaseGeometry& operator=(const BaseGeometry&);

private:
G4LogicalVolume* logicVol_; ///< Pointer to the logical volume
G4LogicalVolume* ActivelogicVol_; ///< Pointer to the logical volume
G4LogicalVolume* WorldlogicVol_; ///< Pointer to the logical volume
G4LogicalVolume* DetlogicVol_; ///< Pointer to the logical volume
G4LogicalVolume* SourceHollogicVol_; ///< Pointer to the logical volume
Hep3Vector offset_;
G4double span_; ///< Maximum dimension of the geometry
G4bool drift_; ///< True if geometry contains a drift field (for hit coordinates)
G4double el_z_; ///< Starting point of EL generation in z
};


// Inline definitions ///////////////////////////////////

inline BaseGeometry::BaseGeometry(): ActivelogicVol_(0),DetlogicVol_(0),WorldlogicVol_(0),logicVol_(0),offset_(0), span_(25.*m), drift_(false), el_z_(0.*mm) {}

inline BaseGeometry::~BaseGeometry() {}

inline G4LogicalVolume* BaseGeometry::GetLogicalVolume() const
{ return logicVol_; }
inline G4LogicalVolume* BaseGeometry::GetWorldLogicalVolume() const
{ return WorldlogicVol_; }

inline void BaseGeometry::SetWorldLogicalVolume(G4LogicalVolume* worldlv)
{ WorldlogicVol_ = worldlv; }

inline G4LogicalVolume* BaseGeometry::GetActiveLogicalVolume() const
{ return ActivelogicVol_; }

inline void BaseGeometry::SetActiveLogicalVolume(G4LogicalVolume* actlv)
{ ActivelogicVol_ = actlv; }

inline G4LogicalVolume* BaseGeometry::GetDetLogicalVolume() const
{ return DetlogicVol_; }

inline void BaseGeometry::SetSourceHolderLogicalVolume(G4LogicalVolume* srhlv)
{ SourceHollogicVol_ = srhlv; }
inline void BaseGeometry::SetDetLogicalVolume(G4LogicalVolume* detLogic)
{ DetlogicVol_ = detLogic; }

inline G4LogicalVolume* BaseGeometry::GetSourceHolderLogicalVolume() const
{ return SourceHollogicVol_;  }
inline  Hep3Vector BaseGeometry::GetDetectorPosition() const
{ return offset_; }

inline void BaseGeometry::SetDetectorPosition(Hep3Vector offset)
{ offset_ = offset; }


inline void BaseGeometry::SetLogicalVolume(G4LogicalVolume* lv)
{ logicVol_ = lv; }

inline G4ThreeVector BaseGeometry::GenerateVertex(const G4String&) const
{ return G4ThreeVector(0., 0., 0.); }

inline void BaseGeometry::SetSpan(G4double s) { span_ = s; }

inline G4double BaseGeometry::GetSpan() { return span_; }

inline void BaseGeometry::SetDrift(G4bool drift) { drift_ = drift; }

inline G4bool BaseGeometry::GetDrift() const { return drift_; }

inline G4double BaseGeometry::GetELzCoord() const {return el_z_;}

inline void BaseGeometry::SetELzCoord(G4double z) {el_z_ = z;}

// This methods is to be used only in the Next1EL and NEW geometries
inline void BaseGeometry::CalculateGlobalPos(G4ThreeVector& vertex) const
{
// Rotates and translates into G4 global
// coordinates, needed for volume checks.
vertex.rotate(pi, G4ThreeVector(0., 1., 0.));

vertex += G4ThreeVector(0., 0., GetELzCoord());
}

#endif