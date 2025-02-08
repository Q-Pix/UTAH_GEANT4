//
// Created by ilker on 1/9/22.
// Implemented from Next
//

#include "QPIX_App.h"
#include "DetectorConstruction.h"
#include "PrimaryGeneration.h"
#include "RunAction.h"
#include "EventAction.h"
#include "TrackingAction.h"
#include "SteppingAction.h"
#include "FactoryBase.h"
#include "BatchSession.h"
#include <G4UImanager.hh>

#include <G4RunManager.hh>


#include <FTFP_BERT_HP.hh>
#include <G4EmStandardPhysics_option4.hh>
#include <G4OpticalPhysics.hh>


#include <G4GenericPhysicsList.hh>
#include <G4StateManager.hh>
#include <G4UserRunAction.hh>
#include <G4UserEventAction.hh>
#include <G4UserTrackingAction.hh>
#include <G4UserSteppingAction.hh>
#include "RTDCodeManager.h"




QPIX_App::QPIX_App(G4String init_macro): G4RunManager(), gen_name_(""),
                                         geo_name_(""), pm_name_(""),
                                         runact_name_(""), evtact_name_(""),
                                         stepact_name_(""), trkact_name_(""),
                                         stkact_name_("")
{
    // Create and configure a generic messenger for the app
    msg_ = new G4GenericMessenger(this, "/QPIX/", "QPIX control commands.");

    // Define the command to register a configuration macro.
    // The user may invoke the command as many times as needed.
    msg_->DeclareMethod("RegisterMacro", &QPIX_App::RegisterMacro, "");

    // Some commands, which we call 'delayed', only work if executed
    // after the initialization of the application. The user may include
    // them in configuration macros registered with the command defined below.
    msg_->DeclareMethod("RegisterDelayedMacro",
                        &QPIX_App::RegisterDelayedMacro, "");

    // Define a command to set a seed for the random number generator.
    msg_->DeclareMethod("random_seed", &QPIX_App::SetRandomSeed,
                        "Set a seed for the random number generator.");

// Define the command to set the desired generator
    msg_->DeclareProperty("RegisterGenerator", gen_name_, "");

    // Define the command to set the desired geometry
    msg_->DeclareProperty("RegisterGeometry", geo_name_, "");


// Define the commands to set the desired actions
    msg_->DeclareProperty("RegisterRunAction", runact_name_, "");
    msg_->DeclareProperty("RegisterEventAction", evtact_name_, "");
    msg_->DeclareProperty("RegisterSteppingAction", stepact_name_, "");
    msg_->DeclareProperty("RegisterTrackingAction", trkact_name_, "");
    msg_->DeclareProperty("RegisterStackingAction", stkact_name_, "");


    /////////////////////////////////////////////////////////

    // We will set now the user initialization class instances
    // in the run manager. In order to do so, we use our own factory
    // so that the messenger commands are already defined
    // by the time we process the initialization macro.

    // The physics lists are handled with Geant4's own 'factory'
    G4VModularPhysicsList* physics_list = new FTFP_BERT_HP(0);
    physics_list->ReplacePhysics(new G4EmStandardPhysics_option4());
    physics_list->RegisterPhysics(new G4OpticalPhysics());;
    //physicsList = new G4GenericPhysicsList();
    BatchSession* batch = new BatchSession(init_macro.c_str());
    batch->SessionStart();

    // Set the physics list in the run manager
    this->SetUserInitialization(physics_list);

    // Set the detector construction instance in the run manager
    DetectorConstruction* dc = new DetectorConstruction();
    if (geo_name_ == "") {
        G4Exception("[QPIX_App]", "QPIX_App()", FatalException, "A geometry must be specified.");
    }

    dc->SetGeometry(ObjFactory<BaseGeometry>::Instance().CreateObject(geo_name_));
    this->SetUserInitialization(dc);

    // Set the primary generation instance in the run manager
    PrimaryGeneration* pg = new PrimaryGeneration();
    if (gen_name_ == "") {
        G4Exception("[QPIX_App]", "QPIX_App()", FatalException, "A generator must be specified.");
    }
    pg->SetGenerator(ObjFactory<G4VPrimaryGenerator>::Instance().CreateObject(gen_name_));
    this->SetUserAction(pg);



    // Set the user action instances, if any, in the run manager
    if (runact_name_ != "")
        this->SetUserAction(ObjFactory<G4UserRunAction>::Instance().CreateObject(runact_name_));
    else
        this->SetUserAction(new RunAction());

    if (evtact_name_ != "")
        this->SetUserAction(ObjFactory<G4UserEventAction>::Instance().CreateObject(evtact_name_));
    else
        this->SetUserAction(new EventAction());

    if (stkact_name_ != "")
        this->SetUserAction(ObjFactory<G4UserStackingAction>::Instance().CreateObject(stkact_name_));

    if (trkact_name_ != "")
        this->SetUserAction(ObjFactory<G4UserTrackingAction>::Instance().CreateObject(trkact_name_));
    else
        this->SetUserAction(new TrackingAction());


    if (stepact_name_ != "")
        this->SetUserAction(ObjFactory<G4UserSteppingAction>::Instance().CreateObject(stepact_name_));
    else
        this->SetUserAction(new SteppingAction());

    /////////////////////////////////////////////////////////

    // Set by default a random seed (system time) for the random
    // number generator
    SetRandomSeed(-1);





}


QPIX_App::~QPIX_App()
{
    delete msg_;
    delete rtd;
}



void QPIX_App::RegisterMacro(G4String macro)
{
    // Store the name of the macro file
    macros_.push_back(macro);
}

void QPIX_App::ExecuteMacroFile(const char* filename)
{
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4UIsession* batchSession = new BatchSession(filename, UI->GetSession());
    UI->SetSession(batchSession);
    G4UIsession* previousSession = UI->GetSession()->SessionStart();
    delete UI->GetSession();
    UI->SetSession(previousSession);
}




void QPIX_App::RegisterDelayedMacro(G4String macro)
{
    // Store the name of the macro file
    delayed_.push_back(macro);
}



void QPIX_App::Initialize()
{
    // Execute all command macro files before initializing the app
    // so that all objects get configured
    // G4UImanager* UI = G4UImanager::GetUIpointer();
    //
    rtd=RTDCodeManager::Instance();

    for (unsigned int i=0; i<macros_.size(); i++) {
        ExecuteMacroFile(macros_[i].data());
    }
    G4RunManager::Initialize();


    for (unsigned int j=0; j<delayed_.size(); j++) {
        ExecuteMacroFile(delayed_[j].data());
    }
}




void QPIX_App::SetRandomSeed(G4int seed)
{
    // Set the seed chosen by the user for the pseudo-random number
    // generator unless a negative number was provided, in which case
    // we will set as seed the system time.
    if (seed < 0) CLHEP::HepRandom::setTheSeed(time(0));
    else CLHEP::HepRandom::setTheSeed(seed);
}
