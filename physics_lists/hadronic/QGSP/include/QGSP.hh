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
// $Id: QGSP.hh,v 1.2 2005-11-11 22:56:29 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   QGSP
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 10.11.2005 V.Ivanchenko edit to provide a standard 
//
//----------------------------------------------------------------------------
//

#ifndef QGSP_h
#define QGSP_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

//class G4EmPhysicsListMessenger;

class QGSP: public G4VModularPhysicsList
{
public:
  QGSP();
  virtual ~QGSP();

  void SetCuts();

  void SetCutForGamma(G4double);
  void SetCutForElectron(G4double);
  void SetCutForPositron(G4double);

  void SetVerbose(G4int val);

private:

  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  G4int    verbose;

  //  G4EmPhysicsListMessenger* pMessenger;

};

#endif



