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
// $Id: G4PlateIrrGasXTRdEdx.hh,v 1.2 2001/07/11 10:01:29 gunter Exp $
// GEANT4 tag $Name: geant4-04-01 $
//
// 
///////////////////////////////////////////////////////////////////////////
// 
// Model describing a radiator of X-ray transition radiation.  
// Thicknesses of plates is fixed while gas gaps are fully irregular.
// We suppose that:
// formation zone ~ mean thickness << absorption length
// for each material and in the range 1-100 keV. This allows us to simplify
// interference effects in radiator stack (GetStackFactor method).
// 
// 
// History:
// 27.02.01 V. Grichine, first version 
//


#ifndef G4PlateIrrGasXTRdEdx_h
#define G4PlateIrrGasXTRdEdx_h 1

#include "G4VFastSimulationModel.hh"
#include "G4VXTRdEdx.hh"

class G4PlateIrrGasXTRdEdx : public G4VXTRdEdx
{
public:

   G4PlateIrrGasXTRdEdx (G4LogicalVolume *anEnvelope,G4double,G4double);
  ~G4PlateIrrGasXTRdEdx ();

  // Pure virtual function from base class

  G4double GetStackFactor( G4double energy, G4double gamma, G4double varAngle);
};

#endif
