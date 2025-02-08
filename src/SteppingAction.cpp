// -----------------------------------------------------------------------------
//  G4_QPIX | SteppingAction.cpp
//
//  Definition of detector geometry and materials.
//   * Author: Justo Martin-Albo
//   * Creation date: 14 Aug 2019
// -----------------------------------------------------------------------------

#include "SteppingAction.h"
#include "FactoryBase.h"
#include "G4GenericMessenger.hh"

#include "AnalysisManager.h"
#include "G4VProcess.hh"

#include <G4OpticalPhoton.hh>

REGISTER_CLASS(SteppingAction,G4UserSteppingAction)
SteppingAction::SteppingAction(): G4UserSteppingAction()
{
    msg_ = new G4GenericMessenger(this, "/Actions/SteppingAction/");
}


SteppingAction::~SteppingAction()
{
    delete msg_;
}


void SteppingAction::UserSteppingAction(const G4Step* step)
{
    AnalysisManager * analysis_manager = AnalysisManager::Instance();

    analysis_manager->AddProcess(step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());

}
