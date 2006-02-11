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
#include "G4MIRDLegBone.hh"

#include "G4Processor/GDMLProcessor.h"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"

G4MIRDLegBone::G4MIRDLegBone()
{
}

G4MIRDLegBone::~G4MIRDLegBone()
{
  sxp.Finalize();
}

G4VPhysicalVolume* G4MIRDLegBone::ConstructLegBone(G4VPhysicalVolume* mother, G4String sex, G4bool sensitivity)
{
  // Initialize GDML Processor
  sxp.Initialize();
  config.SetURI( "gdmlData/"+sex+"/MIRDLegBone.gdml" );
  config.SetSetupName( "Default" );
  sxp.Configure( &config );

  // Run GDML Processor
  sxp.Run();
 

  G4LogicalVolume* logicLegBone = (G4LogicalVolume *)GDMLProcessor::GetInstance()->GetLogicalVolume("LegBoneVolume");

  G4ThreeVector position = (G4ThreeVector)*GDMLProcessor::GetInstance()->GetPosition("LegBonePos");
  G4RotationMatrix* rm = (G4RotationMatrix*)GDMLProcessor::GetInstance()->GetRotation("LegBoneRot");
  
  // Define rotation and position here!
  G4VPhysicalVolume* physLegBone = new G4PVPlacement(rm,position,
      			       "physicalLegBone",
  			       logicLegBone,
			       mother,
			       false,
			       0);

  // Sensitive Body Part
  if (sensitivity==true)
  { 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    logicLegBone->SetSensitiveDetector( SDman->FindSensitiveDetector("BodyPartSD") );
  }

  // Visualization Attributes
  G4VisAttributes* LegBoneVisAtt = new G4VisAttributes(G4Colour(0.46,0.53,0.6));
  LegBoneVisAtt->SetForceSolid(true);
  logicLegBone->SetVisAttributes(LegBoneVisAtt);

  G4cout << "LegBone created !!!!!!" << G4endl;

  // Testing LegBone Volume
  G4double LegBoneVol = logicLegBone->GetSolid()->GetCubicVolume();
  G4cout << "Volume of LegBone = " << LegBoneVol/cm3 << " cm^3" << G4endl;
  
  // Testing LegBone Material
  G4String LegBoneMat = logicLegBone->GetMaterial()->GetName();
  G4cout << "Material of LegBone = " << LegBoneMat << G4endl;
  
  // Testing Density
  G4double LegBoneDensity = logicLegBone->GetMaterial()->GetDensity();
  G4cout << "Density of Material = " << LegBoneDensity*cm3/g << " g/cm^3" << G4endl;

  // Testing Mass
  G4double LegBoneMass = (LegBoneVol)*LegBoneDensity;
  G4cout << "Mass of LegBone = " << LegBoneMass/gram << " g" << G4endl;

  
  return physLegBone;
}
