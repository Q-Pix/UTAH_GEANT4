//
// Created by ilker on 1/9/22.
// Implimented from  nexus
//

#ifndef G4QPIX_QPIX_APP_H
#define G4QPIX_QPIX_APP_H
#include <G4RunManager.hh>
class G4GenericMessenger;
class RTDCodeManager;
class QPIX_App : public G4RunManager {


    public:
        /// Constructor
        QPIX_App(G4String init_macro);
        /// Destructor
        ~QPIX_App();

        virtual void Initialize();

        /// Returns the number of events to be processed in the current run
        G4int GetNumberOfEventsToBeProcessed() const;

    private:
        void RegisterMacro(G4String);

        void RegisterDelayedMacro(G4String);

        void ExecuteMacroFile(const char*);

        /// Set a seed for the G4 random number generator.
        /// If a negative value is chosen, the system time is set as seed.
        void SetRandomSeed(G4int);

    private:
        RTDCodeManager * rtd;
        G4GenericMessenger* msg_;
        G4String gen_name_; ///< Name of the chosen primary generator
        G4String geo_name_;  ///< Name of the chosen geometry
        G4String pm_name_;  ///< Name of the chosen persistency manager
        G4String runact_name_; ///< Name of the chosen run action
        G4String evtact_name_; ///< Name of the chosen event action
        G4String stepact_name_; ///< Name of the chosen stepping action
        G4String trkact_name_; ///< Name of the chosen tracking action
        G4String stkact_name_; ///< Name of the chosen stacking action

        std::vector<G4String> macros_;
        std::vector<G4String> delayed_;

    };

    // INLINE DEFINITIONS ////////////////////////////////////

    inline G4int QPIX_App::GetNumberOfEventsToBeProcessed() const
    { return numberOfEventToBeProcessed; }




#endif //G4QPIX_QPIX_APP_H
