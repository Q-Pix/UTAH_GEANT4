//
// Created by ilker on 1/13/22.
//

#include "RTDCodeManager.h"
#include <G4ThreeVector.hh>
#include <CLHEP/Units/SystemOfUnits.h>
#include "AnalysisManager.h"


RTDCodeManager *RTDCodeManager::instance= nullptr;
RTDCodeManager::RTDCodeManager():msg_(0),
        Wvalue(23.6),
        E_vel ( 164800.0),
        DiffusionL (6.8223),
        DiffusionT(13.1586),
        Life_Time(0.1),
        Reset(6250),
        Sample_time(1e-9),
        Buffer_time(100e-6),
        ElectronCharge_(1.60217662e-19),
        cumulativeCharge_(0),
        SensorPos_(4, 4, 0),
        NumofSensors_(12),
        sensorspacing_(0.1),
        sensorwidth_(0.1)
{
    msg_ = new G4GenericMessenger(this, "/Actions/RTDManager/");
    msg_->DeclareProperty("Wvalue",Wvalue ,  "Change Wvalue");
    msg_->DeclareProperty("E_vel",E_vel ,  "Change E_vel");
    msg_->DeclareProperty("DiffusionL",DiffusionL ,  "Change DiffusionL");
    msg_->DeclareProperty("DiffusionT",DiffusionT ,  "Change DiffusionT");
    msg_->DeclareProperty("Life_Time",Life_Time ,  "Change Life_Time");
    msg_->DeclareProperty("Reset",Reset ,  "Change Reset");
    msg_->DeclareProperty("SampleTime",Sample_time ,  "Change sampling time");
    msg_->DeclareProperty("BufferTime",Buffer_time ,  "Change buffer time");
    msg_->DeclarePropertyWithUnit("SensorPos","cm",SensorPos_ ,  "Position of the Sensors");
    msg_->DeclareProperty("NumOfSensors",NumofSensors_ ,  "How many sensors");
    msg_->DeclarePropertyWithUnit("SensorSpacing","mm",sensorspacing_ ,  "Spacing for the sensors");
    msg_->DeclarePropertyWithUnit("SensorWidth","mm",sensorwidth_,  "Sensor Trace Width");

}
RTDCodeManager * RTDCodeManager::Instance()
{
    if (instance == 0) instance = new RTDCodeManager();
    return instance;
}
RTDCodeManager::~RTDCodeManager() {}



void RTDCodeManager::Diffuser()
{

    int indexer = 0;
    hit_e.clear();
    // loop over all hits in the event
    AnalysisManager * AnaMngr=AnalysisManager::Instance();

    if(AnaMngr->Get_hit_end_x().empty()) {
        return;
        G4Exception("[RTDCodeManager]", "Diffuser()", FatalException,
                    "There are no available hits!");
    }
    std::cout<<"Diffusing Hits for this event ..." <<std::endl;
    std::vector<double> elocx;
    std::vector<double> elocy;
    std::vector<double> elocz;
    std::vector<double> eloct;
    std::map<int,SENSORS*>fSensors;
    fSensors=CreateTheSensors(NumofSensors_,sensorspacing_,sensorwidth_,SensorPos_);
    std::cout<<"There are " << fSensors.size() << " many sensors"<<std::endl;
    for (G4int idx=0;idx<AnaMngr->Get_hit_end_x().size();idx++)
    {
        // from PreStepPoint
        G4double const start_x = AnaMngr->Get_hit_start_x()[idx];      // cm
        G4double const start_y = AnaMngr->Get_hit_start_y()[idx];      // cm
        G4double const start_z = AnaMngr->Get_hit_start_z()[idx];      // cm
        G4double const start_t = (AnaMngr->Get_hit_start_t()[idx]*1e-9); // nsec

        // from PostStepPoint
        G4double const end_x = AnaMngr->Get_hit_end_x()[idx];      // cm
        G4double const end_y = AnaMngr->Get_hit_end_y()[idx];      // cm
        G4double const end_z = AnaMngr->Get_hit_end_z()[idx];      // cm
        G4double const end_t = (AnaMngr->Get_hit_end_t()[idx]*1e-9); // nsec
        // follow the track for truth matching
        if ((start_t < 0.0) || (start_t > Buffer_time)){

            G4Exception("[RTDCodeManager]","Diffuser",JustWarning,"skipping this because of Buffer_Time");
            continue;
        }

        // energy deposit

        G4double const energy_deposit = AnaMngr->Get_Energy()[idx];;  // MeV

        // hit length
        //G4double const length_of_hit = hit.Length();  // cm

        // Set up the paramaters for the recombiataion
        //G4double const dEdx = energy_deposit/length_of_hit;
        //G4double const Recombonation = Modified_Box(dEdx);
        int Nelectron;

        // to account for recombination or not
        // calcualte the number of electrons in the hit
        /* if (Recombination)
         {
             Nelectron = round(Recombonation * (energy_deposit*1e6/Wvalue) );
         }else
         {
             Nelectron = round( (energy_deposit*1e6/Wvalue) );
         }*/
        Nelectron = round( (energy_deposit*1e6/Wvalue) );


        // if not enough move on
        if (Nelectron == 0){continue;}

        // define the electrons start position
        G4double electron_loc_x = start_x;
        G4double electron_loc_y = start_y;
        G4double electron_loc_z = start_z;
        G4double electron_loc_t = start_t;

        // Determin the "step" size (pre to post hit)
        G4double const step_x = (end_x - start_x) / Nelectron;
        G4double const step_y = (end_y - start_y) / Nelectron;
        G4double const step_z = (end_z - start_z) / Nelectron;
        G4double const step_t = (end_t - start_t) / Nelectron;

        G4double electron_x, electron_y, electron_z;
        G4double T_drift, sigma_L, sigma_T;

        // Loop through the electrons
        for (int i = 0; i < Nelectron; i++)
        {
            // calculate drift time for diffusion
            T_drift = electron_loc_z / E_vel;
            // electron lifetime

            if (G4UniformRand() >= exp(-T_drift/Life_Time)){continue;}

            // diffuse the electrons position


            sigma_T = sqrt(2*DiffusionT*T_drift);
            sigma_L = sqrt(2*DiffusionL*T_drift);

            electron_x = G4RandGauss::shoot(electron_loc_x,sigma_T);
            electron_y = G4RandGauss::shoot(electron_loc_y,sigma_T);
            electron_z = G4RandGauss::shoot(electron_loc_z,sigma_L);

            eloct.push_back(electron_loc_t);
            elocx.push_back(electron_x);
            elocy.push_back(electron_y);
            elocz.push_back(electron_z);


            // add the electron to the vector.
            hit_e.push_back(ELECTRON());

            // convert the electrons x,y to a pixel index
            /* int Pix_Xloc, Pix_Yloc;
             Pix_Xloc = (int) ceil(electron_x / Pix_Size);
             Pix_Yloc = (int) ceil(electron_y / Pix_Size);
             */
            int SensorID=GetTheSensorID(electron_x,electron_y,fSensors);


            hit_e[indexer].Pix_ID =SensorID;  //When adding sensors use some pixel identification as above

            //hit_e[indexer].Pix_ID = 1; //When adding sensors use some pixel identification as above
            hit_e[indexer].time = electron_loc_t + ( electron_z / E_vel );

            // Move to the next electron

            electron_loc_x += step_x;
            electron_loc_y += step_y;
            electron_loc_z += step_z;
            electron_loc_t += step_t;
            indexer += 1;

        }
    }
    AnaMngr->AddElectronLocation(elocx,elocy,elocz,eloct);
    for(auto &sr:fSensors) MakeCurrent(sr.first);
    //PrintSensorInfo(fSensors);
    // sorts the electrons in terms of the pixel ID
   // std::sort(hit_e.begin(), hit_e.end(), Electron_Pix_Sort);

}//Get_Event
// sort the elctrons by pixel index
void RTDCodeManager::MakeCurrent(int SensorID) {
    AnalysisManager * AnaMngr=AnalysisManager::Instance();
    std::vector<G4double> TempHiteTime;
    std::cout<<"Creating the current profile for sensor ID " << SensorID<<std::endl;
    for (auto &x:hit_e){
        if(SensorID==x.Pix_ID)
            TempHiteTime.push_back(x.time);
    }
    if(TempHiteTime.empty()) return;
    if(TempHiteTime.empty()) {G4Exception("[RTDCodeManager]","MakeCurrent",JustWarning,"TempHiteTime vector is empty.."); return;}
    std::cout<<"There are "<<TempHiteTime.size()<<" many timeHits "<<std::endl;
    CumulativeCharge.clear();
    InstantaneousCharge.clear();
    int charge = 0;
    int Icharge= 0;
    int charge_dex = 0;
    int charge_size = hit_e.size();
    G4double charge_time = 0;
    G4double current_time = 0;

    bool End_Reached = false;


    std::sort( TempHiteTime.begin(), TempHiteTime.end() );

    // for each pixel loop through the buffer time
    while (current_time <= Buffer_time)
    {
        // setting the "time"
        current_time += Sample_time;

        Icharge = 0;

        // main loop to add electrons to the counter
        if ( current_time > charge_time && charge_dex < charge_size)
        {
            // this adds the electrons that are in the step
            while( current_time > charge_time )
            {
                Icharge += 1;
                charge_dex += 1;
                if (charge_dex >= charge_size){break; }
                charge_time = hit_e[charge_dex].time;
            }
            charge += Icharge;

        }
        if (charge_dex >= charge_size && !End_Reached)
        {
            End_Reached = true;
        }

        // write the instanuoous and cummlitive currents
        //cumulativeCharge_+=(charge*ElectronCharge_/10e-9)*1e9;
        InstantaneousCharge.push_back( (Icharge*ElectronCharge_/Sample_time)*1e9 );
        CumulativeCharge.push_back( charge*ElectronCharge_*1e15);


    }
    AnaMngr->AddInstCurrenttoFile(InstantaneousCharge);
    AnaMngr->AddCumCurrenttoFile(CumulativeCharge);



}// Get_Hot_Current

std::map<int,RTDCodeManager::SENSORS*> RTDCodeManager::CreateTheSensors(int NumOf,double spacing , double width,CLHEP::Hep3Vector Pos){
    std::map<int,SENSORS*> SensorMap;
    float Inc=0;
    for (int i=0;i<NumOf;i++)
    {
        if (SensorMap.find(i)!=SensorMap.end())
            continue;
        SENSORS *sr=new SENSORS();
        sr->Sensor_ID=i;
        sr->Pos=Pos;
        sr->innerRad=spacing+Inc;
        sr->OutterRad=width+Inc+spacing;
        Inc+=spacing+spacing;
        SensorMap.insert(std::pair(i,sr));
    }

    return SensorMap;

}

int RTDCodeManager::GetTheSensorID(double electronX,double electronY,std::map<int,SENSORS*> sensors){
    double distance;
    distance= sqrt((electronX-SensorPos_[0])*(electronX-SensorPos_[0])+(electronY-SensorPos_[1])*(electronY-SensorPos_[1]));
    // std::cout<<distance<<std::endl;
    for(auto &sr:sensors){
        //std::cout<<distance<<std::endl;
        if(distance >= sr.second->innerRad && distance<=sr.second->OutterRad)
            return sr.first;
    }
    return -1;
}
void RTDCodeManager::PrintSensorInfo(std::map<int,SENSORS*> sr){
    std::cout<<"Priting Sensor Information ..."<<std::endl;
    for(auto &x:sr){
        std::cout<<"ID -> "<<x.second->Sensor_ID<<std::endl;
        std::cout<<"OutR -> "<<x.second->OutterRad<<std::endl;
        std::cout<<"InR -> "<<x.second->innerRad<<std::endl;
    }

}

