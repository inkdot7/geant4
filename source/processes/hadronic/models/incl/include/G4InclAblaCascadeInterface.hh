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
// $Id: G4InclAblaCascadeInterface.hh,v 1.3 2007-09-11 13:18:42 miheikki Exp $ 
// Translation of INCL4.2/ABLA V3 
// Pekka Kaitaniemi, HIP (translation)
// Christelle Schmidt, IPNL (fission code)
// Alain Boudard, CEA (contact person INCL/ABLA)
// Aatos Heikkinen, HIP (project coordination)


// CLASS DESCRIPTION
// This class is a preliminary interface code for INCL4 cascade and
// ABLA evaporation codes. This class is intended to be used as an 
// interface for colliding light ions (deuterons, tritons, he3 and 
// alphas) to nuclei.

// This class was created by Pekka Kaitaniemi
// (kaitanie@cc.helsinki.fi) , Helsinki Institute of Physics using
// G4CascadeInterface as a template.


#ifndef G4INCLABLACASCADEINTERFACE_H
#define G4INCLABLACASCADEINTERFACE_H 1

#include "G4Nucleon.hh"
#include "G4Nucleus.hh"
#include "G4HadronicInteraction.hh"
#include "G4VIntraNuclearTransportModel.hh"
#include "G4KineticTrackVector.hh"
#include "G4FragmentVector.hh"
#include "G4ParticleChange.hh"
#include "G4ReactionProductVector.hh"
#include "G4ReactionProduct.hh"

// INCL includes
#include "G4InclDataDefs.hh"
#include "G4AblaDataDefs.hh"
#include "G4Incl.hh"

#include <fstream>
#include <iostream>

using namespace std;

/**
 * Interface for INCL/ABLA. This interface handles basic hadron
 * bullet particles (protons, neutrons, pions).
 * @see G4InclAblaLightIonInterface
 */

class G4InclAblaCascadeInterface : public G4VIntraNuclearTransportModel {

public:
  /**
   * Basic constructor.
   */
  G4InclAblaCascadeInterface();

  
  G4int operator==(G4InclAblaCascadeInterface& right) {
    return (this == &right);
  }

  G4int operator!=(G4InclAblaCascadeInterface& right) {
    return (this != &right);
  }

  /**
   * Destructor
   */
  ~G4InclAblaCascadeInterface();

  /**
   * Propagate
   */
  G4ReactionProductVector* Propagate(G4KineticTrackVector* theSecondaries, G4V3DNucleus* theNucleus);

  /**
   * Applies the INCL/ABLA physics model.
   * @param aTrack the projectile particle
   * @param theNucleus target nucleus
   * @return the output of the INCL/ABLA physics model
   */
  G4HadFinalState* ApplyYourself(const G4HadProjectile& aTrack, 
                                   G4Nucleus& theNucleus); 

private:
  G4int outputVerbosity;
  G4int verboseLevel;
  G4int enableAbla; //1 = enabled, 0 = disabled
  G4bool useAbla;

private:
  G4HadFinalState theResult;  
  ofstream diagdata;

  G4int eventNumber;
  G4double previousTargetA;
  G4double previousTargetZ;
  G4bool theAblaModelEnabled;
};

#endif // G4INCLABLACASCADEINTERFACE_H
