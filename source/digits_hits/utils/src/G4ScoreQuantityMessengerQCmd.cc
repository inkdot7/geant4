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
// $Id: G4ScoreQuantityMessengerQCmd.cc,v 1.3 2007-11-03 22:39:37 asaim Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ---------------------------------------------------------------------

#include "G4ScoreQuantityMessenger.hh"
#include "G4ScoringManager.hh"
#include "G4VScoringMesh.hh"

#include "G4PSCellCharge3D.hh"
#include "G4PSCellFlux3D.hh"
#include "G4PSPassageCellFlux3D.hh"
#include "G4PSEnergyDeposit3D.hh"
#include "G4PSDoseDeposit3D.hh"
#include "G4PSNofStep3D.hh"
#include "G4PSNofSecondary3D.hh"
//
#include "G4PSTrackLength3D.hh"
#include "G4PSPassageCellCurrent3D.hh"
#include "G4PSPassageTrackLength3D.hh"
#include "G4PSFlatSurfaceCurrent3D.hh"
#include "G4PSFlatSurfaceFlux3D.hh"
#include "G4PSSphereSurfaceCurrent3D.hh"
#include "G4PSSphereSurfaceFlux3D.hh"
#include "G4PSCylinderSurfaceCurrent3D.hh"
#include "G4PSCylinderSurfaceFlux3D.hh"
#include "G4PSNofCollision3D.hh"
#include "G4PSPopulation3D.hh"
#include "G4PSTrackCounter3D.hh"
#include "G4PSTermination3D.hh"

#include "G4SDChargedFilter.hh"
#include "G4SDNeutralFilter.hh"
#include "G4SDKineticEnergyFilter.hh"
#include "G4SDParticleFilter.hh"
#include "G4SDParticleWithEnergyFilter.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcommand.hh"
#include "G4Tokenizer.hh"
#include "G4UnitsTable.hh"

void G4ScoreQuantityMessenger::QuantityCommands()
{
  G4UIparameter* param;

  //
  // Quantity commands
  quantityDir = new G4UIdirectory("/score/quantity/");
  quantityDir->SetGuidance("Scoring quantity of the mesh");
  //
  qTouchCmd= new G4UIcmdWithAString("/score/quantity/touch",this);
  qTouchCmd->SetGuidance("Assign previously defined quantity to current quantity.");
  qTouchCmd->SetParameterName("qname",false);
  //
  qeDepCmd = new G4UIcmdWithAString("/score/quantity/energyDeposit",this);
  qeDepCmd->SetGuidance("Energy Deposit Scorer");
  qeDepCmd->SetParameterName("qname",false);
  //
  qCellChgCmd  = new G4UIcmdWithAString("/score/quantity/cellCharge",this);
  qCellChgCmd->SetGuidance("Cell Charge Scorer");
  qCellChgCmd->SetParameterName("qname",false);
  //
  qCellFluxCmd = new G4UIcmdWithAString("/score/quantity/cellFlux",this);
  qCellFluxCmd->SetGuidance("Cell Flux Scorer");
  qCellFluxCmd->SetParameterName("qname",false);
  //
  qPassCellFluxCmd = new G4UIcmdWithAString("/score/quantity/passageCellFlux",this);
  qPassCellFluxCmd->SetGuidance("Passage Cell Flux Scorer");
  qPassCellFluxCmd->SetParameterName("qname",false);
  //
  qdoseDepCmd = new G4UIcmdWithAString("/score/quantity/doseDeposit",this);
  qdoseDepCmd->SetGuidance("Dose Deposit Scorer");
  qdoseDepCmd->SetParameterName("qname",false);
  //
  qnOfStepCmd = new G4UIcmdWithAString("/score/quantity/nOfStep",this);
  qnOfStepCmd->SetGuidance("Number of Step Scorer ");
  qnOfStepCmd->SetParameterName("qname",false);
  //
  qnOfSecondaryCmd = new G4UIcmdWithAString("/score/quantity/nOfSecondary",this);
  qnOfSecondaryCmd->SetGuidance("Number of Secondary Scorer ");
  qnOfSecondaryCmd->SetParameterName("qname",false);
  //
  qTrackLengthCmd = new G4UIcommand("/score/quantity/trackLength",this);
  qTrackLengthCmd->SetGuidance("TrackLength Scorer");
  qTrackLengthCmd->
      SetGuidance("[usage] /score/quantiy/trackLength qname wflag kflag vflag ");
  qTrackLengthCmd->SetGuidance("  qname  :(String) scorer name");
  qTrackLengthCmd->SetGuidance("  wflag  :(Bool) Weighted");
  qTrackLengthCmd->SetGuidance("  kflag  :(Bool) MultiplyKineticEnergy");
  qTrackLengthCmd->SetGuidance("  vflag  :(Bool) DivideByVelocity");
  param = new G4UIparameter("qname",'s',false);
  qTrackLengthCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("false");
  qTrackLengthCmd->SetParameter(param);
  param = new G4UIparameter("kflag",'b',true);
  param->SetDefaultValue("false");
  qTrackLengthCmd->SetParameter(param);
  param = new G4UIparameter("vflag",'b',true);
  param->SetDefaultValue("false");
  qTrackLengthCmd->SetParameter(param);
  //
  qPassCellCurrCmd = new G4UIcommand("/score/quantity/passageCellCurrent",this);
  qPassCellCurrCmd->SetGuidance("PassageCellCurrent Scorer");
  qPassCellCurrCmd->
      SetGuidance("[usage] /score/quantiy/passageCellCurrent qname wflag");
  qPassCellCurrCmd->SetGuidance("  qname  :(String) scorer name");
  qPassCellCurrCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
  qPassCellCurrCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("true");
  qPassCellCurrCmd->SetParameter(param);
  //
  qPassTrackLengthCmd = new G4UIcommand("/score/quantity/passageTrackLength",this);
  qPassTrackLengthCmd->SetGuidance("PassageTrackLength Scorer");
  qPassTrackLengthCmd->
      SetGuidance("[usage] /score/quantiy/passageTrackLength qname wflag");
  qPassTrackLengthCmd->SetGuidance("  qname  :(String) scorer name");
  qPassTrackLengthCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
  qPassTrackLengthCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("true");
  qPassTrackLengthCmd->SetParameter(param);
  //
  qFlatSurfCurrCmd = new G4UIcommand("/score/quantity/flatSurfaceCurrent",this);
  qFlatSurfCurrCmd->SetGuidance("Flat surface current Scorer");
  qFlatSurfCurrCmd->
      SetGuidance("[usage] /score/quantiy/flatSurfaceCurrent qname dflag wflag aflag");
  qFlatSurfCurrCmd->SetGuidance("  qname  :(String) scorer name");
  qFlatSurfCurrCmd->SetGuidance("  dflag  :(Int) direction flag");
  qFlatSurfCurrCmd->SetGuidance("         : 0 = Both In and Out");
  qFlatSurfCurrCmd->SetGuidance("         : 1 = In only");
  qFlatSurfCurrCmd->SetGuidance("         : 2 = Out only");
  qFlatSurfCurrCmd->SetGuidance("  wflag  :(Bool) Weighted");
  qFlatSurfCurrCmd->SetGuidance("  aflag  :(Bool) DivideByArea");
  param = new G4UIparameter("qname",'s',false);
  qFlatSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qFlatSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("true");
  qFlatSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("aflag",'b',true);
  param->SetDefaultValue("true");
  qFlatSurfCurrCmd->SetParameter(param);
  //
  qFlatSurfFluxCmd = new G4UIcommand("/score/quantity/flatSurfaceFlux",this);
  qFlatSurfFluxCmd->SetGuidance("Flat surface Flux Scorer");
  qFlatSurfFluxCmd->
      SetGuidance("[usage] /score/quantiy/flatSurfaceFlux qname dflag");
  qFlatSurfFluxCmd->SetGuidance("  qname  :(String) scorer name");
  qFlatSurfFluxCmd->SetGuidance("  dflag  :(Int) direction flag");
  qFlatSurfFluxCmd->SetGuidance("         : 0 = Both In and Out");
  qFlatSurfFluxCmd->SetGuidance("         : 1 = In only");
  qFlatSurfFluxCmd->SetGuidance("         : 2 = Out only");
  param = new G4UIparameter("qname",'s',false);
  qFlatSurfFluxCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qFlatSurfFluxCmd->SetParameter(param);
  //
//  qSphereSurfCurrCmd = new G4UIcommand("/score/quantity/sphereSurfaceCurrent",this);
//  qSphereSurfCurrCmd->SetGuidance("Sphere surface current Scorer");
//  qSphereSurfCurrCmd->
//      SetGuidance("[usage] /score/quantiy/sphereSurfaceCurrent qname dflag wflag aflag");
//  qSphereSurfCurrCmd->SetGuidance("  qname  :(String) scorer name");
//  qSphereSurfCurrCmd->SetGuidance("  dflag  :(Int) direction flag");
//  qSphereSurfCurrCmd->SetGuidance("         : 0 = Both In and Out");
//  qSphereSurfCurrCmd->SetGuidance("         : 1 = In only");
//  qSphereSurfCurrCmd->SetGuidance("         : 2 = Out only");
//  qSphereSurfCurrCmd->SetGuidance("  wflag  :(Bool) Weighted");
//  qSphereSurfCurrCmd->SetGuidance("  aflag  :(Bool) DivideByArea");
//  param = new G4UIparameter("qname",'s',false);
//  qSphereSurfCurrCmd->SetParameter(param);
//  param = new G4UIparameter("dflag",'i',true);
//  param->SetDefaultValue("0");
//  qSphereSurfCurrCmd->SetParameter(param);
//  param = new G4UIparameter("wflag",'b',true);
//  param->SetDefaultValue("true");
//  qSphereSurfCurrCmd->SetParameter(param);
//  param = new G4UIparameter("aflag",'b',true);
//  param->SetDefaultValue("true");
//  qSphereSurfCurrCmd->SetParameter(param);

  //
//  qSphereSurfFluxCmd = new G4UIcommand("/score/quantity/sphereSurfaceFlux",this);
//  qSphereSurfFluxCmd->SetGuidance("Sphere surface Flux Scorer");
//  qSphereSurfFluxCmd->
//      SetGuidance("[usage] /score/quantiy/sphereSurfaceFlux qname dflag");
//  qSphereSurfFluxCmd->SetGuidance("  qname  :(String) scorer name");
//  qSphereSurfFluxCmd->SetGuidance("  dflag  :(Int) direction flag");
//  qSphereSurfFluxCmd->SetGuidance("         : 0 = Both In and Out");
//  qSphereSurfFluxCmd->SetGuidance("         : 1 = In only");
//  qSphereSurfFluxCmd->SetGuidance("         : 2 = Out only");
//  param = new G4UIparameter("qname",'s',false);
//  qSphereSurfFluxCmd->SetParameter(param);
//  param = new G4UIparameter("dflag",'i',true);
//  param->SetDefaultValue("0");
//  qSphereSurfFluxCmd->SetParameter(param);

  //
//  qCylSurfCurrCmd = new G4UIcommand("/score/quantity/cylinderSurfaceCurrent",this);
//  qCylSurfCurrCmd->SetGuidance("Cylinder surface current Scorer");
//  qCylSurfCurrCmd->
//      SetGuidance("[usage] /score/quantiy/cylinderSurfaceCurrent qname dflag wflag aflag");
//  qCylSurfCurrCmd->SetGuidance("  qname  :(String) scorer name");
//  qCylSurfCurrCmd->SetGuidance("  dflag  :(Int) direction flag");
//  qCylSurfCurrCmd->SetGuidance("         : 0 = Both In and Out");
//  qCylSurfCurrCmd->SetGuidance("         : 1 = In only");
//  qCylSurfCurrCmd->SetGuidance("         : 2 = Out only");
//  qCylSurfCurrCmd->SetGuidance("  wflag  :(Bool) Weighted");
//  qCylSurfCurrCmd->SetGuidance("  aflag  :(Bool) DivideByArea");
//  param = new G4UIparameter("qname",'s',false);
//  qCylSurfCurrCmd->SetParameter(param);
//  param = new G4UIparameter("dflag",'i',true);
//  param->SetDefaultValue("0");
//  qCylSurfCurrCmd->SetParameter(param);
//  param = new G4UIparameter("wflag",'b',true);
//  param->SetDefaultValue("true");
//  qCylSurfCurrCmd->SetParameter(param);
//  param = new G4UIparameter("aflag",'b',true);
//  param->SetDefaultValue("true");
//  qCylSurfCurrCmd->SetParameter(param);
  //
//  qCylSurfFluxCmd = new G4UIcommand("/score/quantity/cylinderSurfaceFlux",this);
//  qCylSurfFluxCmd->SetGuidance("Cylinder surface Flux Scorer");
//  qCylSurfFluxCmd->
//      SetGuidance("[usage] /score/quantiy/cylinderSurfaceFlux qname dflag");
//  qCylSurfFluxCmd->SetGuidance("  qname  :(String) scorer name");
//  qCylSurfFluxCmd->SetGuidance("  dflag  :(Int) direction flag");
//  qCylSurfFluxCmd->SetGuidance("         : 0 = Both In and Out");
//  qCylSurfFluxCmd->SetGuidance("         : 1 = In only");
//  qCylSurfFluxCmd->SetGuidance("         : 2 = Out only");
//  param = new G4UIparameter("qname",'s',false);
//  qCylSurfFluxCmd->SetParameter(param);
//  param = new G4UIparameter("dflag",'i',true);
//  param->SetDefaultValue("0");
//  qCylSurfFluxCmd->SetParameter(param);
  //
  qNofCollisionCmd = new G4UIcommand("/score/quantity/nOfCollision",this);
  qNofCollisionCmd->SetGuidance("Number of Collision Scorer");
  qNofCollisionCmd->
      SetGuidance("[usage] /score/quantiy/nOfCollision qname wflag");
  qNofCollisionCmd->SetGuidance("  qname  :(String) scorer name");
  qNofCollisionCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
  qNofCollisionCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("false");
  qNofCollisionCmd->SetParameter(param);
  //
  qPopulationCmd = new G4UIcommand("/score/quantity/population",this);
  qPopulationCmd->SetGuidance("Population Scorer");
  qPopulationCmd->
      SetGuidance("[usage] /score/quantiy/population qname wflag");
  qPopulationCmd->SetGuidance("  qname  :(String) scorer name");
  qPopulationCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
  qPopulationCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("false");
  qPopulationCmd->SetParameter(param);

  //
  qTrackCountCmd = new G4UIcommand("/score/quantity/trackCounter",this);
  qTrackCountCmd->SetGuidance("Number of Track Counter Scorer");
  qTrackCountCmd->
      SetGuidance("[usage] /score/quantiy/trackCounter qname dflag wflag");
  qTrackCountCmd->SetGuidance("  qname  :(String) scorer name");
  qTrackCountCmd->SetGuidance("  dflag  :(Int) Direction");
  qTrackCountCmd->SetGuidance("         : 0 = Both In and Out");
  qTrackCountCmd->SetGuidance("         : 1 = In only");
  qTrackCountCmd->SetGuidance("         : 2 = Out only");
  qTrackCountCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
  qTrackCountCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qTrackCountCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("false");
  qTrackCountCmd->SetParameter(param);

  //
  qTerminationCmd = new G4UIcommand("/score/quantity/termination",this);
  qTerminationCmd->SetGuidance("Number of Terminated tracks Scorer");
  qTerminationCmd->
      SetGuidance("[usage] /score/quantiy/termination qname wflag");
  qTerminationCmd->SetGuidance("  qname  :(String) scorer name");
  qTerminationCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
  qTerminationCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("false");
  qTerminationCmd->SetParameter(param);
}

