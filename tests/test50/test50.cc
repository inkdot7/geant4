
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
// $Id: test50.cc,v 1.21 2003-04-28 14:58:56 guatelli Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 
#include <strstream.h>
//#include <iostream.h>

#include "globals.hh"
#include "G4ios.hh"
#include "g4std/fstream"
#include "g4std/iostream"
#include "g4std/iomanip" 
#include "Tst50DetectorConstruction.hh"
#include "Tst50PhysicsList.hh"
#include "Tst50PrimaryGeneratorAction.hh"
#include "Tst50RunAction.hh"
#include "Tst50EventAction.hh"
#include "Tst50SteppingAction.hh"
#include "Tst50SteppingVerbose.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4VIS_USE
#include "Tst50VisManager.hh"
#endif


int main(int argc,char** argv) {
       
  HepRandom::setTheEngine(new RanecuEngine);
 
 
  G4String filename = "Test50_output.txt";  

  G4cout.setf(G4std::ios::scientific, G4std::ios::floatfield);
  
      G4std::ofstream ofs;
      ofs.open(filename);
      {
	ofs << " test50 output "<< G4endl;
      } 
      ofs.close();                     
		
     
  G4int seed=time(NULL);
  HepRandom ::setTheSeed(seed);
  // my Verbose output class
  G4VSteppingVerbose::SetInstance(new Tst50SteppingVerbose);
  
  // Run manager
  G4RunManager * runManager = new G4RunManager;

  // UserInitialization classes (mandatory)
  Tst50DetectorConstruction* Tst50detector = new Tst50DetectorConstruction();
  runManager->SetUserInitialization(Tst50detector);
  

  Tst50PhysicsList* fisica = new Tst50PhysicsList();
  runManager->SetUserInitialization(fisica);
  


  // Visualization, if you choose to have it!
  G4VisManager* visManager = new Tst50VisManager;
  visManager->Initialize();


  // UserAction classes
  Tst50PrimaryGeneratorAction* p_Primary = new Tst50PrimaryGeneratorAction(); 
  runManager->SetUserAction(p_Primary);

  Tst50RunAction* p_run=new Tst50RunAction(); 
  runManager->SetUserAction(p_run);  

  Tst50EventAction *pEventAction = new Tst50EventAction(p_Primary);
 
  runManager->SetUserAction(pEventAction);
     
  Tst50SteppingAction* steppingaction = new Tst50SteppingAction(pEventAction, p_Primary, p_run,Tst50detector);
  runManager->SetUserAction(steppingaction);

  //Initialize G4 kernel
  //  runManager->Initialize();

  
  //get the pointer to the User Interface manager 
  G4UImanager * UI = G4UImanager::GetUIpointer();  
  UI->ApplyCommand("/run/verbose 0");
  UI->ApplyCommand("/event/verbose 0");
  UI->ApplyCommand("/tracking/verbose 0");
  
  if (argc == 1)
    // Define (G)UI terminal for interactive mode  
    { 
      // G4UIterminal is a (dumb) terminal.
      G4UIsession * session = 0;

#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);      
#else
      session = new G4UIterminal();
#endif    

      UI->ApplyCommand("/control/execute ");
      session->SessionStart();
      delete session;
    }
  else
    // Batch mode
    {     
      G4String command =("/control/execute ");
      G4String fileName = argv[1];
      G4cout << fileName << G4endl;
      UI->ApplyCommand(command+fileName);
    }

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}
