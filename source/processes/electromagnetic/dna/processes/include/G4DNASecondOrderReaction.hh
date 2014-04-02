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
// $Id$
//
// Author: Mathieu Karamitros (kara (AT) cenbg . in2p3 . fr)
//
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//
// History:
// -----------
// 16 March 2012 M.Karamitros created
//
// -------------------------------------------------------------------

#ifndef G4DNASECONDORDERREACTION_HH
#define G4DNASECONDORDERREACTION_HH

#include "G4VITProcess.hh"

class G4MolecularConfiguration;

class G4DNASecondOrderReaction : public G4VITProcess
{
public:
    G4DNASecondOrderReaction(const G4String& aName =  "DNASecondOrderReaction",
                             G4ProcessType type = fDecay);
    virtual ~G4DNASecondOrderReaction();

    G4IT_ADD_CLONE(G4VITProcess,G4DNASecondOrderReaction)

    G4DNASecondOrderReaction(const G4DNASecondOrderReaction&);
    G4DNASecondOrderReaction& operator=(const G4DNASecondOrderReaction&);
    void StartTracking(G4Track*);

    void SetReaction(const G4MolecularConfiguration*, const G4Material*, double /*reactionRate*/);

public :
   virtual void BuildPhysicsTable(const G4ParticleDefinition&);
   virtual G4double PostStepGetPhysicalInteractionLength(
                           const G4Track& track,
               G4double   previousStepSize,
               G4ForceCondition* condition
              );

   virtual G4VParticleChange* PostStepDoIt(
               const G4Track& ,
               const G4Step&
              );

   virtual G4double AtRestGetPhysicalInteractionLength(
                           const G4Track& ,
               G4ForceCondition*
              ){ return -1.0; }

   virtual G4VParticleChange* AtRestDoIt(
               const G4Track& ,
               const G4Step&
              ){return 0;}

   //  no operation in  AlongStepDoIt
   virtual G4double AlongStepGetPhysicalInteractionLength(
                           const G4Track&,
               G4double  ,
               G4double  ,
               G4double& ,
                           G4GPILSelection*
                          ){ return -1.0; }

   //  no operation in  AlongStepDoIt
   virtual G4VParticleChange* AlongStepDoIt(
               const G4Track& ,
               const G4Step&
                          ) {return 0;}

protected:
    struct SecondOrderReactionState : public G4ProcessState
    {
        SecondOrderReactionState();
        virtual ~SecondOrderReactionState(){;}
        G4double fPreviousTimeAtPreStepPoint;
        G4bool fIsInGoodMaterial;
    };

private :
    void Create();

protected:
    G4bool fIsInitialized;

    G4double fReturnedValue;

    const std::vector<double>* fpMoleculeDensity;
    G4double fReactionRate;
    G4double fConcentration;
    G4double fMolarMassOfMaterial;
    SecondOrderReactionState*& fpSecondOrderReactionState;
    G4ParticleChange fParticleChange;

    const G4MolecularConfiguration* fpMolecularConfiguration;
    const G4Material* fpMaterial;
};

#endif // G4DNASECONDORDERREACTION_HH
