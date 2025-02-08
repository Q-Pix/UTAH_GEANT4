//
// Created by ilker on 1/5/22.
//

#include "DiffusionGenerator.h"
#include "BaseGeometry.h"
#include "DetectorConstruction.h"
#include "FactoryBase.h"

// Q-Pix includes
#include "MCTruthManager.h"
#include "GeneratorParticle.h"

// GEANT4 includes
#include "G4PhysicalConstants.hh"
#include "G4LogicalVolumeStore.hh"

#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>

#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonTable.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"


#include "G4GenericMessenger.hh"
#include "Randomize.hh"

// C++ includes
#include <stdlib.h>
#include <math.h>

REGISTER_CLASS(DiffusionGenerator, G4VPrimaryGenerator)

DiffusionGenerator::DiffusionGenerator(): G4VPrimaryGenerator(),
                                          atomic_number_(0),
                                          atomic_mass_(0),
                                          energy_level_(0.),
                                          decay_at_time_zero_(true),
                                          Event_window_(0),
                                          N_Decays_per_s_(0.),
                                          region_(""),
                                          NDecays(1)//,
// Detector_Geometry_("NAPA")
{
    msg_ = new G4GenericMessenger(this, "/Generator/DiffusionGenerator/", "Control commands of the ion primary generator.");
    msg_->DeclareProperty("AtomicNumber",atomic_number_,"Atomic Number of the isotope ");
    msg_->DeclareProperty("AtomicMass",atomic_mass_,"Atomic Mass of the isotope ");
    msg_->DeclareProperty("DecayRate",N_Decays_per_s_,"Decay Rate in Decays/s(Bq) ");
    msg_->DeclareProperty("energy_level",energy_level_,"Energy Level ");
    msg_->DeclareProperty("EventWindow",Event_window_,"Event Windows  of the decay ");
    msg_->DeclareProperty("Region",region_,"Source Region");
    msg_->DeclareProperty("decay_at_time_zero", decay_at_time_zero_,"Set to true to make unstable isotopes decay at t=0.");

    // Load the detector geometry, which will be used for the generation of vertices
    const DetectorConstruction* detconst = dynamic_cast<const DetectorConstruction*>
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    if (detconst) geo_  = detconst->GetGeometry();
    else G4Exception("[DiffusionGenerator]", "DiffusionGenerator()", FatalException, "Unable to load geometry.");
}

DiffusionGenerator::~DiffusionGenerator()
{
    delete msg_;
}

/*void DiffusionGenerator::GeneratePrimaries(G4Event* event)
{
    // get MC truth manager
    MCTruthManager * mc_truth_manager = MCTruthManager::Instance();
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
    G4ThreeVector const & direction = particle_gun_->GetParticleMomentumDirection();

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
    GeneratorParticSetPDGCodele * particle = new GeneratorParticle();
    particle-> (pdg_code);
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


}*/
G4ParticleDefinition * DiffusionGenerator::IonDefinition() {
    G4ParticleDefinition * pdef= G4IonTable::GetIonTable()->GetIon(atomic_number_,atomic_mass_,energy_level_);
    if (!pdef) G4Exception("DiffusionGenerator","IonDefinition()",FatalException,"Could not find the ion");
    // Unstable ions decay by default at a random time t sampled from an exponential
    // decay distribution proportional to their mean lifetime. This, even for
    // not so long-lived nuclides, pushes the global time of the event to scales
    // orders of magnitude longer than the nanosecond precision required in NEXUS
    // for the correct simulation of the ionization drift and photon tracing.
    // To prevent this behaviour, the lifetime of unstable isotopes is reset here
    // (unless the configuration variable 'decay_at_time_zero' has been set to false)
    // to an arbitrary, short value (1 ps).
    if (decay_at_time_zero_ && !(pdef->GetPDGStable())) pdef->SetPDGLifeTime(1.*ps);

    return pdef;
}
void DiffusionGenerator::GeneratePrimaryVertex(G4Event* event)
{

    //Converting nCi to Bq 1nCi to 37 Bq
    G4int DecayRateinBq=(N_Decays_per_s_);
    G4int N_Decays=DecayRateinBq*Event_window_;
    //Printing Some Values
    /*G4cout<<"-----Printing Setting Values----" <<G4endl;

    if(event_window_>0){
        G4cout<<"NumberOfDecays--> "<<N_Decays<<G4endl;
        G4cout<<"event_window(us)--> "<<event_window_<<G4endl;
        G4cout<<"N_Decays_per_s(nCi)--> "<<N_Decays_per_s_<<G4endl;
        G4cout<<"DecayRateInBq--> "<<DecayRateinBq<<G4endl;
    }
    else{
        G4cout<<"NumberOfDecays--> "<<NDecays<<G4endl;
    }*/
    //if time window is zero then run sim for specified number of Particles
    if(Event_window_<=0){
        if (NDecays==0) NDecays=1;
        for(G4int i=0;i<NDecays;i++){
            decay_time_=Event_window_;
            EventsWithWindow(event,decay_time_);
        }
    }else{ //run sim for a given us time window
        for(G4int Decay=0;Decay<N_Decays;Decay++){
            decay_time_=G4UniformRand()*Event_window_;
            EventsWithWindow(event,decay_time_);
        }
    }

}
void DiffusionGenerator::EventsWithWindow(G4Event*event, G4double decay_time){
    // Pointer declared as static so that it gets allocated only once
    // (i.e. the ion definition is only looked up in the first event).
    static G4ParticleDefinition* pdef = IonDefinition();
    // Create the new primary particle (i.e. the ion)
    G4PrimaryParticle* ion = new G4PrimaryParticle(pdef);
    // Generate an initial position for the ion using the geometry
    G4ThreeVector position = geo_->GenerateVertex(region_);

    //Saving Particle Info
    SavetheTruth(ion,position);
    //G4ThreeVector position  (0,0,0);
    // Ion generated at the start-of-event time
    // Create a new vertex
    //G4cout<<"ParticlePosition " <<position<< " DecayTime "<< decay_time<<G4endl;
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position,decay_time);

    // Add ion to the vertex and this to the event
    vertex->SetPrimary(ion);
    event->AddPrimaryVertex(vertex);
}



void DiffusionGenerator::SavetheTruth(G4PrimaryParticle *const particle_gun_ , G4ThreeVector  const pos){

    // get MC truth manager
    MCTruthManager * mc_truth_manager = MCTruthManager::Instance();

    // get generated particle
 const  G4ParticleDefinition *  particle_definition = particle_gun_->GetParticleDefinition();

    int const pdg_code = particle_definition->GetPDGEncoding();
    double const charge = particle_definition->GetPDGCharge();
    double const mass = particle_definition->GetPDGMass() * CLHEP::MeV;


    G4ThreeVector const & momentum = particle_gun_->GetMomentum();

    double const x = pos.x() / CLHEP::cm;
    double const y = pos.y() / CLHEP::cm;
    double const z = pos.z() / CLHEP::cm;
    double const t = decay_time_;  // ns



    double const kinetic_energy = particle_gun_->GetKineticEnergy() * CLHEP::MeV;
    double const energy = kinetic_energy + mass;


    double const px = momentum[0] ;
    double const py = momentum[1];
    double const pz = momentum[2];

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
