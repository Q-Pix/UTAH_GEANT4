//
// Created by ilker on 1/9/22.
//

#ifndef G4QPIX_UTA_TPC_GEOMETRY_H
#define G4QPIX_UTA_TPC_GEOMETRY_H

#include "BaseGeometry.h"
class G4Material;
class G4GenericMessenger;
class G4MaterialPropertiesTable;
class UTA_TPC_Geometry :public BaseGeometry{
public:
    UTA_TPC_Geometry();/// Constructor
    ~UTA_TPC_Geometry();/// Destructor

    virtual void Construct();
    virtual G4ThreeVector GenerateVertex(const G4String& region) const;
    G4MaterialPropertiesTable* GetMyPhotOptSurf() ;
private:
    G4ThreeVector vtx_;
    G4GenericMessenger *msg_; // Messenger for configuration parameters

};


#endif //G4QPIX_UTA_TPC_GEOMETRY_H
