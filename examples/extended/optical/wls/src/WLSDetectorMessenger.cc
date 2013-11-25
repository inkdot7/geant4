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
// $Id$
//
/// \file optical/wls/src/WLSDetectorMessenger.cc
/// \brief Implementation of the WLSDetectorMessenger class
//
//
#include "WLSDetectorMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSDetectorMessenger::WLSDetectorMessenger(WLSDetectorConstruction * det)
  : fDetector(det)
{
  fDetDir = new G4UIdirectory("/WLS/");
  fDetDir->SetGuidance(" Geometry Setup ");

  SetPhotonDetGeometryCmd =
                      new G4UIcmdWithAString("/WLS/setPhotonDetGeometry",this);
  SetPhotonDetGeometryCmd->
                  SetGuidance("Select the geometry of the PhotonDet detector");
  SetPhotonDetGeometryCmd->SetGuidance("Only Accepts 'Circle' and 'Square'");
  SetPhotonDetGeometryCmd->SetCandidates("Circle Square");
  SetPhotonDetGeometryCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetPhotonDetGeometryCmd->SetToBeBroadcasted(false);
 
  SetNumOfCladLayersCmd = new G4UIcmdWithAnInteger("/WLS/setNumOfLayers", this);
  SetNumOfCladLayersCmd->SetGuidance("Select the number of cladding layers");
  SetNumOfCladLayersCmd->SetGuidance("Maximum number is 2");
  SetNumOfCladLayersCmd->SetParameterName("numberOfLayers",false);
  SetNumOfCladLayersCmd->SetRange("numberOfLayers>=0 && numberOfLayers<=2");
  SetNumOfCladLayersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetNumOfCladLayersCmd->SetToBeBroadcasted(false);

  SetSurfaceRoughnessCmd =
                      new G4UIcmdWithADouble("/WLS/setSurfaceRoughness", this);
  SetSurfaceRoughnessCmd->
                  SetGuidance("Set the roughness between Clad1 and WLS Fiber");
  SetSurfaceRoughnessCmd->SetParameterName("roughness",false);
  SetSurfaceRoughnessCmd->SetRange("roughness>0 && roughness<=1");
  SetSurfaceRoughnessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetSurfaceRoughnessCmd->SetToBeBroadcasted(false);

  SetXYRatioCmd = new G4UIcmdWithADouble("/WLS/setXYRatio", this);
  SetXYRatioCmd->SetGuidance("Set the ratio between x and y axis (x/y)");
  SetXYRatioCmd->SetParameterName("ratio",false);
  SetXYRatioCmd->SetRange("ratio>0 && ratio<=1");
  SetXYRatioCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetXYRatioCmd->SetToBeBroadcasted(false);

  SetMirrorPolishCmd = new G4UIcmdWithADouble("/WLS/setMirrorPolish", this);
  SetMirrorPolishCmd->SetGuidance("Set the polish of the mirror");
  SetMirrorPolishCmd->SetParameterName("polish",false);
  SetMirrorPolishCmd->SetRange("polish>0 && polish<=1");
  SetMirrorPolishCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetMirrorPolishCmd->SetToBeBroadcasted(false);

  SetMirrorReflectivityCmd =
                    new G4UIcmdWithADouble("/WLS/setMirrorReflectivity", this);
  SetMirrorReflectivityCmd->SetGuidance("Set the reflectivity of the mirror");
  SetMirrorReflectivityCmd->SetParameterName("reflectivity",false);
  SetMirrorReflectivityCmd->SetRange("reflectivity>=0 && reflectivity<=1");
  SetMirrorReflectivityCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetMirrorReflectivityCmd->SetToBeBroadcasted(false);

  SetPhotonDetPolishCmd =
                       new G4UIcmdWithADouble("/WLS/setPhotonDetPolish", this);
  SetPhotonDetPolishCmd->SetGuidance("Set the polish of the mirror");
  SetPhotonDetPolishCmd->SetParameterName("polish",false);
  SetPhotonDetPolishCmd->SetRange("polish>0 && polish<=1");
  SetPhotonDetPolishCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetPhotonDetPolishCmd->SetToBeBroadcasted(false);

  SetPhotonDetReflectivityCmd =
                 new G4UIcmdWithADouble("/WLS/setPhotonDetReflectivity", this);
  SetPhotonDetReflectivityCmd->
                             SetGuidance("Set the reflectivity of the mirror");
  SetPhotonDetReflectivityCmd->SetParameterName("reflectivity",false);
  SetPhotonDetReflectivityCmd->SetRange("reflectivity>=0 && reflectivity<=1");
  SetPhotonDetReflectivityCmd->AvailableForStates(G4State_PreInit);
  SetPhotonDetReflectivityCmd->SetToBeBroadcasted(false);

  SetWLSLengthCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setWLSLength",this);
  SetWLSLengthCmd->SetGuidance("Set the half length of the WLS fiber");
  SetWLSLengthCmd->SetParameterName("length",false);
  SetWLSLengthCmd->SetRange("length>0.");
  SetWLSLengthCmd->SetUnitCategory("Length");
  SetWLSLengthCmd->SetDefaultUnit("mm");
  SetWLSLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetWLSLengthCmd->SetToBeBroadcasted(false);

  SetWLSRadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setWLSRadius",this);
  SetWLSRadiusCmd->SetGuidance("Set the radius of the WLS fiber");
  SetWLSRadiusCmd->SetParameterName("radius",false);
  SetWLSRadiusCmd->SetRange("radius>0.");
  SetWLSRadiusCmd->SetUnitCategory("Length");
  SetWLSRadiusCmd->SetDefaultUnit("mm");
  SetWLSRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetWLSRadiusCmd->SetToBeBroadcasted(false);

  SetClad1RadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setClad1Radius",this);
  SetClad1RadiusCmd->SetGuidance("Set the radius of Cladding 1");
  SetClad1RadiusCmd->SetParameterName("radius",false);
  SetClad1RadiusCmd->SetRange("radius>0.");
  SetClad1RadiusCmd->SetUnitCategory("Length");
  SetClad1RadiusCmd->SetDefaultUnit("mm");
  SetClad1RadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetClad1RadiusCmd->SetToBeBroadcasted(false);

  SetClad2RadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setClad2Radius",this);
  SetClad2RadiusCmd->SetGuidance("Set the radius of Cladding 2");
  SetClad2RadiusCmd->SetParameterName("radius",false);
  SetClad2RadiusCmd->SetRange("radius>0.");
  SetClad2RadiusCmd->SetUnitCategory("Length");
  SetClad2RadiusCmd->SetDefaultUnit("mm");
  SetClad2RadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetClad2RadiusCmd->SetToBeBroadcasted(false);

  SetPhotonDetHalfLengthCmd =
             new G4UIcmdWithADoubleAndUnit("/WLS/setPhotonDetHalfLength",this);
  SetPhotonDetHalfLengthCmd->
                      SetGuidance("Set the half length of PhotonDet detector");
  SetPhotonDetHalfLengthCmd->SetParameterName("halfL",false);
  SetPhotonDetHalfLengthCmd->SetRange("halfL>0.");
  SetPhotonDetHalfLengthCmd->SetUnitCategory("Length");
  SetPhotonDetHalfLengthCmd->SetDefaultUnit("mm");
  SetPhotonDetHalfLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetPhotonDetHalfLengthCmd->SetToBeBroadcasted(false);

  SetGapCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setGap",this);
  SetGapCmd->SetGuidance("Set the distance between PhotonDet and fiber end");
  SetGapCmd->SetParameterName("theta",false);
  SetGapCmd->SetUnitCategory("Length");
  SetGapCmd->SetDefaultUnit("mm");
  SetGapCmd->SetRange("theta>=0.");
  SetGapCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetGapCmd->SetToBeBroadcasted(false);

  SetPhotonDetAlignmentCmd =
                       new G4UIcmdWithADoubleAndUnit("/WLS/setAlignment",this);
  SetPhotonDetAlignmentCmd->
                     SetGuidance("Set the deviation of PhotonDet from z axis");
  SetPhotonDetAlignmentCmd->SetParameterName("theta",false);
  SetPhotonDetAlignmentCmd->SetUnitCategory("Angle");
  SetPhotonDetAlignmentCmd->SetDefaultUnit("deg");
  SetPhotonDetAlignmentCmd->SetRange("theta>-90. && theta<90.");
  SetPhotonDetAlignmentCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetPhotonDetAlignmentCmd->SetToBeBroadcasted(false);

  SetMirrorCmd = new G4UIcmdWithABool("/WLS/setMirror", this);
  SetMirrorCmd->SetGuidance("Place a mirror at the end of the fiber");
  SetMirrorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetMirrorCmd->SetToBeBroadcasted(false);

  SetBarLengthCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setBarLength",this);
  SetBarLengthCmd->SetGuidance("Set the length of the scintillator bar");
  SetBarLengthCmd->SetParameterName("length",false);
  SetBarLengthCmd->SetRange("length>0.");
  SetBarLengthCmd->SetUnitCategory("Length");
  SetBarLengthCmd->SetDefaultUnit("mm");
  SetBarLengthCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetBarLengthCmd->SetToBeBroadcasted(false);

  SetBarBaseCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setBarBase",this);
  SetBarBaseCmd->SetGuidance("Set the side length of the scintillator bar");
  SetBarBaseCmd->SetParameterName("length",false);
  SetBarBaseCmd->SetRange("length>0.");
  SetBarBaseCmd->SetUnitCategory("Length");
  SetBarBaseCmd->SetDefaultUnit("mm");
  SetBarBaseCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetBarBaseCmd->SetToBeBroadcasted(false);

  SetHoleRadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setHoleRadius",this);
  SetHoleRadiusCmd->SetGuidance("Set the radius of the fiber hole");
  SetHoleRadiusCmd->SetParameterName("radius",false);
  SetHoleRadiusCmd->SetRange("radius>0.");
  SetHoleRadiusCmd->SetUnitCategory("Length");
  SetHoleRadiusCmd->SetDefaultUnit("mm");
  SetHoleRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetHoleRadiusCmd->SetToBeBroadcasted(false);

  SetCoatingThicknessCmd =
               new G4UIcmdWithADoubleAndUnit("/WLS/setCoatingThickness",this);
  SetCoatingThicknessCmd->
                   SetGuidance("Set thickness of the coating on the bars");
  SetCoatingThicknessCmd->SetParameterName("thick",false);
  SetCoatingThicknessCmd->SetUnitCategory("Length");
  SetCoatingThicknessCmd->SetDefaultUnit("mm");
  SetCoatingThicknessCmd->SetRange("thick>=0.");
  SetCoatingThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetCoatingThicknessCmd->SetToBeBroadcasted(false);

  SetCoatingRadiusCmd =
                new G4UIcmdWithADoubleAndUnit("/WLS/setCoatingRadius",this);
  SetCoatingRadiusCmd->
                    SetGuidance("Set inner radius of the corner bar coating");
  SetCoatingRadiusCmd->SetParameterName("cradius",false);
  SetCoatingRadiusCmd->SetUnitCategory("Length");
  SetCoatingRadiusCmd->SetDefaultUnit("mm");
  SetCoatingRadiusCmd->SetRange("cradius>=0.");
  SetCoatingRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  SetCoatingRadiusCmd->SetToBeBroadcasted(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSDetectorMessenger::~WLSDetectorMessenger()
{
  delete fDetDir;

  delete SetPhotonDetGeometryCmd;
  delete SetNumOfCladLayersCmd;
  delete SetWLSLengthCmd;
  delete SetWLSRadiusCmd;
  delete SetClad1RadiusCmd;
  delete SetClad2RadiusCmd;
  delete SetPhotonDetHalfLengthCmd;
  delete SetGapCmd;
  delete SetPhotonDetAlignmentCmd;
  delete SetSurfaceRoughnessCmd;
  delete SetMirrorPolishCmd;
  delete SetMirrorReflectivityCmd;
  delete SetXYRatioCmd;
  delete SetMirrorCmd;
  delete SetBarLengthCmd;
  delete SetBarBaseCmd;
  delete SetHoleRadiusCmd;
  delete SetCoatingThicknessCmd;
  delete SetCoatingRadiusCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorMessenger::SetNewValue(G4UIcommand* command,G4String val)
{
  if( command == SetPhotonDetGeometryCmd ) {
 
    fDetector->SetPhotonDetGeometry(val);
  }
  else if( command == SetNumOfCladLayersCmd ) {

    fDetector->SetNumberOfCladding(G4UIcmdWithAnInteger::GetNewIntValue(val));
  }
  else if( command == SetSurfaceRoughnessCmd ) {

    fDetector->SetSurfaceRoughness(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if( command == SetXYRatioCmd ) {
 
    fDetector->SetXYRatio(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if( command == SetMirrorPolishCmd ) {

    fDetector->SetMirrorPolish(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if( command == SetMirrorReflectivityCmd ) {
 
    fDetector->
             SetMirrorReflectivity(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if( command == SetPhotonDetPolishCmd ) {
 
    fDetector->SetPhotonDetPolish(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if( command == SetPhotonDetReflectivityCmd ) {
 
    fDetector->
          SetPhotonDetReflectivity(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if( command == SetWLSLengthCmd ) {
 
    fDetector->SetWLSLength(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetWLSRadiusCmd ) {
 
    fDetector->SetWLSRadius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetClad1RadiusCmd ) {
 
    fDetector->
             SetClad1Radius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetClad2RadiusCmd ) {
 
    fDetector->
             SetClad2Radius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetPhotonDetHalfLengthCmd ) {
 
    fDetector->
     SetPhotonDetHalfLength(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetGapCmd ) {
 
   fDetector->SetGap(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetPhotonDetAlignmentCmd ) {
 
   fDetector->
      SetPhotonDetAlignment(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if( command == SetMirrorCmd ) {

   fDetector->SetMirror(G4UIcmdWithABool::GetNewBoolValue(val));
  }
  else if( command == SetBarLengthCmd ) {

   fDetector->SetBarLength(G4UIcmdWithABool::GetNewBoolValue(val));
  }
  else if( command == SetBarBaseCmd ) {

   fDetector->SetBarBase(G4UIcmdWithABool::GetNewBoolValue(val));
  }
  else if( command == SetHoleRadiusCmd ) {

   fDetector->SetHoleRadius(G4UIcmdWithABool::GetNewBoolValue(val));
  }
  else if( command == SetCoatingThicknessCmd ) {

   fDetector->SetCoatingThickness(G4UIcmdWithABool::GetNewBoolValue(val));
  }
  else if( command == SetCoatingRadiusCmd ) {

   fDetector->SetCoatingRadius(G4UIcmdWithABool::GetNewBoolValue(val));
  }
}
