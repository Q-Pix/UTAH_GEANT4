// -----------------------------------------------------------------------------
//  G4_QPIX | DetectorConstruction.h
//
//  Class for the definition of the detector geometry and materials.
//   * Author: Justo Martin-Albo
//   * Creation date: 14 Aug 2019
// -----------------------------------------------------------------------------

#ifndef DETECTOR_CONSTRUCTION_H
#define DETECTOR_CONSTRUCTION_H 1

#include "G4VUserDetectorConstruction.hh"

class G4GenericMessenger;
class G4MaterialPropertiesTable;
class BaseGeometry;
class DetectorConstruction: public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct();
    void SetGeometry(BaseGeometry*);
    virtual void ConstructSDandField();

    const BaseGeometry * GetGeometry() const;

    G4MaterialPropertiesTable* GetMyPhotOptSurf();
private:
    BaseGeometry * geo_;

  // G4String Detector_Geometry_;

};
inline void DetectorConstruction::SetGeometry(BaseGeometry* g)
{ geo_ = g; }

inline const BaseGeometry* DetectorConstruction::GetGeometry() const
{ return geo_; }


#endif
