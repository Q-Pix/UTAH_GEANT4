// -----------------------------------------------------------------------------
//  G4_QPIX | PrimaryGeneration.cpp
//
//  Class for the definition of the primary generation action.
//   * Author: Everybody is an author!
//   * Creation date: 14 Aug 2019
// -----------------------------------------------------------------------------

#include "PrimaryGeneration.h"

// Q-Pix includes
#include "MCTruthManager.h"
#include "GeneratorParticle.h"

// GEANT4 includes
#include "G4PhysicalConstants.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonTable.hh"
#include <G4VPrimaryGenerator.hh>

#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"


#include "G4Proton.hh"


#include "G4GenericMessenger.hh"

// C++ includes
#include <stdlib.h>
#include <math.h>

PrimaryGeneration::PrimaryGeneration()
  : G4VUserPrimaryGeneratorAction()
{
}


PrimaryGeneration::~PrimaryGeneration()
{

}


void PrimaryGeneration::GeneratePrimaries(G4Event* event)
{
    if (!generator_)
        G4Exception("[PrimaryGeneration]", "GeneratePrimaries()",
                    FatalException, "Generator not set!");

    generator_->GeneratePrimaryVertex(event);
}


