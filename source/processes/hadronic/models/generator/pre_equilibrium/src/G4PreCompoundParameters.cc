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
// * authors in the GEANT4 collaboration.                             *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4PreCompoundParameters.cc,v 1.7 2002/06/06 17:19:24 larazb Exp $
// GEANT4 tag $Name: geant4-04-01 $
//
// by V. Lara

#include "G4PreCompoundParameters.hh"


G4PreCompoundParameters G4PreCompoundParameters::thePreCompoundParameters;

G4PreCompoundParameters * G4PreCompoundParameters::GetAddress()
{ return &thePreCompoundParameters; }

