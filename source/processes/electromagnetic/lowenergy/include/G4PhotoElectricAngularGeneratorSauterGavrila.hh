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
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:  G4PhotoElectricAngularGeneratorSauterGavrila
//
// Creation date: 10 May 2004
//
// Modifications: 
// 10 May 2003     P. Rodrigues    First implementation acording with new design
//
// Class Description: 
//
// Concrete class for PhotoElectric Electron Angular Distribution Generation 
// This model is a re-implementation of the Photolectric angular distribution
// developed my M. Maire for the Standard EM Physics G4PhotoElectricEffect 
//
// Further documentation available from http://www.ge.infn.it/geant4/lowE

// -------------------------------------------------------------------
//

#ifndef G4PhotoElectricAngularGeneratorSauterGavrila_h
#define G4PhotoElectricAngularGeneratorSauterGavrila_h 1

#include "G4VPhotoElectricAngularDistribution.hh"
#include "G4ios.hh"
#include "globals.hh"

class G4PhotoElectricAngularGeneratorSauterGavrila : public G4VPhotoElectricAngularDistribution
{

public:

  G4PhotoElectricAngularGeneratorSauterGavrila(const G4String& name);

  ~G4PhotoElectricAngularGeneratorSauterGavrila();

  G4ThreeVector GetPhotoElectronDirection(const G4ThreeVector& direction, 
					  const G4double kineticEnergy, 
					  const G4ThreeVector& polarization, 
					  const G4int shellId) const;

  void PrintGeneratorInformation() const;

protected:

private:

  // hide assignment operator 
     G4PhotoElectricAngularGeneratorSauterGavrila & operator=(const  G4PhotoElectricAngularGeneratorSauterGavrila &right);
     G4PhotoElectricAngularGeneratorSauterGavrila(const  G4PhotoElectricAngularGeneratorSauterGavrila&);

};

#endif

