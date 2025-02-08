#define Ana_cxx
#include "Ana.h"
#include <TH2.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace chrono;
void Ana::Loop() {
//   In a ROOT session, you can do:
//      root> .L Ana.C
//      root> Ana t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

    int range=35;
    TCanvas *c1 = new TCanvas("c1", "t", 1900, 800);
    TH1D *hite = new TH1D("hite", "E>0.4", 100,0,1);
    TH1D *hits = new TH1D("hits", "h", 100,0,range);
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    int phCount = 0;
    int eCount = 0;
    //std::vector<double> dists;


    map<double,double> Results;

    vector<double> dr;
    //Create an array with specified range
    dr = arange(0, range, 1);


    double TotalNumOfHitsOutFud = 0;
    double TotalEnergyInFudical = 0;
    double TotalEnergyOutFudical = 0;

    vector<double> newHitx;
    vector<double> newHity;
    vector<double> newHitoutE;
    //TGraph *gr= new TGraph();
    double HitsCount=0;
    double NonQualHitsCount=0;

    double HitsCountless10=0;
    int NumEntrysOut=0;
    int HitandStayed=0;
    int HitOut=0;
    int EverythingElse=0;
    float EnergyLimit=0.4;


    auto start = high_resolution_clock::now();

    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        nbytes += nb;
        if (jentry % 5000 == 0) cout << "Event  " << jentry << " are done !" << endl;


        /*double dist;
        double max_dist = 1000;
        TPolyLine3D *p1 = new TPolyLine3D(hit_end_x->size());

        for (int it=0; it<hit_end_x->size();it++){
            p1->SetPoint(it, hit_end_x->at(it), hit_end_y->at(it), hit_end_z->at(it));
            hite->Fill(hit_end_x->at(it), hit_end_y->at(it), hit_end_z->at(it));

        }

        p1->Draw("same");
        */


        double distance = 0;
        int EventsOutFud = 0;
        int EventsInFud = 0;

        double EnergyInFudical = 0;
        double EnergyOutFudical = 0;
        // New Variables for Qualified Events
        newHitx.clear();
        newHity.clear();
        newHitoutE.clear();
        // Loop Through All the hits per event
        for (int i = 0; i < hit_end_x->size(); i++) {
            distance = Distance(hit_end_x->at(i), hit_end_y->at(i));
            // Events in the Fudical

            if (distance < r) {
                EventsInFud++;
                EnergyInFudical += hit_energy_deposit->at(i);

            }

            //Events Outside the Fudical
            if (distance >= r) {
                EventsOutFud++;
                EnergyOutFudical += hit_energy_deposit->at(i);
                newHitx.push_back(hit_end_x->at(i));
                newHity.push_back(hit_end_y->at(i));
                newHitoutE.push_back(hit_energy_deposit->at(i));
            }
        }


        //if(EventsInFud>0 and EventsOutFud>0) NumEntrysOut++;

        if ((EventsOutFud > 0 and EventsInFud > 0) and EnergyInFudical > EnergyLimit) {
            TotalNumOfHitsOutFud += EventsOutFud;
            TotalEnergyOutFudical += EnergyOutFudical;
            NumEntrysOut++;

            if (EnergyOutFudical>0.01) hite->Fill(EnergyOutFudical);


            for (auto &d:dr) {
                double QualifEvents = 0;
                for (double i = 0; i < newHitx.size(); i++) {
                    distance = Ana::Distance(newHitx.at(i), newHity.at(i));
                    // 10 keV cut
                    if (distance < r + d ) {

                        //QualifEvents+=1; // Count the events
                        QualifEvents += newHitoutE.at(i); // Add Deposited Energy;

                    }


                }


                    //hits->Fill(d, QualifEvents);
                    HitsCountless10++;
                    if(QualifEvents>0.01)
                    {
                        //gr->SetPoint(gr->GetN(),d,QualifEvents);


                        HitsCount++;

                        //if(d<=15) HitsCountless10++;

                        if (Results.find(d) == Results.end()) {
                            //Results.insert(pair<double, double>(d, QualifEvents));
                            Results.insert(pair<double, double>(d, 1));
                        } else {
                            //Results[d] += QualifEvents;
                            Results[d] += 1;
                        }

                    }else {
                        hits->Fill(-1);
                        NonQualHitsCount++;

                    }

                //}



            }

            //if (jentry==3) break;
            // if (Cut(ientry) < 0) continue;

        }else if ((EventsOutFud < 1 and EventsInFud > 0) and EnergyInFudical > EnergyLimit) {HitandStayed++;}

        if ((EventsOutFud > 0 and EventsInFud <1) and EnergyOutFudical>EnergyLimit) {HitOut++;}



    }
        int n = dr.size();

        //cout << "TotalCountHits -> " << TotalNumOfHitsOutFud << endl;
        //cout << "TotalHitEnergyOut -> " << TotalEnergyOutFudical << endl;
        cout << "Number of InteractedEntries -> " << NumEntrysOut << endl;
        cout << "HitAndStayed -> " << HitandStayed << endl;
        cout << "HitOut -> " << HitOut << endl;
        cout << "Everything Else -> " << EverythingElse << endl;


    //cout << "Ratio of NumInteracted/TotalSim -> " << NumEntrysOut/nentries*100 <<" %" <<endl;

        vector<vector<double>> Values(2,vector<double>(n,0));
        int Cntr=0;
        //TGraph *gr= new TGraph(n,&Values.at(0)[0],&Values.at(1)[0]);
        TGraph *gr= new TGraph();
        gr->SetTitle("Hit E>0.4 MeV;dr(cm);Counts");
        TGraph *grnorm= new TGraph();
        grnorm->SetTitle("Normalized;dr(cm);,Counts");



    /////////////////////////////////////////////////////////////////////////////////
        // Get the Max Value Element From a Mapp

        auto item = max_element(Results.begin(), Results.end(),
                              [](const pair<double, double>& p1, const pair<double, double>& p2) {
                                  return p1.second < p2.second; });
        ////////////////////////////////////////////////////////////////////////////////////////

        for (auto &it:Results){
            //Values.at(0).at(Cntr)=it.first;
            //Values.at(1).at(Cntr)=it.second;
            Cntr++;
            grnorm->SetPoint(grnorm->GetN(),it.first,it.second/item->second);
            gr->SetPoint(gr->GetN(),it.first,it.second);
        }

        std::cout<<Results.size()<<endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<seconds>(stop - start);


    c1->Divide(2,1,0.01,0.01);
    c1->cd(1);
    gr->Draw("AP*");
    c1->cd(2);
    grnorm->Draw("AP*");


    // Double_t factor = 1.; hits->Scale(factor/(hits->GetMaximum()));
    //hite->Draw();
    //hite->SaveAs("E>0.7.png");
    //c1->cd(2);
    hits->GetYaxis()->SetLabelOffset(0.01);
    //hits->Draw();




    //c1->SetLogy();



        c1->SaveAs("HitE>0.4_MeV.png");

        cout << "Proccessing Time is " << duration.count() << " (s)" << endl;

    }

