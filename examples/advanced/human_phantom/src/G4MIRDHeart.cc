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
#include "G4MIRDHeart.hh"

#include "G4Processor/GDMLProcessor.h"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"

G4MIRDHeart::G4MIRDHeart()
{
}

G4MIRDHeart::~G4MIRDHeart()
{
  sxp.Finalize();
}

G4VPhysicalVolume* G4MIRDHeart::ConstructHeart(G4VPhysicalVolume* mother, G4String sex, G4bool sensitivity)
{
  // Initialize GDML Processor
  sxp.Initialize();
  config.SetURI( "gdmlData/"+sex+"/MIRDHeart.gdml" );
  config.SetSetupName( "Default" );
  sxp.Configure( &config );

  // Run GDML Processor
  sxp.Run();
 

  G4LogicalVolume* logicHeart = (G4LogicalVolume *)GDMLProcessor::GetInstance()->GetLogicalVolume("HeartVolume");

  G4ThreeVector position = (G4ThreeVector)*GDMLProcessor::GetInstance()->GetPosition("HeartPos");
  G4RotationMatrix* rm = (G4RotationMatrix*)GDMLProcessor::GetInstance()->GetRotation("HeartRot");
  
  // Define rotation and position here!
  G4VPhysicalVolume* physHeart = new G4PVPlacement(rm,position,
      			       "physicalHeart",
  			       logicHeart,
			       mother,
			       false,
			       0);

  // Sensitive Body Part
  if (sensitivity==true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicHeart->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  G4VisAttributes* HeartVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  HeartVisAtt->SetForceSolid(true);
  logicHeart->SetVisAttributes(HeartVisAtt);

  G4cout << "Heart created !!!!!!" << G4endl;

  // Testing Heart Volume
  G4double HeartVol = logicHeart->GetSolid()->GetCubicVolume();
  G4cout << "Volume of Heart = " << HeartVol/cm3 << " cm^3" << G4endl;
  
  // Testing Heart Material
  G4String HeartMat = logicHeart->GetMaterial()->GetName();
  G4cout << "Material of Heart = " << HeartMat << G4endl;
  
  // Testing Density
  G4double HeartDensity = logicHeart->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << HeartDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double HeartMass = (HeartVol)*HeartDensity;
  G4cout << "Mass of Heart = " << HeartMass/gram << " g" << G4endl;

  
  return physHeart;
}
