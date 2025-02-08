//
// Created by ilker on 1/9/22.
//

#ifndef G4QPIX_UTA_TPC_PARTICLEGENERATOR_H
#define G4QPIX_UTA_TPC_PARTICLEGENERATOR_H
// GEANT4 includes

#include <G4VPrimaryGenerator.hh>
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4String.hh"
#include "BaseGeometry.h"


class G4ParticleDefinition;
class G4GenericMessenger;
class G4Event;
class G4GenericMessenger;
class G4ParticleDefinition;

class UTA_TPC_ParticleGenerator:public G4VPrimaryGenerator {
public:
    UTA_TPC_ParticleGenerator();
    ~UTA_TPC_ParticleGenerator();
    virtual void GeneratePrimaries(G4Event*);
    void GeneratePrimaryVertex(G4Event*);

protected:

    // GEANT4 dictionary of particles
    G4ParticleTable* particle_table_;
private:

    G4GenericMessenger* msg_; // Messenger for configuration parameters
    G4String Particle_Type_;
    G4String Detector_Geometry_;

    bool decay_at_time_zero_;

    G4GeneralParticleSource * particle_gun_;
    G4String region_;
    G4double energy_;
    G4ThreeVector momentumDirection_;
};


#endif //G4QPIX_UTA_TPC_PARTICLEGENERATOR_H
