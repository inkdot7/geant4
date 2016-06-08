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
// $Id: G4ThreeVector.hh,v 1.3 2001/07/11 10:00:51 gunter Exp $
// GEANT4 tag $Name: geant4-04-01 $
//
// 
// ----------------------------------------------------------------------
//
// G4ThreeVector class, typedef to CLHEP Hep3Vector
//
// ----------------------------------------------------------------------

#ifndef G4THREEVECTOR_HH
#define G4THREEVECTOR_HH

#include "globals.hh"
#include <CLHEP/Vector/ThreeVector.h>

typedef Hep3Vector G4ThreeVector;

#endif
