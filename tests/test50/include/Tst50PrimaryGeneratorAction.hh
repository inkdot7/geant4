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
// $Id: Tst50PrimaryGeneratorAction.hh,v 1.3 2002-12-18 17:04:42 guatelli Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
#ifndef Tst50PrimaryGeneratorAction_h
#define Tst50PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ios.hh"
#include "globals.hh"

class Tst50PrimaryGeneratorMessenger;
class G4ParticleGun;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
class Tst50PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    Tst50PrimaryGeneratorAction();    
   ~Tst50PrimaryGeneratorAction();
    
  G4double GetInitialEnergy();

  public:
    void GeneratePrimaries(G4Event*);
    
  private:
    G4ParticleGun* particleGun;
    Tst50PrimaryGeneratorMessenger* gunMessenger; 
    G4double energy;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
inline G4double Tst50PrimaryGeneratorAction::GetInitialEnergy()
{
  return energy;

}
#endif










