//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4eplusAnnihilation.hh,v 1.22 2007-05-22 17:34:36 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4eplusAnnihilation
//
// Author:        Vladimir Ivanchenko on base of Michel Maire code
//
// Creation date: 02.08.2004
//
// Modifications:
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivanchenko)
// 15-03-05 Update interface according to changings 
//           in G4VEmProcess (V.Ivanchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivanchenko)
// 04-05-05 Make class to be default (V.Ivanchenko)
// 04-12-05 SetProposedKineticEnergy(0.) for annihilated positron (mma)
// 09-08-06 add SetModel(G4VEmModel*) (mma)
// 12-09-06, move SetModel(G4VEmModel*) in G4VEmProcess (mma)
//
//
// Class Description:
//
// This class manages the process of e+ annihilation into 2 gammas
//

// -------------------------------------------------------------------
//

#ifndef G4eplusAnnihilation_h
#define G4eplusAnnihilation_h 1

#include "G4VEmProcess.hh"
#include "G4Positron.hh"
#include "G4VEmModel.hh"

class G4eplusAnnihilation : public G4VEmProcess
{

public:

  G4eplusAnnihilation(const G4String& name = "annihil");

  virtual ~G4eplusAnnihilation();

  G4bool IsApplicable(const G4ParticleDefinition& p);

  virtual G4VParticleChange* AtRestDoIt(
                             const G4Track& track,
                             const G4Step& stepData);

  G4double AtRestGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4ForceCondition* condition
                            );

  // Print out of the class parameters
  virtual void PrintInfo();

protected:

  virtual void InitialiseProcess(const G4ParticleDefinition*);

  void SecondariesPostStep(std::vector<G4DynamicParticle*>*,
			   G4VEmModel*,
			   const G4MaterialCutsCouple*,
			   const G4DynamicParticle*);

private:
  
  G4bool  isInitialised;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4bool G4eplusAnnihilation::IsApplicable(const G4ParticleDefinition& p)
{
  return (&p == G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline 
G4double G4eplusAnnihilation::AtRestGetPhysicalInteractionLength(
                              const G4Track&, G4ForceCondition* condition)
{
  *condition = NotForced;
  return 0.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline 
void G4eplusAnnihilation::SecondariesPostStep(std::vector<G4DynamicParticle*>* vdp,
					      G4VEmModel* model,
					      const G4MaterialCutsCouple* couple,
					      const G4DynamicParticle* dp)
{
  fParticleChange.SetProposedKineticEnergy(0.);
  fParticleChange.ProposeTrackStatus(fStopAndKill);
  model->SampleSecondaries(vdp, couple, dp);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
