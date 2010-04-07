#ifndef G4UNBOUNDPN_HH
#define G4UNBOUNDPN_HH
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
// $Id: G4UnboundPN.hh,v 1.2 2010-04-07 17:28:35 mkelsey Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ------------------------------------------------------------
//      Bertini Cascade unboundPN class header file
//
//      History: first implementation, inspired by G4Proton
//      17 Nov 2009:  Michael Kelsey
//	06 Apr 2010:  Reset theInstance in dtor, implement ctor in .cc.
// ----------------------------------------------------------------

#include "G4Ions.hh"

// ######################################################################
// ###                        UNBOUNDPN                                ###
// ######################################################################

class G4UnboundPN : public G4Ions {
private:
  static G4UnboundPN* theInstance;
  G4UnboundPN();
  ~G4UnboundPN() { theInstance = 0; }
  
public:
  static G4UnboundPN* Definition();
  static G4UnboundPN* UnboundPNDefinition();
  static G4UnboundPN* UnboundPN();
};

#endif	/* G4UNBOUNDPN_HH */
