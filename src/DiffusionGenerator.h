//
// Created by ilker on 1/5/22.
//

#ifndef G4QPIX_DIFFUSIONGENERATOR_H
#define G4QPIX_DIFFUSIONGENERATOR_H

// GEANT4 includes
#include <G4VPrimaryGenerator.hh>
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4String.hh"


class G4ParticleDefinition;
class G4GenericMessenger;
class BaseGeometry;
class DiffusionGenerator : public G4VPrimaryGenerator{
public:
    DiffusionGenerator();
     ~DiffusionGenerator();
    //virtual void GeneratePrimaries(G4Event*);
    //virtual void GENIEGeneratePrimaries(G4Event*);

    // This method is invoked at the beginning of the event,
    // setting a primary vertex that contains the chosen ion
    void GeneratePrimaryVertex(G4Event*);
    void EventsWithWindow(G4Event *,G4double decay_time);
    void SavetheTruth(G4PrimaryParticle *const particle_gun_ , G4ThreeVector const pos_);


        protected:

    // GEANT4 dictionary of particles
    G4ParticleTable* particle_table_;

private:
    G4ParticleDefinition* IonDefinition();
    G4GenericMessenger* msg_; // Messenger for configuration parameters
    G4int atomic_number_,atomic_mass_;
    G4double energy_level_;
    G4bool decay_at_time_zero_;
    G4double Event_window_;
    G4int N_Decays_per_s_;
    G4String region_;
    G4int NDecays;

    G4double decay_time_;

    const BaseGeometry* geo_;
};

#endif //G4QPIX_DIFFUSIONGENERATOR_H
