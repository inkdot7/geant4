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
// $Id: Tst50RunAction.cc,v 1.19 2003-05-17 14:18:49 pia Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
#include "G4ios.hh"
#include <math.h>
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "Tst50RunAction.hh"
#include "Tst50RunMessenger.hh"
#include "Tst50PrimaryGeneratorAction.hh"
#include "Tst50DetectorConstruction.hh"
#include "Tst50AnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
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
 
  if (G4VVisManager::GetConcreteInstance())
    {
      G4UImanager* UI = G4UImanager::GetUIpointer();
      UI->ApplyCommand("/vis/scene/notifyHandlers");
    } 

  gamma_trans=0;
  numberEvents=0;
  particle_trans=0;
  particle_back=0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4int Tst50RunAction::GetRun_ID()
{
  return runID;
}
G4bool Tst50RunAction::Get_flag()
{
  //transmission test or CSDA Range and Stopping Power test for massive 
  //particles 
  return flag;
}


void Tst50RunAction::EndOfRunAction(const G4Run* aRun)
{
  G4RunManager* runManager = G4RunManager::GetRunManager();
  p_Primary =
    (Tst50PrimaryGeneratorAction*)(runManager->GetUserPrimaryGeneratorAction());
  G4double energy = p_Primary->GetInitialEnergy();
  G4String particle_name= p_Primary->GetParticle();
  p_Detector =
    (Tst50DetectorConstruction*)(runManager->GetUserDetectorConstruction());
  G4String name = p_Detector->GetMaterialName(); 
  G4double density = p_Detector->GetDensity();
  G4double thickness = p_Detector->GetTargetThickness();

  if (G4VVisManager::GetConcreteInstance())
    {
      G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
    }

  numberEvents=aRun->GetNumberOfEvent();
  
  if (particle_name =="gamma")
    {
      G4double trans=(gamma_trans/numberEvents);
      G4double trans_error=-(log(sqrt(gamma_trans)/numberEvents)/(thickness*density)); 
      G4double trans_coeff= -(log(trans))/(thickness*density);
      Tst50AnalysisManager* analysis = Tst50AnalysisManager::getInstance();
      analysis -> attenuation_coeffiecient(runID,energy/MeV,trans_coeff/(cm2/g),trans_error/(cm2/g));

    }
 
  if (flag)
    {
      if (particle_name =="e-" || particle_name =="e+")
	{
	  G4double ft=(particle_trans/numberEvents) ;
	  G4double ft_error= (sqrt(particle_trans))/numberEvents;
	  G4double fb=(particle_back/numberEvents);
	  G4double fb_error= (sqrt(particle_back))/numberEvents;
	  Tst50AnalysisManager* analysis = Tst50AnalysisManager::getInstance();
	  analysis-> trasmission(runID,energy/MeV,ft,fb,ft_error,fb_error);
	}
    }
}

void  Tst50RunAction::Set_Trans(G4String newValue)
{
  if (newValue=="on"){flag=true;} else {flag=false;};
} 


void  Tst50RunAction::gamma_transmitted()
{
  gamma_trans= gamma_trans +1;
}

void  Tst50RunAction::Trans_number()
{
  particle_trans= particle_trans+1;
}
void  Tst50RunAction::Back_number()
{
  particle_back= particle_back+1;
}
