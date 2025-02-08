// -----------------------------------------------------------------------------
//  G4_QPIX | PrimaryGeneration.h
//
//  Class for the definition of the primary generation action.
//   * Author: Everybody is an author!
//   * Creation date: 14 Aug 2019
// -----------------------------------------------------------------------------

#ifndef PRIMARY_GENERATION_H
#define PRIMARY_GENERATION_H

// GEANT4 includes
#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4String.hh"


class G4ParticleDefinition;
class G4GenericMessenger;

class PrimaryGeneration : public G4VUserPrimaryGeneratorAction
{

  public:
    PrimaryGeneration();
     ~PrimaryGeneration();
     void GeneratePrimaries(G4Event*);
     ///Returns a pointer to the primary generator
    const G4VPrimaryGenerator* GetGenerator() const;
    void SetGenerator(G4VPrimaryGenerator*);


protected:

    // GEANT4 dictionary of particles
    G4ParticleTable* particle_table_;

  private:

    G4GenericMessenger* msg_; // Messenger for configuration parameters
    G4VPrimaryGenerator* generator_;

};

inline void PrimaryGeneration::SetGenerator(G4VPrimaryGenerator* pg)
{ generator_ = pg; }

inline const G4VPrimaryGenerator* PrimaryGeneration::GetGenerator() const
{ return generator_; }

#endif
