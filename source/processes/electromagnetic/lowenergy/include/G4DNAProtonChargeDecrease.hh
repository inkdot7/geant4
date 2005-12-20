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
// $Id: G4DNAProtonChargeDecrease.hh,v 1.2 2005-12-20 13:53:34 capra Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#ifndef   G4DNAPROTONCHARGEDECREASE_HH
 #define  G4DNAPROTONCHARGEDECREASE_HH 1

 #include "G4DNAChargeDecreaseInWater.hh"
 #include "G4DNADingfelderChargeChangeTotalCrossSectionPolicy.hh"
 #include "G4DNAProtonChargeDecreaseFinalStatesPolicy.hh"
 #include "G4DNAStopAndKillBelowEnergyLimitPolicy.hh"

 class G4DNAProtonChargeDecreaseEnergyLimitsPolicy
 {
  protected:
                      G4DNAProtonChargeDecreaseEnergyLimitsPolicy();

   const G4double     lowEnergyLimit;
   const G4bool       zeroBelowLowEnergyLimit;
   const G4double     highEnergyLimit;
   const G4bool       zeroAboveHighEnergyLimit;
 };

 class G4DNAProtonChargeDecreaseIncomingParticlePolicy
 {
  protected:
                                        G4DNAProtonChargeDecreaseIncomingParticlePolicy();
   const G4ParticleDefinition *         IncomingParticleDefinition(void) const;

   G4int                                NumberOfPartialCrossSections(void) const;

   G4double                             f0[1];
   G4double                             a0[1];
   G4double                             a1[1];
   G4double                             b0[1];
   G4double                             b1[1];
   G4double                             c0[1];
   G4double                             d0[1];
   G4double                             x0[1];
   G4double                             x1[1];
 };

 class G4DNAProtonChargeDecrease : public G4DNAChargeDecreaseInWater<G4DNADingfelderChargeChangeTotalCrossSectionPolicy<G4DNAProtonChargeDecreaseIncomingParticlePolicy, G4DNAProtonChargeDecreaseEnergyLimitsPolicy>, G4DNAProtonChargeDecreaseFinalStatesPolicy<G4DNAProtonChargeDecreaseEnergyLimitsPolicy> >
 {
  public:
                                         G4DNAProtonChargeDecrease(const G4String & name = "G4DNAProtonChargeDecrease") : G4DNAChargeDecreaseInWater<G4DNADingfelderChargeChangeTotalCrossSectionPolicy<G4DNAProtonChargeDecreaseIncomingParticlePolicy, G4DNAProtonChargeDecreaseEnergyLimitsPolicy>, G4DNAProtonChargeDecreaseFinalStatesPolicy<G4DNAProtonChargeDecreaseEnergyLimitsPolicy> > (name) {}
   virtual                              ~G4DNAProtonChargeDecrease() {}
 };
#endif /* G4DNAPROTONCHARGEDECREASE_HH */
