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
//
// ------------------------------------------------------------
//      GEANT 4 class header file --- Copyright CERN 1998
//      CERN Geneva Switzerland
//
//      History: first implementation, based on object model of
//      2nd December 1995, G.Cosmo
//      ------------ G4AntiProtonAnnihilationAtRest physics process ------
//                   by Larry Felawka (TRIUMF), April 1998
//                     E-mail: felawka@alph04.triumf.ca
// ************************************************************
//-----------------------------------------------------------------------------

#ifndef G4AntiProtonAnnihilationAtRest_h
#define G4AntiProtonAnnihilationAtRest_h 1
 
// Class Description
// Process for annihilation of p-bar at rest; 
// to be used in your physics list in case you need this physics.
// Class Description - End

#include "globals.hh"
#include "Randomize.hh" 
#include "G4VRestProcess.hh"
#include "G4VParticleChange.hh"
#include "G4ParticleDefinition.hh"
#include "G4GHEKinematicsVector.hh"

class G4AntiProtonAnnihilationAtRest : public G4VRestProcess
 
{ 
  private:
  // hide assignment operator as private 
      G4AntiProtonAnnihilationAtRest& operator=(const G4AntiProtonAnnihilationAtRest &right);
      G4AntiProtonAnnihilationAtRest(const G4AntiProtonAnnihilationAtRest& );
   
  public:
 
     G4AntiProtonAnnihilationAtRest(const G4String& processName ="AntiProtonAnnihilationAtRest");
 
    ~G4AntiProtonAnnihilationAtRest();

     G4bool IsApplicable(const G4ParticleDefinition&);

  // null physics table
     void BuildPhysicsTable(const G4ParticleDefinition&){}

     G4double AtRestGetPhysicalInteractionLength(const G4Track&,
						 G4ForceCondition*);

  // zero mean lifetime
     G4double GetMeanLifeTime(const G4Track& ,
			      G4ForceCondition* ) {return 0.0;}

     G4VParticleChange* AtRestDoIt(const G4Track&, const G4Step&); 

  // return number of secondaries produced
     G4int GetNumberOfSecondaries();

  // pointer to array containg kinematics of secondaries
     G4GHEKinematicsVector* GetSecondaryKinematics();

  private:

     void GenerateSecondaries();
     void Poisso( G4float, G4int* );
     void Normal( G4float* );
     void AntiProtonAnnihilation( G4int* );
     G4double ExNu( G4float );
     G4int NFac( G4int );

  private:

// global time-of-flight of stopped AntiProton
     G4float  globalTime;

// atomic mass of target nucleus
     G4float  targetAtomicMass;

// charge of target nucleus
     G4float  targetCharge;

     G4GHEKinematicsVector* pv;
     G4GHEKinematicsVector* eve;
     G4GHEKinematicsVector* gkin;

     G4float  evapEnergy1;
     G4float  evapEnergy3;

     G4int    ngkine;

     G4int    ntot;
     G4GHEKinematicsVector result;

     G4float  massPionMinus;
     G4float  massProton;
     G4float  massPionZero;
     G4float  massAntiProton;
     G4float  massPionPlus;
     G4float  massGamma;

     G4ParticleDefinition* pdefGamma;
     G4ParticleDefinition* pdefPionPlus;
     G4ParticleDefinition* pdefPionZero;
     G4ParticleDefinition* pdefPionMinus;
     G4ParticleDefinition* pdefProton;
     G4ParticleDefinition* pdefAntiProton;
     G4ParticleDefinition* pdefNeutron;
     G4ParticleDefinition* pdefDeuteron;
     G4ParticleDefinition* pdefTriton;
     G4ParticleDefinition* pdefAlpha;

};

#endif
 
