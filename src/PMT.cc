// ----------------------------------------------------------------------------
// nexus | PmtR11410.cc
//
// Geometry of the Hamamatsu R11410 photomultiplier.
//
// The NEXT Collaboration
// ----------------------------------------------------------------------------

#include "PmtR11410.h"
#include "MaterialsList.h"
#include "OpticalMaterialProperties.h"
#include "PmtSD.h"
#include "CylinderPointSampler.h"
#include "Visibilities.h"

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4VisAttributes.hh>
#include <Randomize.hh>
#include <G4OpticalSurface.hh>
#include <G4GenericMessenger.hh>

#include <CLHEP/Units/SystemOfUnits.h>

using namespace CLHEP;


PmtR11410::PmtR11410():
BaseGeometry(),
// Dimensions
front_body_diam_ (76. * mm),
front_body_length_ (38. * mm),
rear_body_diam_ (53. * mm),
rear_body_length_ (76. * mm),
body_thickness_ (.5 * mm),       // To be checked
window_thickness_ (2. * mm),
photocathode_diam_ (64. * mm),
photocathode_thickness_ (.1 * mm),
visibility_(1),
sd_depth_(-1),
binning_(100.*nanosecond)
{
msg_ = new G4GenericMessenger(this, "/Geometry/PmtR11410/",
                "Control commands of PmtR11410 geometry.");

msg_->DeclareProperty("visibility", visibility_, "Hamamatsu R11410 PMTs visibility");

G4GenericMessenger::Command& bin_cmd =
    msg_->DeclareProperty("time_binning", binning_,
            "Time binning of R11410 PMT");
bin_cmd.SetUnitCategory("Time");
bin_cmd.SetParameterName("time_binning", false);
bin_cmd.SetRange("time_binning>0.");
}



void PmtR11410::Construct()
{
// PMT BODY //////////////////////////////////////////////////////

G4Tubs* front_body_solid =
    new G4Tubs("FRONT_BODY", 0., front_body_diam_/2., front_body_length_/2.,
    0., twopi);

G4Tubs* rear_body_solid =
    new G4Tubs("REAR_BODY", 0., rear_body_diam_/2., rear_body_length_/2.,
    0., twopi);

// Union of the two volumes of the phototube body
G4double z_transl = -front_body_length_/2. - rear_body_length_/2.;
G4ThreeVector transl(0., 0., z_transl);
G4UnionSolid* pmt_solid =
    new G4UnionSolid("PMT_R11410",front_body_solid,rear_body_solid,0,transl);


G4Material* Kovar = MaterialsList::Kovar();

G4LogicalVolume* pmt_logic =
    new G4LogicalVolume(pmt_solid, Kovar, "PMT_R11410");
this->SetLogicalVolume(pmt_logic);


// PMT GAS  //////////////////////////////////////////////////////

G4double front_body_gas_diam = front_body_diam_ - 2. * body_thickness_;
G4double front_body_gas_length = front_body_length_ - body_thickness_;
G4Tubs* front_body_gas_solid =
    new G4Tubs("FRONT_BODY_GAS", 0., front_body_gas_diam/2., front_body_gas_length/2., 0., twopi);

G4double rear_body_gas_diam = rear_body_diam_ - 2. * body_thickness_;
G4double rear_body_gas_length = rear_body_length_;
G4Tubs* rear_body_gas_solid =
    new G4Tubs("REAR_BODY_GAS", 0., rear_body_gas_diam/2., rear_body_gas_length/2., 0., twopi);

// Union of the two volumes of the phototube body gas
G4double z_gas_transl = -front_body_gas_length/2. - rear_body_gas_length/2.;
G4ThreeVector gas_transl(0., 0., z_gas_transl);
G4UnionSolid* pmt_gas_solid = new G4UnionSolid("PMT_GAS", front_body_gas_solid,
                        rear_body_gas_solid, 0, gas_transl);

G4Material* pmt_gas_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
G4LogicalVolume* pmt_gas_logic = new G4LogicalVolume(pmt_gas_solid, pmt_gas_mat, "PMT_GAS");

G4double pmt_gas_posz = body_thickness_/2.;
new G4PVPlacement(0, G4ThreeVector(0., 0., pmt_gas_posz), pmt_gas_logic,
            "PMT_GAS", pmt_logic, false, 0);


// PMT WINDOW ////////////////////////////////////////////////////

window_diam_ = front_body_gas_diam;
G4Tubs* window_solid =
    new G4Tubs("PMT_WINDOW", 0, window_diam_/2., window_thickness_/2., 0., twopi);

G4Material* silica = MaterialsList::FusedSilica();
silica->SetMaterialPropertiesTable(OpticalMaterialProperties::FusedSilica());
G4LogicalVolume* window_logic = new G4LogicalVolume(window_solid, silica, "PMT_WINDOW");

G4double window_posz = front_body_gas_length/2. - window_thickness_/2.;
new G4PVPlacement(0, G4ThreeVector(0.,0.,window_posz), window_logic,
            "PMT_WINDOW", pmt_gas_logic, false, 0);


// PMT PHOTOCATHODE  /////////////////////////////////////////////

G4Tubs* photocathode_solid =
    new G4Tubs("PMT_PHOTOCATHODE", 0, photocathode_diam_/2., photocathode_thickness_/2.,
        0., twopi);

G4Material* aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
G4LogicalVolume* photocathode_logic =
    new G4LogicalVolume(photocathode_solid, aluminum, "PMT_PHOTOCATHODE");

G4double photocathode_posz = window_posz - window_thickness_/2. - photocathode_thickness_/2.;
new G4PVPlacement(0, G4ThreeVector(0., 0., photocathode_posz), photocathode_logic,
            "PMT_PHOTOCATHODE", pmt_gas_logic, false, 0);

// Optical properties
G4OpticalSurface* pmt_opt_surf = GetPhotOptSurf();
new G4LogicalSkinSurface("PMT_PHOTOCATHODE", photocathode_logic, pmt_opt_surf);

// Sensitive detector
PmtSD* pmtsd = new PmtSD("/PMT_R11410/PmtR11410");
if (sd_depth_ == -1) 
    G4Exception("[PmtR11410]", "Construct()", FatalException,
                "Sensor Depth must be set before constructing");
pmtsd->SetDetectorVolumeDepth(sd_depth_);
pmtsd->SetTimeBinning(binning_);
G4SDManager::GetSDMpointer()->AddNewDetector(pmtsd);
photocathode_logic->SetSensitiveDetector(pmtsd);


// VISIBILITIES //////////////////////////////////////////////////
pmt_gas_logic->SetVisAttributes(G4VisAttributes::Invisible);
window_logic->SetVisAttributes(G4VisAttributes::Invisible);
if (visibility_) {
    G4VisAttributes pmt_col = nexus::LightGrey();
    pmt_col.SetForceSolid(true);
    pmt_logic->SetVisAttributes(pmt_col);
    G4VisAttributes phot_col = nexus::Brown();
    phot_col.SetForceSolid(true);
    photocathode_logic->SetVisAttributes(phot_col);
} else {
    pmt_logic->SetVisAttributes(G4VisAttributes::Invisible);
    photocathode_logic->SetVisAttributes(G4VisAttributes::Invisible);
}



PmtR11410::~PmtR11410()
{
delete front_body_gen_;
delete medium_body_gen_;
delete rear_body_gen_;
delete rear_cap_gen_;
delete front_cap_gen_;
}



G4ThreeVector PmtR11410::GetRelPosition()
{
return G4ThreeVector(0., 0., front_body_length_/2.);
}



G4OpticalSurface* PmtR11410::GetPhotOptSurf()
{
const G4int entries = 57;
G4double ENERGIES[entries] =
    {	2.06640321682 *eV, 2.10142700016 *eV,  2.13765850016 *eV,  2.17516128086 *eV,
2.21400344659 *eV, 2.25425805471 *eV,  2.29600357425 *eV,  2.3393243964 *eV,
2.38431140402 *eV, 2.43106260802 *eV,  2.47968386018 *eV,  2.53028965325 *eV,
2.58300402103 *eV, 2.63796155339 *eV,  2.69530854368 *eV,  2.75520428909 *eV,
2.81782256839 *eV, 2.8833533258 *eV,   2.95200459546 *eV,  3.02400470754 *eV,
3.09960482523 *eV, 3.17908187203 *eV,  3.2627419213 *eV,   3.35092413538 *eV,
3.44400536137 *eV, 3.54240551455 *eV,  3.64659391204 *eV,  3.75709675786 *eV,
3.87450603154 *eV, 3.99949009707 *eV,  4.13280643364 *eV,  4.27531700032 *eV,
4.42800689319 *eV, 4.59200714849 *eV,  4.76862280805 *eV,  4.95936772037 *eV,
5.16600804205 *eV, 5.39061708736 *eV,  5.63564513678 *eV,  5.90400919092 *eV,
6.19920965046 *eV, 6.358163744063561 *eV, 6.525483842591549 *eV, 6.7018482707697 *eV,
6.888010722735524 *eV, 7.084811029099397 *eV, 7.293187824072908 *eV, 7.314701652462505 *eV,
7.336342781611801 *eV, 7.358112344761985 *eV, 7.380011488645204 *eV, 7.402041373685937 *eV,
7.424203174205955 *eV, 7.446498078633 *eV, 7.4689272897132195 *eV, 7.491492024727459 *eV,
7.51419351571 *eV};

G4double EFFICIENCY[entries] =
    { 0.0530,	0.0625, 0.0720, 0.0850,
0.1050, 0.1190, 0.1335, 0.1550,
0.1770, 0.1970, 0.2100, 0.2200,
0.2300,	0.2430, 0.2580, 0.2770,
0.2920,	0.3050, 0.3150, 0.3270,
0.3320, 0.3400, 0.3480, 0.3500,
0.3530,	0.3600, 0.3680, 0.3650,
0.3640, 0.3640, 0.3560, 0.3420,

0.3280, 0.3180, 0.3050, 0.2980,
0.2920,	0.2900, 0.2920, 0.2945,
0.3100,	0.3280, 0.3560, 0.3880,
0.3920,	0.3900, 0.4040, 0.3930,
0.3700,	0.3500, 0.3300, 0.3150,
0.2950,	0.2750, 0.2550, 0.2450,
0.2400 };

G4double REFLECTIVITY[entries] =
    { 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0. };

G4MaterialPropertiesTable* phcath_mpt = new G4MaterialPropertiesTable();
phcath_mpt->AddProperty("EFFICIENCY", ENERGIES, EFFICIENCY, entries);
phcath_mpt->AddProperty("REFLECTIVITY", ENERGIES, REFLECTIVITY, entries);

G4OpticalSurface* opt_surf =
    new G4OpticalSurface("PHOTOCATHODE", unified, polished, dielectric_metal);
opt_surf->SetMaterialPropertiesTable(phcath_mpt);

return opt_surf;
}
