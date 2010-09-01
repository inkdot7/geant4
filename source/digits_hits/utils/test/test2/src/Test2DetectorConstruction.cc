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
// $Id: Test2DetectorConstruction.cc,v 1.2 2010-09-01 08:03:10 akimura Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

#include "Test2DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4PVDivision.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ios.hh"

#include "Test2PhantomSD.hh"

Test2DetectorConstruction::Test2DetectorConstruction()
:fbConstructed(false)
{;}

Test2DetectorConstruction::~Test2DetectorConstruction()
{;}

G4VPhysicalVolume* Test2DetectorConstruction::Construct()
{
  if(!fbConstructed)
  { 
    fbConstructed = true;
    DefineMaterials();
    SetupGeometry();
    SetupDetectors();
  }
  return fWorldPhys;
}

void Test2DetectorConstruction::DefineMaterials()
{ 
  G4String name, symbol;             //a=mass of a mole;
  G4double a, z, density;            //z=mean number of protons;  

  G4int ncomponents, natoms;
  G4double fractionmass;

  //
  // define Elements
  //

  a = 1.01*g/mole;
  G4Element* H  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);

  a = 14.01*g/mole;
  G4Element* N  = new G4Element(name="Nitrogen",symbol="N" , z= 7., a);

  a = 16.00*g/mole;
  G4Element* O  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);

  //
  // define a material from elements.   case 1: chemical molecule
  //
 
  density = 1.000*g/cm3;
  fWaterMat = new G4Material(name="Water", density, ncomponents=2);
  fWaterMat->AddElement(H, natoms=2);
  fWaterMat->AddElement(O, natoms=1);

  //
  // define a material from elements.   case 2: mixture by fractional mass
  //

  density = 1.290*mg/cm3;
  fAirMat = new G4Material(name="Air"  , density, ncomponents=2);
  fAirMat->AddElement(N, fractionmass=0.7);
  fAirMat->AddElement(O, fractionmass=0.3);
}

void Test2DetectorConstruction::SetupGeometry()
{
  //     
  // World
  //
  G4VSolid* worldSolid = new G4Box("World",2.*m,2.*m,2.*m);
  G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid,fAirMat,"World");
  fWorldPhys = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"World",
                        0,false,0);
  
  //                               
  // 3D nested phantom
  //
  // parameters
  G4double phantomSize[3] = {1.*m, 1.*m, 1.*m};
  G4int nSegment[3] = {10, 10, 10};

  //  phantom box
  G4VSolid* phantomSolid = new G4Box("PhantomBox", phantomSize[0], phantomSize[1], phantomSize[2]);
  G4LogicalVolume* phantomLogical = new G4LogicalVolume(phantomSolid, fWaterMat, "Phantom");
  new G4PVPlacement(0,G4ThreeVector(), phantomLogical, "Phantom",
                         worldLogical, false, 0);

  G4String layerName[3] = {"layerX", "layerY", "layerZ"};
  G4VSolid * layerSolid[3];


  // replication along X
  layerSolid[0] = new G4Box(layerName[0],
			    phantomSize[0]/nSegment[0],
			    phantomSize[1],
			    phantomSize[2]);
  fLayerLogical[0] = new G4LogicalVolume(layerSolid[0], fWaterMat, layerName[0]);
  new G4PVReplica(layerName[0], fLayerLogical[0], phantomLogical, kXAxis,
		  nSegment[0], phantomSize[0]/nSegment[0]*2.);

  // replication along Y
  layerSolid[1] = new G4Box(layerName[1],
			    phantomSize[0]/nSegment[0],
			    phantomSize[1]/nSegment[1],
			    phantomSize[2]);
  fLayerLogical[1] = new G4LogicalVolume(layerSolid[1], fWaterMat, layerName[1]);
  new G4PVReplica(layerName[1], fLayerLogical[1], fLayerLogical[0], kYAxis,
		  nSegment[1], phantomSize[1]/nSegment[1]*2.);

  // replication along Z
  layerSolid[2] = new G4Box(layerName[2],
			    phantomSize[0]/nSegment[0],
			    phantomSize[1]/nSegment[1],
			    phantomSize[2]/nSegment[2]);
  fLayerLogical[2] = new G4LogicalVolume(layerSolid[2], fWaterMat, layerName[2]);
  //fPhantomPhys = new G4PVReplica(layerName[2], fLayerLogical[2], fLayerLogical[1], kZAxis,
  //		  nSegment[2], phantomSize[2]/nSegment[2]*2.);
  fPhantomPhys = new G4PVDivision(layerName[2], fLayerLogical[2], fLayerLogical[1], kZAxis,
		  nSegment[2], 0.);

  //                                        
  // Visualization attributes
  //
  //  worldLogical->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  phantomLogical->SetVisAttributes(simpleBoxVisAtt);

  G4VisAttributes * invisatt = new G4VisAttributes(G4Colour(.0,.0,.0));
  invisatt->SetVisibility(false);
  fLayerLogical[0]->SetVisAttributes(invisatt);
  fLayerLogical[1]->SetVisAttributes(invisatt);
  G4VisAttributes * visatt = new G4VisAttributes(G4Colour(.8,.8,.8));
  visatt->SetVisibility(true);
  fLayerLogical[2]->SetVisAttributes(visatt);

}


#include "G4SDManager.hh"

void Test2DetectorConstruction::SetupDetectors() {
  //
  // sensitive detectors
  //
  G4SDManager * sdManager = G4SDManager::GetSDMpointer();
  sdManager->SetVerboseLevel(1);

  G4String phantomSDName = "MassWorld/Phantom";
  Test2PhantomSD * phantomSD = new Test2PhantomSD(phantomSDName);
  sdManager->AddNewDetector(phantomSD);
  fLayerLogical[2]->SetSensitiveDetector(phantomSD);

  sdManager->SetVerboseLevel(0);

}

