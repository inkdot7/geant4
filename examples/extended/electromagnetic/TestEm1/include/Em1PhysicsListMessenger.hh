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
// $Id: Em1PhysicsListMessenger.hh,v 1.5 2002/03/08 13:43:29 maire Exp $
// GEANT4 tag $Name: geant4-04-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef Em1PhysicsListMessenger_h
#define Em1PhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class Em1PhysicsList;
class G4UIcmdWithADoubleAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Em1PhysicsListMessenger: public G4UImessenger
{
  public:
    Em1PhysicsListMessenger(Em1PhysicsList*);
   ~Em1PhysicsListMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    Em1PhysicsList* physList;
    G4UIcmdWithADoubleAndUnit* cutGCmd;
    G4UIcmdWithADoubleAndUnit* cutECmd;
    G4UIcmdWithADoubleAndUnit* cutPCmd;
    G4UIcmdWithADoubleAndUnit* rCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
