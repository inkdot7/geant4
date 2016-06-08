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
// $Id: Em2SteppingAction.hh,v 1.4 2001/10/25 15:12:06 maire Exp $
// GEANT4 tag $Name: geant4-04-01 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef Em2SteppingAction_h
#define Em2SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class Em2DetectorConstruction;
class Em2RunAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Em2SteppingAction : public G4UserSteppingAction
{
  public:
    Em2SteppingAction(Em2DetectorConstruction*,Em2RunAction*);
   ~Em2SteppingAction();

    void UserSteppingAction(const G4Step*);
  
  private:
  Em2DetectorConstruction* Em2Det;
  Em2RunAction*            Em2Run;  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
