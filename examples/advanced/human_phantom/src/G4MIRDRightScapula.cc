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
// Authors: S. Guatelli and M. G. Pia, INFN Genova, Italy
// 
// Based on code developed by the undergraduate student G. Guerrieri 
// Note: this is a preliminary beta-version of the code; an improved 
// version will be distributed in the next Geant4 public release, compliant
// with the design in a forthcoming publication, and subject to a 
// design and code review.
//
#include "G4MIRDRightScapula.hh"

#include "globals.hh"

#include "G4SDManager.hh"
#include "G4Cons.hh"

#include "G4VisAttributes.hh"
#include "G4HumanPhantomMaterial.hh"
#include "G4EllipticalTube.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4HumanPhantomColour.hh"
#include "G4Box.hh"

G4MIRDRightScapula::G4MIRDRightScapula()
{
}

G4MIRDRightScapula::~G4MIRDRightScapula()
{
}

G4VPhysicalVolume* G4MIRDRightScapula::Construct(const G4String& volumeName, G4VPhysicalVolume* mother, 
						 const G4String& colourName, G4bool wireFrame,G4bool sensitivity)
{
 
  G4cout << "Construct"<< volumeName << G4endl;

  G4HumanPhantomMaterial* material = new G4HumanPhantomMaterial();
  G4Material* skeleton = material -> GetMaterial("skeleton");
 
  G4double ax_in = 17.* cm;
  G4double by_in = 9.8* cm;
  G4double ax_out = 19.*cm;
  G4double by_out = 9.8*cm;
  G4double dz= 16.4* cm;


  G4EllipticalTube* inner_scapula = new G4EllipticalTube("ScapulaIn", ax_in, by_in, (dz+ 1.*cm)/2);
  G4EllipticalTube* outer_scapula = new G4EllipticalTube("ScapulaOut", ax_out, by_out, dz/2);
  
 
  

  G4Box* subtraction = new G4Box("subtraction",ax_out, ax_out, ax_out);

  G4double xx = -ax_out * 0.242 ; //(sin 14deg)
  G4double yy  = - ax_out * 0.97; // (cos 14 deg)
						   
  G4RotationMatrix* rm = new G4RotationMatrix();
  rm -> rotateZ(14.* degree);

 

  G4SubtractionSolid* scapula_first =  new G4SubtractionSolid("Scapula_first",
  						      outer_scapula,
  						      subtraction,
  						      rm, 
  					              G4ThreeVector(xx, yy, 0. *cm));

  G4double xx2 = ax_out * 0.62470 ; //(cos 51.34deg)
  G4double yy2 = ax_out * 0.78087; // (sin 51.34 deg)
						   
  G4RotationMatrix* rm2 = new G4RotationMatrix();
  rm2 -> rotateZ(38.6598* degree);


 G4SubtractionSolid* scapula_bone =  new G4SubtractionSolid("Scapula",
  						      scapula_first,
  						subtraction,
  						rm2, 
  					       G4ThreeVector(xx2, yy2, 0. *cm));

 G4SubtractionSolid* scapula =  new G4SubtractionSolid("Scapula",
						      scapula_bone,
						      inner_scapula);

 G4LogicalVolume* logicRightScapula = new G4LogicalVolume(scapula,
						   skeleton,
						   "logical" + volumeName,
						    0, 0, 0);

  G4VPhysicalVolume* physRightScapula = new G4PVPlacement(0,
                                
				G4ThreeVector(0. * cm, 0. * cm, 24.1 *cm),
      			       "physicalRightScapula",
  			       logicRightScapula,
			       mother,
			       false,
			       0, true);

   if (sensitivity == true)
  { 
     G4SDManager* SDman = G4SDManager::GetSDMpointer();
     logicRightScapula->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
   }

  // Visualization Attributes
  //G4VisAttributes* RightScapulaVisAtt = new G4VisAttributes(G4Colour(0.94,0.5,0.5));
  G4HumanPhantomColour* colourPointer = new G4HumanPhantomColour();
  G4Colour colour = colourPointer -> GetColour(colourName);
  G4VisAttributes* RightScapulaVisAtt = new G4VisAttributes(colour);
  RightScapulaVisAtt->SetForceSolid(wireFrame);
  logicRightScapula->SetVisAttributes(RightScapulaVisAtt);
  G4cout << "RightScapula created !!!!!!" << G4endl;

  // Testing RightScapula Volume
  G4double RightScapulaVol = logicRightScapula->GetSolid()->GetCubicVolume();
  G4cout << "Volume of RightScapula = " << RightScapulaVol/cm3 << " cm^3" << G4endl;
  
  // Testing RightScapula Material
  G4String RightScapulaMat = logicRightScapula->GetMaterial()->GetName();
  G4cout << "Material of RightScapula = " << RightScapulaMat << G4endl;
  
  // Testing Density
  G4double RightScapulaDensity = logicRightScapula->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << RightScapulaDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double RightScapulaMass = (RightScapulaVol)*RightScapulaDensity;
  G4cout << "Mass of RightScapula = " << RightScapulaMass/gram << " g" << G4endl;

  
  return physRightScapula;
}
