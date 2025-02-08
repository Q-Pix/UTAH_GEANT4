//
// Created by ilker on 1/9/22.
// This is  Austin's Default Generator of UTA TPC
//

#include "UTA_TPC_ParticleGenerator.h"
#include "FactoryBase.h"
#include "BaseGeometry.h"
// Q-Pix includes
#include "MCTruthManager.h"
#include "GeneratorParticle.h"


// GEANT4 includes
#include "G4PhysicalConstants.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"

#include "G4Electron.hh"
#include "G4MuonPlus.hh"
#include "G4Proton.hh"


#include "G4GenericMessenger.hh"
#include "Randomize.hh"

#include "DetectorConstruction.h"
#include "G4RunManager.hh"



// C++ includes
#include <stdlib.h>
#include <math.h>
REGISTER_CLASS(UTA_TPC_ParticleGenerator, G4VPrimaryGenerator)
UTA_TPC_ParticleGenerator::UTA_TPC_ParticleGenerator()
        : G4VPrimaryGenerator(),
          decay_at_time_zero_(false)
// Detector_Geometry_("NAPA")
{
    msg_ = new G4GenericMessenger(this, "/Generator/UTA_TPC_ParticleGenerator/", "Control commands of the ion primary generator.");
    msg_->DeclareProperty("Particle_Type", Particle_Type_,  "which particle?");
    msg_->DeclareProperty("decay_at_time_zero", decay_at_time_zero_,
                          "Set to true to make unstable isotopes decay at t=0.");

    particle_gun_ = new G4GeneralParticleSource();

    // get dictionary of particles
    particle_table_ = G4ParticleTable::GetParticleTable();


}
UTA_TPC_ParticleGenerator::~UTA_TPC_ParticleGenerator()
{
    delete msg_;
    delete particle_gun_;
    // delete supernova_timing_;
    // delete super;
}


void UTA_TPC_ParticleGenerator::GeneratePrimaries(G4Event* event)
{
    // get MC truth manager
    MCTruthManager * mc_truth_manager = MCTruthManager::Instance();
    if (Particle_Type_ ==  "SUPERNOVA")
    {
        // super->Get_Detector_Dimensions(detector_length_x_, detector_length_y_, detector_length_z_);
        // super->Gen_Supernova_Background(event);
    }

    else if (Particle_Type_ ==  "MARLEY")
    {
        // this->MARLEYGeneratePrimaries(event);
    } else if(Particle_Type_=="GENIE"){
        //this->GENIEGeneratePrimaries(event);
    }

    else
    {
        particle_gun_->GeneratePrimaryVertex(event);

        // get generated particle
        G4ParticleDefinition * const particle_definition = particle_gun_->GetParticleDefinition();

        int const pdg_code = particle_definition->GetPDGEncoding();
        double const charge = particle_definition->GetPDGCharge();
        double const mass = particle_definition->GetPDGMass() * CLHEP::MeV;

        // if the particle is a nucleus
        if (pdg_code > 1000000000)
        {
            if (decay_at_time_zero_ && !(particle_definition->GetPDGStable()))
            {
                // make unstable isotopes decay at t=0
                particle_definition->SetPDGLifeTime(1.*CLHEP::ps);
            }
        } // if the particle is a nucleus

        G4ThreeVector const & position = particle_gun_->GetParticlePosition();

        G4ThreeVector const & direction =particle_gun_->GetParticleMomentumDirection();

        double const x = position.x() / CLHEP::cm;
        double const y = position.y() / CLHEP::cm;
        double const z = position.z() / CLHEP::cm;
        double const t = particle_gun_->GetParticleTime();  // ns

        double const dx = direction.x();
        double const dy = direction.y();
        double const dz = direction.z();

        double const kinetic_energy = particle_gun_->GetParticleEnergy() * CLHEP::MeV;
        double const energy = kinetic_energy + mass;

        double const momentum = std::sqrt(energy*energy - mass*mass);

        double const px = momentum * dx;
        double const py = momentum * dy;
        double const pz = momentum * dz;

        // create generator particle
        GeneratorParticle * particle = new GeneratorParticle();
        particle->SetPDGCode (pdg_code);
        particle->SetMass    (mass    );
        particle->SetCharge  (charge  );
        particle->SetX       (x       );
        particle->SetY       (y       );
        particle->SetZ       (z       );
        particle->SetT       (t       );
        particle->SetPx      (px      );
        particle->SetPy      (py      );
        particle->SetPz      (pz      );
        particle->SetEnergy  (energy  );

        // add to MC truth manager
        mc_truth_manager->AddInitialGeneratorParticle(particle);
        mc_truth_manager->AddFinalGeneratorParticle(particle);
    }

}

void UTA_TPC_ParticleGenerator::GeneratePrimaryVertex(G4Event * event) {
    GeneratePrimaries(event);
}