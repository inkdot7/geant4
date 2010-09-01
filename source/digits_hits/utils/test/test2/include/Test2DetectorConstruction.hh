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
// $Id: Test2DetectorConstruction.hh,v 1.2 2010-09-01 08:03:10 akimura Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

#ifndef Test2DetectorConstruction_h
#define Test2DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4Material;
class G4LogicalVolume;

class Test2DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    Test2DetectorConstruction();
    virtual ~Test2DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
     
  private:
    void DefineMaterials();
    void SetupGeometry();
    void SetupDetectors();

  private:
    G4Material* fAirMat;
    G4Material* fWaterMat;
    G4VPhysicalVolume* fWorldPhys;
    G4VPhysicalVolume* fPhantomPhys;
    G4LogicalVolume * fLayerLogical[3];

    G4bool fbConstructed;
};

#endif

