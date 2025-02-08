// -----------------------------------------------------------------------------
//  G4_QPIX | DetectorConstruction.cpp
//
//  Definition of detector geometry and materials.
//   * Author: Justo Martin-Albo
//   * Creation date: 14 Aug 2019
// -----------------------------------------------------------------------------

#include "DetectorConstruction.h"
#include "BaseGeometry.h"
#include "TrackingSD.h"


#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"

#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

DetectorConstruction::DetectorConstruction(): geo_(0)//, Detector_Geometry_("APA")
{

}

DetectorConstruction::~DetectorConstruction()
{
  // delete messenger_;
  delete geo_;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    if(!geo_){
        G4Exception("[DetectorConstruction]", "Construct()",
                    FatalException, "Geometry not set!");
    }
    G4LogicalVolume* World_logic;
    geo_->SetWorldLogicalVolume(World_logic);
    geo_->Construct();

    // We define now the world volume as an empty box big enough
    // to fit the user's geometry inside.

    G4double size = geo_->GetSpan();

    G4Box* World_solid = new G4Box("World", size/2., size/2., size/2.);

    G4Material* vacuum =
            G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

     World_logic =
            new G4LogicalVolume(World_solid, vacuum, "World", 0, 0, 0, true);

    World_logic->SetVisAttributes(G4VisAttributes::GetInvisible);
    G4PVPlacement* World_physi =
            new G4PVPlacement(0, G4ThreeVector(), World_logic, "World", 0, false, 0, true);

    // We place the user's geometry in the center of the World
    G4LogicalVolume* geometry_logic = geo_->GetLogicalVolume();

    new G4PVPlacement(0,G4ThreeVector(0,0,0),geometry_logic, geometry_logic->GetName(), World_logic, false, 0,true);

    return World_physi;

}

void DetectorConstruction::ConstructSDandField()
{
  // SENSITIVE DETECTOR ////////////////////////////////////
  // PmtSD* pmtsd = new PmtSD("/G4QPIX/PHOTON", "PmtHitsCollection");
  // G4SDManager::GetSDMpointer()->AddNewDetector(pmtsd);

  // G4LogicalVolume* optical_logic_vol =
  //   G4LogicalVolumeStore::GetInstance()->GetVolume("SENSAREA");

  // SetSensitiveDetector(optical_logic_vol, pmtsd);




  TrackingSD* tracking_sd = new TrackingSD("/G4QPIX/TRACKING", "TrackingHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(tracking_sd);

  //G4LogicalVolume* detector_logic_vol =G4LogicalVolumeStore::GetInstance()->GetVolume("detector.logical");
  G4LogicalVolume* detector_logic_vol = geo_->GetActiveLogicalVolume();

  SetSensitiveDetector(detector_logic_vol, tracking_sd);

  //////////////////////////////////////////////////////////
}
