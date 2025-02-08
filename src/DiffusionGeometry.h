//
// Created by ilker on 1/4/22.
//

#ifndef G4QPIX_DIFFUSIONGEOMETRY_H
#define G4QPIX_DIFFUSIONGEOMETRY_H

#include "BaseGeometry.h"


class G4Material;
class G4GenericMessenger;
class G4MaterialPropertiesTable;
class G4PVPlacement;

class DiffusionGeometry: public BaseGeometry {
public:


    DiffusionGeometry();

     ~DiffusionGeometry();
     void SetVisuals();


private:

    virtual void Construct();
    virtual G4ThreeVector GenerateVertex(const G4String& region) const;



    G4GenericMessenger *msg_; // Messenger for configuration parameters
    G4String Name_;
    G4double Temperature_;
    G4double Pressure_;
    //For Detector Dimensions
    G4double Det_rmin_;
    G4double Det_rmax_;
    G4double Det_z_;

    // For Active Area Dimensions in the detector
    G4double DetAct_rmin_;
    G4double DetAct_rmax_;
    G4double DetAct_z_;

    // For Source Holder Dimensions in the detector
    G4double SourceHolder_rmin_;
    G4double SourceHolder_rmax_;
    G4double SourceHolder_z_;

     Hep3Vector vtx_;
     G4double world_size_;
    G4double phi_min ;
    G4double phi_max ;
    Hep3Vector offset_;
    Hep3Vector SourceHolderPosition_;
    bool HideSourceHolder_ ;


 };




#endif //G4QPIX_DIFFUSIONGEOMETRY_H