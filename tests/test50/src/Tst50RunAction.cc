//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: Tst50RunAction.cc,v 1.13 2003-04-28 14:58:57 guatelli Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "Tst50RunAction.hh"
#include <math.h>
#include "G4Run.hh"
#include "Tst50RunMessenger.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "Tst50PrimaryGeneratorAction.hh"
#include "Tst50DetectorConstruction.hh"
#ifdef G4ANALYSIS_USE
#include "Tst50AnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
Tst50RunAction::Tst50RunAction()
{
 p_messenger= new Tst50RunMessenger(this);
 flag =false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Tst50RunAction::~Tst50RunAction()
{
 delete p_messenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Tst50RunAction::BeginOfRunAction(const G4Run* aRun)
{
  // G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl; 

  runID=aRun->GetRunID();
  G4cout<< flag<<"--------->flag"<<G4endl;

#ifdef G4ANALYSIS_USE   
Tst50AnalysisManager* analysis = Tst50AnalysisManager::getInstance();
   analysis->book();
#endif

  if (G4VVisManager::GetConcreteInstance())
    {
      G4UImanager* UI = G4UImanager::GetUIpointer();
      UI->ApplyCommand("/vis/scene/notifyHandlers");
    } 
  fg=0;
  numberEvents=0;
  number=0;
  numberB=0;
  numberTransp=0;
  numberRay=0;
  numberPh=0; 
  numberCo=0;
  numberPair=0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int Tst50RunAction::GetRun_ID()
{
  return runID;
}
G4bool Tst50RunAction::Get_flag()
{
  return flag;
}


void Tst50RunAction::EndOfRunAction(const G4Run* aRun)
{
  G4RunManager* runManager = G4RunManager::GetRunManager();
  p_Primary =
(Tst50PrimaryGeneratorAction*)(runManager->GetUserPrimaryGeneratorAction());
G4double energy= p_Primary->GetInitialEnergy();
 G4String particle_name= p_Primary->GetParticle();
 

 p_Detector =
(Tst50DetectorConstruction*)(runManager->GetUserDetectorConstruction());

G4String name=p_Detector->GetMaterialName(); 
G4double density =p_Detector->GetDensity();
G4double thickness =p_Detector->GetTargetThickness();

#ifdef G4ANALYSIS_USE
 Tst50AnalysisManager* analysis = Tst50AnalysisManager::getInstance();

  analysis->finish();
#endif

  if (G4VVisManager::GetConcreteInstance())
    {
     G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
    }
  numberEvents=aRun->GetNumberOfEvent();
  
  if (particle_name =="gamma")
    {
 G4double trans=(fg/numberEvents); 
 
 G4double tran_coeff= -(log(trans))/(thickness*density);
 G4double error= -(log(sqrt(trans)))/(thickness*density);
G4std::ofstream pmtfile("Test50_output.txt", G4std::ios::app);
if(pmtfile.is_open()){
  if (runID==0)
    { pmtfile<<" TEST ----> gamma attenuation coefficient  <-----"<<name<<G4endl;
      pmtfile<<" slab absorber material: "<<name<<G4endl;
      pmtfile<<" slab density: "<<density/(g/cm3)<<G4endl;
      pmtfile<<"BeamOn events: "<<numberEvents<<G4endl;
      pmtfile<< " energy gamma ( MeV)"<<"    "<<"attenuation coeff(cm2/g)"<<     G4endl; 
    }  
 
  pmtfile<<energy/MeV <<"                    "<<tran_coeff/(cm2/g)<< G4endl; }
 
    }
 
  if(flag)
    {
 if (particle_name =="e-" || particle_name =="e+")
    {
      
      G4double ft=(number/numberEvents) ;
      G4double fb=(numberB/numberEvents);
      G4double fa=(((numberEvents-(numberB+number)))/numberEvents);
      G4double trans=(fg/numberEvents); 
G4std::ofstream pmtfile("Test50_output.txt", G4std::ios::app);
if(pmtfile.is_open()){
 
 if (runID==0)
    { 
      pmtfile<<" TEST ----> " <<particle_name << "backscattering  <-----"<<name<<G4endl;
      pmtfile<<" slab absorber material: "<<name<<G4endl;
      pmtfile<<" slab density: "<<density/(g/cm3)<<G4endl;
      pmtfile<<"BeamOn events: "<<numberEvents<<G4endl;
      pmtfile<< " initial energy e- ( MeV)"<<"    "<<" e- back fraction"<<"      "<<
	"e- trans fraction"<<"         "<<" e- absorbed"<< G4endl; 
    }  
 
 
 pmtfile<<"   "<<energy/MeV <<"                   ";		
 pmtfile<<fb <<"                      "<<ft<<"                   "<<fa<<G4endl;
     
}}}}

  

  /* 
 if (numberTransp==0 && numberRay==0 && numberPh==0 && numberCo==0 &&numberPair==0){;}
else
    { 
G4cout<<"---------- Particelle primarie gamma -------------- "<<number<<G4endl;
 G4cout<<"--------- Processi verificatesi----------------"<<G4endl;
  G4cout<<"Number of transmitted gamma: "<<number<<G4endl;
  G4cout<<numberTransp <<" processo di trasporto"<< G4endl;
  G4cout<<numberRay<<" processi Rayleigh"<<G4endl;
  G4cout<<numberPh<< " processi fotoelettrici"<<G4endl;
  G4cout<<numberCo<< " processi Compton"<< G4endl;
  G4cout<<numberPair<< " processi di produzione di coppie"<< G4endl;}
  */
void  Tst50RunAction::Set_Trans(G4String newValue)
{
  if (newValue=="on"){flag=true;} else {flag=false;};


} 


void  Tst50RunAction::gamma_transmitted()
{
  fg = fg +1;
}

void  Tst50RunAction::Trans_number()
{
  number= number+1;
}
void  Tst50RunAction::Back_number()
{
  numberB= numberB+1;
}
void  Tst50RunAction::primary_processes(G4int i)
{
  if( i==1) numberTransp=numberTransp+1;
  if( i==2) numberRay= numberRay+1;
  if( i==3) numberPh= numberPh+1;
  if( i==4) numberCo= numberCo+1;
  if( i==5) numberPair= numberPair+1;
}
