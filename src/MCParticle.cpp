// -----------------------------------------------------------------------------
//  MCParticle.cpp
//
//  Class definition of MCParticle
//   * Author: Everybody is an author!
//   * Creation date: 7 August 2020
// -----------------------------------------------------------------------------

#include "MCParticle.h"

// GEANT4 includes
#include "G4VProcess.hh"

//-----------------------------------------------------------------------------
MCParticle::MCParticle()
{}

//-----------------------------------------------------------------------------
MCParticle::~MCParticle()
{}

//-----------------------------------------------------------------------------
// void MCParticle::AddTrajectoryPoint(const TrajectoryPoint & point)
// {
//     trajectory_.push_back(point);
// }

//-----------------------------------------------------------------------------
void MCParticle::AddTrajectoryHit(const TrajectoryHit & hit)
{
    // G4cout << "HIT TIME2, " << hit.StartTime() <<"\t"<< hit.EndPoint().X() <<"\t"<< hit.EndPoint().Y() <<"\t"<< hit.EndPoint().Z() << G4endl;
    hits_.push_back(hit);
    // G4cout << "HIT TIME1, " << hits_.back().StartTime() <<"\t"<< hits_.back().EndPoint().X() <<"\t"<< hits_.back().EndPoint().Y() <<"\t"<< hits_.back().EndPoint().Z() << G4endl;
    // G4cout << "HIT size, " << hits_.size() << G4endl;
}

//-----------------------------------------------------------------------------
void MCParticle::AddTrajectoryHit(const G4Step * step)
{
    // initialize hit
    TrajectoryHit hit;
    hit.start_ = { step->GetPreStepPoint()->GetPosition().x()/CLHEP::cm,
                   step->GetPreStepPoint()->GetPosition().y()/CLHEP::cm,
                   step->GetPreStepPoint()->GetPosition().z()/CLHEP::cm, };
    hit.end_   = { step->GetPostStepPoint()->GetPosition().x()/CLHEP::cm,
                   step->GetPostStepPoint()->GetPosition().y()/CLHEP::cm,
                   step->GetPostStepPoint()->GetPosition().z()/CLHEP::cm, };
    hit.energy_deposit_ = step->GetTotalEnergyDeposit() / CLHEP::MeV;
    hit.start_time_ = step->GetPreStepPoint()->GetGlobalTime() / CLHEP::ns;
    hit.end_time_   = step->GetPostStepPoint()->GetGlobalTime() / CLHEP::ns;
    hit.track_id_   = step->GetTrack()->GetTrackID();
    hit.pdg_code_   = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    hit.length_     = step->GetStepLength() / CLHEP::cm;
    hit.process_    = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    // add hit
    this->AddTrajectoryHit(hit);
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MCParticle::AddPmtHit(const TrajectoryHit & hit)
{
    pmt_hits_.push_back(hit);
}

//-----------------------------------------------------------------------------
void MCParticle::AddPmtHit(const G4Step * step)
{
    // initialize hit
    TrajectoryHit hit;
    hit.start_ = { step->GetPreStepPoint()->GetPosition().x()/CLHEP::cm,
                   step->GetPreStepPoint()->GetPosition().y()/CLHEP::cm,
                   step->GetPreStepPoint()->GetPosition().z()/CLHEP::cm, };
    hit.end_   = { step->GetPostStepPoint()->GetPosition().x()/CLHEP::cm,
                   step->GetPostStepPoint()->GetPosition().y()/CLHEP::cm,
                   step->GetPostStepPoint()->GetPosition().z()/CLHEP::cm, };
    hit.energy_deposit_ = step->GetTotalEnergyDeposit() / CLHEP::MeV;
    hit.start_time_ = step->GetPreStepPoint()->GetGlobalTime() / CLHEP::ns;
    hit.end_time_   = step->GetPostStepPoint()->GetGlobalTime() / CLHEP::ns;
    hit.track_id_   = step->GetTrack()->GetTrackID();
    hit.pdg_code_   = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    hit.length_     = step->GetStepLength() / CLHEP::cm;
    hit.process_    = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();

    // add hit
    this->AddPmtHit(hit);
}
