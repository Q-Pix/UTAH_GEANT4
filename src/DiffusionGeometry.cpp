//
// Created by ilker on 1/4/22.
//
#include "DiffusionGeometry.h"
#include "TrackingSD.h"
#include "OpticalMaterialProperties.h"
#include "MaterialsList.h"
#include "FactoryBase.h"
// #include "GenericPhotosensor.h"
#include "PmtSD.h"

#include "CADMesh.hh"
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4GenericMessenger.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4RotationMatrix.hh"



// Construct
REGISTER_CLASS(DiffusionGeometry, BaseGeometry)
DiffusionGeometry::DiffusionGeometry(): BaseGeometry(),
                                        msg_(nullptr),
                                        world_size_(55*cm),
                                        Name_("GasAr"),
                                        Temperature_(293.15*kelvin),
                                        Pressure_(10*bar),
                                        offset_(0,0,0),
                                        Det_rmin_(0),
                                        Det_rmax_(4.6*cm),
                                        Det_z_(12.5*cm),
                                        DetAct_rmin_(0),
                                        DetAct_rmax_(2.6*cm),
                                        DetAct_z_(10*cm),
                                        SourceHolder_rmin_(0),
                                        SourceHolder_rmax_(1*cm),
                                        SourceHolder_z_(3*mm),
                                        vtx_(0,0,0),
                                        phi_min(0.0),
                                        phi_max (2 * M_PI),
                                        HideSourceHolder_(false)
{
    msg_ = new G4GenericMessenger(this, "/Geometry/DiffusionGeometry/", "Control commands of the ion primary generator.");
    msg_->DeclarePropertyWithUnit("world_size","cm",world_size_ ,  "Size of the World");
    msg_->DeclareProperty("MaterialName",Name_ ,  "Name of the material");
    msg_->DeclareProperty("HideSourceHolder",HideSourceHolder_ ,  "This Hides SourceHolder");
    msg_->DeclarePropertyWithUnit("Pressure","bar",Pressure_ ,  "Gas Pressure");
    msg_->DeclarePropertyWithUnit("offset","cm",offset_ ,  "Gas Pressure");
    msg_->DeclarePropertyWithUnit("Temperature","kelvin",Temperature_ ,  "Temperature");
    msg_->DeclarePropertyWithUnit("Det_rmin","cm",Det_rmin_ ,  "Starting radius for cylindrical detector");
    msg_->DeclarePropertyWithUnit("Det_rmax","cm",Det_rmax_ ,  "Final radius for cylindrical detector");
    msg_->DeclarePropertyWithUnit("Det_z","cm",Det_z_ ,  "length of detector");
    msg_->DeclarePropertyWithUnit("DetAct_rmin","cm",DetAct_rmin_ ,  "Starting radius for fiducial");
    msg_->DeclarePropertyWithUnit("DetAct_rmax","cm",DetAct_rmax_ ,  "Final radius for fiducial");
    msg_->DeclarePropertyWithUnit("DetAct_z","cm",DetAct_z_ ,  "length of fiducial");
    msg_->DeclarePropertyWithUnit("SourceHolder_rmin","cm",SourceHolder_rmin_ ,  "Starting radius for SourceHolder");
    msg_->DeclarePropertyWithUnit("SourceHolder_rmax","cm",SourceHolder_rmax_ ,  "Final radius for SourceHolder");
    msg_->DeclarePropertyWithUnit("SourceHolder_z","cm",SourceHolder_z_ ,  "length of SourceHolder");
    msg_->DeclarePropertyWithUnit("vtx","cm",vtx_,"Source Position");
}


//Destruct
DiffusionGeometry::~DiffusionGeometry()  {}



//Detector Geometry
void DiffusionGeometry::Construct() {
    ////////////////////////////////// WORLD //////////////////////////////////


    this->SetSpan(world_size_);

    G4Material *mat;

    // GasAr or Liquid
    Name_=="GasAr" ? mat = MaterialsList::GasAr(Pressure_,Temperature_) : mat =MaterialsList::OLAr();
    //G4Material *DetecMat =MaterialsList::Steel();

    ////////////////////////////////// DETECTOR //////////////////////////////////

    G4Tubs* detector_solid_vol = new G4Tubs("detector.solid", Det_rmin_, Det_rmax_, Det_z_, phi_min, phi_max);

    G4LogicalVolume* detector_logic_vol = new G4LogicalVolume(detector_solid_vol, mat, "detector.logical");
    this->SetDetectorPosition(Hep3Vector(0,0,0));
    this->SetLogicalVolume(detector_logic_vol);


                    ////////////// DETECTOR Active Area //////////////

    G4Tubs* detectorActive_solid_vol = new G4Tubs("detectorActive.solid", DetAct_rmin_, DetAct_rmax_, DetAct_z_, phi_min, phi_max);

    G4LogicalVolume* detectorActive_logic_vol = new G4LogicalVolume(detectorActive_solid_vol, mat, "detectorActive.logical");
    //this->SetActiveLogicalVolume(detectorActive_logic_vol); // Setting active region to use as sensitive detector
    //this->SetActiveLogicalVolume(detector_logic_vol);

    new G4PVPlacement(0,offset_,detectorActive_logic_vol, detectorActive_logic_vol->GetName(),detector_logic_vol, false, 0,true);

    //Aluminum source holder
    if(!HideSourceHolder_) {
        G4Material *SourceHolderMat = MaterialsList::Al();
        G4Tubs *SourceHolder_solid_vol = new G4Tubs("SourceHolder.solid", SourceHolder_rmin_, SourceHolder_rmax_,
                                                    SourceHolder_z_, phi_min, phi_max);
        G4LogicalVolume *SourceHolder_logic_vol = new G4LogicalVolume(SourceHolder_solid_vol, SourceHolderMat,
                                                                      "SourceHolder.logical");
        this->SetSourceHolderLogicalVolume(SourceHolder_logic_vol);
        new G4PVPlacement(0, vtx_, SourceHolder_logic_vol, SourceHolder_logic_vol->GetName(), detector_logic_vol, false,
                          0, true);
    }
    // This is for an example including an stl file as an example
    /* auto g1=CADMesh::TessellatedMesh::FromSTL("/home/ilker/Projects/CADMesh/meshes/bunny.stl");
        g1->SetScale(0.1);
        g1->SetOffset(0,0,0);
        auto g1_solid=g1->GetSolid();
        G4LogicalVolume * g1_logic= new G4LogicalVolume(g1_solid,SourceHolderMat,"g1.logical");
        new G4PVPlacement(0,vtx_,g1_logic, g1_logic->GetName(),detector_logic_vol, false, 0,true);
    */

    SetVisuals();
    this->SetActiveLogicalVolume(detector_logic_vol);


}


G4ThreeVector DiffusionGeometry::GenerateVertex(const G4String& region) const
{
    G4ThreeVector SourcePosition;
    SourcePosition=vtx_;
    if(!(region=="detectorActive.logical")){

        G4Exception("[DiffusionGeometry]", "GenerateVertex()", FatalException,
                    "Unknown vertex generation region.");
    }
    if (!HideSourceHolder_)
        SourcePosition[2]=SourcePosition[2]-SourceHolder_z_;
    return SourcePosition;
}
void  DiffusionGeometry::SetVisuals() {

    G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();
    G4LogicalVolume* DetectorLogical = lvStore->GetVolume("detector.logical");
    G4LogicalVolume* DetectorActive = lvStore->GetVolume("detectorActive.logical");

    G4VisAttributes *DetActivVa=new G4VisAttributes(G4Colour(0,1,0));
    G4VisAttributes *DetLogicVa=new G4VisAttributes(G4Colour(0,0,1));


    DetActivVa->SetForceCloud(true);
    DetLogicVa->SetForceWireframe(true);
    DetectorActive->SetVisAttributes(DetActivVa);

    //DetectorLogical->SetVisAttributes(DetActivVa);
    DetectorLogical->SetVisAttributes(G4VisAttributes::GetInvisible);

    if(!HideSourceHolder_){
        G4LogicalVolume* SourceHolderLogical = lvStore->GetVolume("SourceHolder.logical");
        G4VisAttributes *SourceHolderLogicalVa=new G4VisAttributes(G4Colour(1,1,1));
        SourceHolderLogicalVa->SetForceSolid(true);
        SourceHolderLogical->SetVisAttributes(SourceHolderLogicalVa);
    }





}

