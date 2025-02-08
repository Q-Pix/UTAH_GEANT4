#define getCurrentForLTSpice_cxx
#include "getCurrentForLTSpice.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TRandom.h"
#include <iostream>
#include <fstream>
#include <random>
#include <map>
using namespace  std;
TGraph *t;
void getCurrentForLTSpice::Loop()
{
//   In a ROOT session, you can do:
//      root> .L getCurrentForLTSpice.C
//      root> getCurrentForLTSpice t
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
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   const int size=100000;
   const float SampleTime=1e-8 ;//seconds
    int  dim =100e-6/SampleTime;
    int ShiftedIndex=0;
    double TotalCharge=0;
   vector<double> CumCur1(size);
    TRandom * RandomUniform=new TRandom();
    int Indexcum=-1;
   Long64_t nbytes = 0, nb = 0;
    vector<double> time;
    TCanvas *c1=new TCanvas("c1","c1",1000,800);
    int ProcessedEvents=0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      /*
      int dim=InstCurrent->size();
      vector<double> current;
      for (int i=0;i<dim;i++){
          time.push_back(i*10e-9);
          current.push_back(InstCurrent->at(i));
      }

      //gROOT->GetObject("c1",c1);
       return;

      */
        //random_device rd;  // Will be used to obtain a seed for the random number engine
        //mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        //uniform_real_distribution<> dis(0, size);
       dim= InstCurrent->size();
       Indexcum=round(size*drand48());
        //cout<<Indexcum<<endl;
        for (int i=0;i<dim;i++){
            ShiftedIndex=Indexcum+i;
            if ((ShiftedIndex)>=size) break;
            CumCur1[ShiftedIndex]+=(InstCurrent->at(i)/1e9);
        }
        //cout<<jentry << " " << Indexcum<<endl;
        ProcessedEvents++;
      // if (Cut(ientry) < 0) continue;
   }
    for(int i=0;i<CumCur1.size();i++)
        TotalCharge+=CumCur1.at(i);

    if(TotalCharge<1e-12) {
        Loop();
        return;
    }
    for (int i=0;i<CumCur1.size();i++)
        time.push_back(i*SampleTime);


    t=new TGraph(size,&time[0],&CumCur1[0]);
    t->SetTitle("Liquid Ar source is at 5cm away;Time(s);Current(A)");
    t->Draw("AL*");
    c1->Print("TotalCurrentLiq5cm.jpg");

    cout<<"Started writing into the file..\n";
    ofstream myfile;
    myfile.open ("current.txt");

    for (int i=0;i<CumCur1.size();i++) myfile <<time.at(i) << " " << CumCur1.at(i) <<"\n";
    cout<<"Finished writing into file..\n";
    cout<<"Proccessed Events " <<ProcessedEvents<<endl;
    myfile.close();


}

void getCurrentForLTSpice::PlotSingleEntries(int event) {
    Long64_t ientry = LoadTree(event);
    if (ientry < 0) return;
    fChain->GetEntry(ientry);
    vector<double>time;
    vector<double>Current;
    TGraph *t2;
    double sTime=10e-9;
    for (int i =0;i<InstCurrent->size();i++){
        time.push_back(i*sTime);
        Current.push_back(InstCurrent->at(i));
    }
    t2=new TGraph(InstCurrent->size(),&time[0],&Current[0]);
    t2->SetTitle("Liquid Ar source is at 5cm away;Time(s);Current(A) for 1ms");
    t2->Draw("AL*");
}

void getCurrentForLTSpice::PlotAllEntries() {
    if (fChain == 0) return;
    Long64_t nentries = fChain->GetEntriesFast();
    const int size=100000;
    const float SampleTime=1e-8 ;//seconds
    int  dim =100e-6/SampleTime;
    double TotalCharge=0;
    vector<double> CumCur1(size);
    vector<vector<double>> AllCurrents;
    TRandom * RandomUniform=new TRandom();
    int Indexcum=-1;
    Long64_t nbytes = 0, nb = 0;
    vector<double> time;
    TCanvas *c1=new TCanvas("c1","c1",1000,800);
    int ProcessedEvents=0;
    cout<<"Started Collecting all the values"<<endl;

    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        AllCurrents.push_back(*InstCurrent);
    }
    TMultiGraph *mg =new TMultiGraph();
    vector<TGraph*> Graphs;
    cout<<"Preparing the Graphs.."<<endl;
    for(int i=0;i<dim;i++)
        time.push_back(i*SampleTime);
    for (auto &x:AllCurrents)
        Graphs.push_back(new TGraph(dim,&time[0],&x[0]));
    for(auto &g:Graphs)
        mg->Add(g,"PL");
    cout<<"Plotting "<< AllCurrents.size()<< " many events..."<<endl;
    mg->Draw("ALP");




}
