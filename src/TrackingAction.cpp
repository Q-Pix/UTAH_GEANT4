// -----------------------------------------------------------------------------
//  TrackingAction.cpp
//
//
//   * Author: Everybody is an author!
//   * Creation date: 4 August 2020
// -----------------------------------------------------------------------------

#include "TrackingAction.h"
#include "FactoryBase.h"
// Q-Pix includes
#include "MCParticle.h"
#include "MCTruthManager.h"

// GEANT4 includes
#include "G4TrackingManager.hh"
#include <G4GenericMessenger.hh>

// C++ includes
#include <iostream>
#include <fstream>

REGISTER_CLASS(TrackingAction,G4UserTrackingAction)
TrackingAction::TrackingAction():G4UserTrackingAction()
{
    msg_ = new G4GenericMessenger(this, "/Actions/TrackingAction/");
}

TrackingAction::~TrackingAction()
{
    delete msg_;
}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    // get MC truth manager
    MCTruthManager * mc_truth_manager = MCTruthManager::Instance();

    MCParticle * particle = new MCParticle();
    particle->SetTrackID(track->GetTrackID());
    particle->SetParentTrackID(track->GetParentID());
    particle->SetPDGCode(track->GetDefinition()->GetPDGEncoding());
    particle->SetMass(track->GetDynamicParticle()->GetMass());
    particle->SetCharge(track->GetDynamicParticle()->GetCharge());
    particle->SetGlobalTime(track->GetGlobalTime() / CLHEP::ns);
    // particle->SetProcess();
    particle->SetTotalOccupancy(track->GetDynamicParticle()->GetTotalOccupancy());

    particle->SetInitialPosition(
        TLorentzVector(
            track->GetPosition().x() / CLHEP::cm,
            track->GetPosition().y() / CLHEP::cm,
            track->GetPosition().z() / CLHEP::cm,
            track->GetGlobalTime()   / CLHEP::ns
        )
    );

    particle->SetInitialMomentum(
        TLorentzVector(
            track->GetMomentum().x() / CLHEP::MeV,
            track->GetMomentum().y() / CLHEP::MeV,
            track->GetMomentum().z() / CLHEP::MeV,
            track->GetTotalEnergy()  / CLHEP::MeV
        )
    );

    mc_truth_manager->AddMCParticle(particle);
    // if (track->GetDefinition()->GetPDGEncoding() == 11)
    // {
    //     G4cout << "\t"<< "Fuck me  " << track->GetTotalEnergy()  / CLHEP::MeV << G4endl;
    // }
    
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    // get MC truth manager
    MCTruthManager * mc_truth_manager = MCTruthManager::Instance();

    // get MC particle
    MCParticle * particle = mc_truth_manager->GetMCParticle(track->GetTrackID());

    // set process
    particle->SetProcess(track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
}

