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
//
// $Id: G4PhysicsLogVector.cc,v 1.19 2008-09-16 18:57:08 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// --------------------------------------------------------------
//      GEANT 4 class implementation file
//
//  G4PhysicsLogVector.cc
//
//  History:
//    02 Dec. 1995, G.Cosmo : Structure created based on object model
//    15 Feb. 1996, K.Amako : Implemented the 1st version
//    01 Jul. 1996, K.Amako : Hidden bin from the user introduced
//    26 Sep. 1996, K.Amako : Constructor with only 'bin size' added
//    11 Nov. 2000, H.Kurashige : use STL vector for dataVector and binVector
//    9  Mar. 2001, H.Kurashige : add PhysicsVector type and Retrieve
//    05 Sep. 2008, V.Ivanchenko : added protections for zero-length vector
//
// --------------------------------------------------------------

#include "G4PhysicsLogVector.hh"

G4PhysicsLogVector::G4PhysicsLogVector()
  : G4PhysicsVector(), dBin(0.), baseBin(0.)
{ 
  type = T_G4PhysicsLogVector;
}

G4PhysicsLogVector::G4PhysicsLogVector(size_t theNbin)
  : G4PhysicsVector(), dBin(0.), baseBin(0.)
{
  type = T_G4PhysicsLogVector;

  // Add extra one bin (hidden to user) to handle correctly when 
  // Energy=theEmax in getValue. 
  dataVector.reserve(theNbin+1);
  binVector.reserve(theNbin+1); 

  numberOfBin = theNbin;

  for (size_t i=0; i<=numberOfBin; i++) {
    binVector.push_back(0.0);
    dataVector.push_back(0.0);
  }
}  

G4PhysicsLogVector::G4PhysicsLogVector(G4double theEmin, 
                                       G4double theEmax, size_t theNbin)
  : G4PhysicsVector(), dBin(std::log10(theEmax/theEmin)/theNbin),
    baseBin(std::log10(theEmin)/dBin)
{
  type = T_G4PhysicsLogVector;

  // Add extra one bin (hidden to user) to handle correctly when 
  // Energy=theEmax in getValue. 
  dataVector.reserve(theNbin+1);
  binVector.reserve(theNbin+1); 

  numberOfBin = theNbin;

  for (size_t i=0; i<=numberOfBin; i++) {
    binVector.push_back(std::pow(10., std::log10(theEmin)+i*dBin));
    dataVector.push_back(0.0);
  }
  edgeMin = binVector[0];
  edgeMax = binVector[numberOfBin-1];
}  

G4PhysicsLogVector::~G4PhysicsLogVector(){}

G4bool G4PhysicsLogVector::Retrieve(std::ifstream& fIn, G4bool ascii)
{
  G4bool success = G4PhysicsVector::Retrieve(fIn, ascii);
  if (success)
  {
    G4double theEmin = binVector[0];
    dBin = std::log10(binVector[1]/theEmin);
    baseBin = std::log10(theEmin)/dBin;
  }
  return success;
}


G4PhysicsLogVector::G4PhysicsLogVector(const G4PhysicsLogVector& right)
  :G4PhysicsVector(right)
{
  dBin = right.dBin;
  baseBin = right.baseBin;
}

G4PhysicsLogVector& 
G4PhysicsLogVector::operator=(const G4PhysicsLogVector& right)
{
  //  if(this != right) {
    dBin    = right.dBin;
    baseBin = right.baseBin;
    // }
  return *this;
}

