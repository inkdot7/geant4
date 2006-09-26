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
#include "G4MIRDTrunk.hh"

#include "globals.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4HumanPhantomMaterial.hh"
#include "G4EllipticalTube.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

G4MIRDTrunk::G4MIRDTrunk()
{
}

G4MIRDTrunk::~G4MIRDTrunk()
{

}

G4VPhysicalVolume* G4MIRDTrunk::ConstructTrunk(G4VPhysicalVolume* mother, G4String sex, G4bool sensitivity)
{

  G4HumanPhantomMaterial* material = new G4HumanPhantomMaterial();
   
  G4cout << "ConstructTrunck for "<< sex <<G4endl;
   
  G4Material* soft = material -> GetMaterial("soft_tissue");
 
  delete material;

  G4double dx = 17.25 * cm;
  G4double dy = 9.80 * cm;
  G4double dz = 31.55 * cm;

  G4EllipticalTube* trunk = new G4EllipticalTube("Trunk",dx, dy, dz);

  G4LogicalVolume* logicTrunk = new G4LogicalVolume(trunk, soft, 
						    "TrunkVolume",
						    0, 0, 0);
  G4RotationMatrix* rm = new G4RotationMatrix();
  rm -> rotateX(90.* degree);

  // Define rotation and position here!
  G4VPhysicalVolume* physTrunk = new G4PVPlacement(rm,
				 G4ThreeVector(0.* cm,31.55*cm, 0.*cm),
      			       "physicalTrunk",
  			       logicTrunk,
			       mother,
			       false,
			       0);

  // Sensitive Body Part
  if (sensitivity == true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicTrunk->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  G4VisAttributes* TrunkVisAtt = new G4VisAttributes(G4Colour(0.94,0.5,0.5));
  TrunkVisAtt->SetForceSolid(false);
  logicTrunk->SetVisAttributes(TrunkVisAtt);

  G4cout << "Trunk created !!!!!!" << G4endl;

  // Testing Trunk Volume
  G4double TrunkVol = logicTrunk->GetSolid()->GetCubicVolume();
  G4cout << "Volume of Trunk = " << TrunkVol/cm3 << " cm^3" << G4endl;
  
  // Testing Trunk Material
  G4String TrunkMat = logicTrunk->GetMaterial()->GetName();
  G4cout << "Material of Trunk = " << TrunkMat << G4endl;
  
  // Testing Density
  G4double TrunkDensity = logicTrunk->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << TrunkDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double TrunkMass = (TrunkVol)*TrunkDensity;
  G4cout << "Mass of Trunk = " << TrunkMass/gram << " g" << G4endl;

  
  return physTrunk;
}
