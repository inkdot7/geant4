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
// $Id: G4DNAHeliumChargeIncrease.hh,v 1.1 2005-12-20 13:42:31 capra Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#ifndef   G4DNAHELIUMCHARGEINCREASE_HH
 #define  G4DNAHELIUMCHARGEINCREASE_HH 1

 #include "G4DNAChargeIncreaseInWater.hh"
 #include "G4DNADingfelderChargeChangeTotalCrossSectionPolicy.hh"
 #include "G4DNAHeliumChargeIncreaseFinalStatesPolicy.hh"
 #include "G4DNAStopAndKillBelowEnergyLimitPolicy.hh"

 class G4DNAHeliumChargeIncreaseEnergyLimitsPolicy
 {
  protected:
                      G4DNAHeliumChargeIncreaseEnergyLimitsPolicy();

   const G4double     lowEnergyLimit;
   const G4bool       zeroBelowLowEnergyLimit;
   const G4double     highEnergyLimit;
   const G4bool       zeroAboveHighEnergyLimit;
 };

 class G4DNAHeliumChargeIncreaseIncomingParticlePolicy
 {
  protected:
                                        G4DNAHeliumChargeIncreaseIncomingParticlePolicy();
   const G4ParticleDefinition *         IncomingParticleDefinition(void) const;

   G4int                                NumberOfPartialCrossSections(void) const;

   G4double                             f0[2];
   G4double                             a0[2];
   G4double                             a1[2];
   G4double                             b0[2];
   G4double                             b1[2];
   G4double                             c0[2];
   G4double                             d0[2];
   G4double                             x0[2];
   G4double                             x1[2];
 };

 class G4DNAHeliumChargeIncrease : public G4DNAChargeIncreaseInWater<G4DNADingfelderChargeChangeTotalCrossSectionPolicy<G4DNAHeliumChargeIncreaseIncomingParticlePolicy, G4DNAHeliumChargeIncreaseEnergyLimitsPolicy>, G4DNAHeliumChargeIncreaseFinalStatesPolicy<G4DNAHeliumChargeIncreaseEnergyLimitsPolicy> >
 {
  public:
                                         G4DNAHeliumChargeIncrease(const G4String & name = "G4DNAHeliumChargeIncrease") : G4DNAChargeIncreaseInWater<G4DNADingfelderChargeChangeTotalCrossSectionPolicy<G4DNAHeliumChargeIncreaseIncomingParticlePolicy, G4DNAHeliumChargeIncreaseEnergyLimitsPolicy>, G4DNAHeliumChargeIncreaseFinalStatesPolicy<G4DNAHeliumChargeIncreaseEnergyLimitsPolicy> > (name) {}
   virtual                              ~G4DNAHeliumChargeIncrease() {}
 };
#endif /* G4DNAHELIUMCHARGEINCREASE_HH */
