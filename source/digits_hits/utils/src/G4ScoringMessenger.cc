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
// $Id: G4ScoringMessenger.cc,v 1.13 2007-09-04 09:39:53 taso Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// ---------------------------------------------------------------------

#include "G4ScoringMessenger.hh"
#include "G4ScoringManager.hh"
#include "G4VScoringMesh.hh"
#include "G4ScoringBox.hh"

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


#include "G4SDParticleFilter.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcommand.hh"
#include "G4Tokenizer.hh"

G4ScoringMessenger::G4ScoringMessenger(G4ScoringManager* SManager)
:fSMan(SManager)
{
  G4UIparameter* param;

  scoreDir = new G4UIdirectory("/score/");
  scoreDir->SetGuidance("Interactive scoring commands.");

  listCmd = new G4UIcmdWithoutParameter("/score/list",this);
  listCmd->SetGuidance("List scoring worlds.");

  verboseCmd = new G4UIcmdWithAnInteger("/score/verbose",this);
  verboseCmd->SetGuidance("Verbosity");

  meshCreateDir = new G4UIdirectory("/score/create/");
  meshCreateDir->SetGuidance("Interactive scoring commands.");
  //
  // Mesh commands
  meshBoxCreateCmd = new G4UIcmdWithAString("/score/create/boxMesh",this);
  meshBoxCreateCmd->SetGuidance("Create scoring mesh.");
  meshBoxCreateCmd->SetParameterName("MeshName",false);
  //
  meshTubsCreateCmd = new G4UIcmdWithAString("/score/create/tubsMesh",this);
  meshTubsCreateCmd->SetGuidance("Create scoring mesh.");
  meshTubsCreateCmd->SetParameterName("MeshName",false);
  //
  meshSphereCreateCmd = new G4UIcmdWithAString("/score/create/sphereMesh",this);
  meshSphereCreateCmd->SetGuidance("Create scoring mesh.");
  meshSphereCreateCmd->SetParameterName("MeshName",false);
  //
  meshOpnCmd = new G4UIcmdWithAString("/score/open",this);
  meshOpnCmd->SetGuidance("Open scoring mesh.");
  meshOpnCmd->SetParameterName("MeshName",false);
  //
  meshClsCmd = new G4UIcmdWithoutParameter("/score/close",this);
  meshClsCmd->SetGuidance("Close scoring mesh.");
  //
  meshActCmd = new G4UIcmdWithABool("/score/mesh/activate",this);
  meshActCmd->SetGuidance("Activate scoring mesh.");
  meshActCmd->SetParameterName("MeshName",false);
  //
  mBoxSizeCmd = new G4UIcmdWith3VectorAndUnit("/score/mesh/boxsize",this);
  mBoxSizeCmd->SetGuidance("Define Size of scoring mesh.");
  mBoxSizeCmd->SetParameterName("Di","Dj","Dk",false,false);
  mBoxSizeCmd->SetDefaultUnit("mm");
  //
  //   Division command
  mBinCmd = new G4UIcommand("/score/mesh/nbin",this);
  mBinCmd->SetGuidance("Define segmentation of scoring mesh.");
  mBinCmd->SetGuidance("[usage] /score/mesh/nbin");
  mBinCmd->SetGuidance("  Ni  :(int) Number of bins i ");
  mBinCmd->SetGuidance("  Nj  :(int) Number of bins j ");
  mBinCmd->SetGuidance("  Nk  :(int) Number of bins k ");
  mBinCmd->SetGuidance("  Axis:(int) Axis of division ");
  mBinCmd->SetGuidance("  P1..Pn-1  :(double) \"paramter from P1 to Pn-1 for division.\"");
  param = new G4UIparameter("Ni",'i',false);
  param->SetDefaultValue("1");
  mBinCmd->SetParameter(param);
  param = new G4UIparameter("Nj",'i',false);
  param->SetDefaultValue("1");
  mBinCmd->SetParameter(param);
  param = new G4UIparameter("Nk",'i',false);
  param->SetDefaultValue("1");
  mBinCmd->SetParameter(param);
  //param = new G4UIparameter("Axis",'i',true);
  //param->SetDefaultValue("3");
  //mBinCmd->SetParameter(param);
  //
  //   Placement command
  mTransDir = new G4UIdirectory("/score/mesh/translate/");
  mTransDir->SetGuidance("Placement of scoring mesh");
  //
  mTResetCmd = new G4UIcmdWithoutParameter("/score/mesh/translate/reset",this);
  mTResetCmd->SetGuidance("Reset translation of scoring mesh placement.");
  //
  mTXyzCmd = new G4UIcmdWith3VectorAndUnit("/score/mesh/translate/xyz",this);
  mTXyzCmd->SetGuidance("Translation the current scoring mesh to the position.");
  mTXyzCmd->SetParameterName("X","Y","Z",false,false);
  mTXyzCmd->SetDefaultUnit("mm");
  //
  mRotDir = new G4UIdirectory("/score/mesh/rotate/");
  mRotDir->SetGuidance("Placement of scoring mesh");
  //
  mRResetCmd = new G4UIcmdWithoutParameter("/score/mesh/rotate/reset",this);
  mRResetCmd->SetGuidance("Reset rotation of scoring mesh placement.");
  //
  mRotXCmd = new G4UIcmdWithADoubleAndUnit("/score/mesh/rotate/rotX",this);
  mRotXCmd->SetGuidance("Add rotation to the current scoring mesh in X.");
  mRotXCmd->SetParameterName("Rx",false);
  mRotXCmd->SetDefaultUnit("deg");
  //
  mRotYCmd = new G4UIcmdWithADoubleAndUnit("/score/mesh/rotate/rotY",this);
  mRotYCmd->SetGuidance("Add rotation to the current scoring mesh in Y.");
  mRotYCmd->SetParameterName("Ry",false);
  mRotYCmd->SetDefaultUnit("deg");
  //
  mRotZCmd = new G4UIcmdWithADoubleAndUnit("/score/mesh/rotate/rotZ",this);
  mRotZCmd->SetGuidance("Add rotation to the current scoring mesh in Z.");
  mRotZCmd->SetParameterName("Rz",false);
  mRotZCmd->SetDefaultUnit("deg");
  //

  // Dump Scoring result
  dumpCmd = new G4UIcmdWithAString("/score/dump",this);
  dumpCmd->SetGuidance("Dump scorer results ");
  dumpCmd->SetParameterName("qname",true);

  //
  // Quantity commands
  quantityDir = new G4UIdirectory("/score/quantity/");
  quantityDir->SetGuidance("Scoring quantity of the mesh");
  //
  qTouchCmd= new G4UIcmdWithAString("/score/quantity/touch",this);
  qTouchCmd->SetGuidance("Assign previously defined quantity to current quantity.");
  qTouchCmd->SetParameterName("qname",false);
  //
  qeDepCmd = new G4UIcmdWithAString("/score/quantity/eDep",this);
  qeDepCmd->SetGuidance("Energy Deposit Scorer");
  qeDepCmd->SetParameterName("qname",false);
  qCellChgCmd  = new G4UIcmdWithAString("/score/quantity/cellCharge",this);
  qCellChgCmd->SetGuidance("Cell Charge Scorer");
  qCellChgCmd->SetParameterName("qname",false);
  qCellFluxCmd = new G4UIcmdWithAString("/score/quantity/cellFlux",this);
  qCellFluxCmd->SetGuidance("Cell Flux Scorer");
  qCellFluxCmd->SetParameterName("qname",false);
  qPassCellFluxCmd = new G4UIcmdWithAString("/score/quantity/passageCellFlux",this);
  qPassCellFluxCmd->SetGuidance("Passage Cell Flux Scorer");
  qPassCellFluxCmd->SetParameterName("qname",false);
  qdoseDepCmd = new G4UIcmdWithAString("/score/quantity/doseDeposit",this);
  qdoseDepCmd->SetGuidance("Dose Deposit Scorer");
  qdoseDepCmd->SetParameterName("qname",false);
  qnOfStepCmd = new G4UIcmdWithAString("/score/quantity/nOfStep",this);
  qnOfStepCmd->SetGuidance("Number of Step Scorer ");
  qnOfStepCmd->SetParameterName("qname",false);
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
  param->SetDefaultValue("false");
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
  param->SetDefaultValue("false");
  qPassTrackLengthCmd->SetParameter(param);

  //
  qFlatSurfCurrCmd = new G4UIcommand("/score/quantity/flatSurfCurrent",this);
  qFlatSurfCurrCmd->SetGuidance("Flat surface current Scorer");
  qFlatSurfCurrCmd->
      SetGuidance("[usage] /score/quantiy/flatSurfCurrent qname dflag wflag aflag");
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
  param->SetDefaultValue("truey");
  qFlatSurfCurrCmd->SetParameter(param);

  //
  qFlatSurfFluxCmd = new G4UIcommand("/score/quantity/flatSurfFlux",this);
  qFlatSurfFluxCmd->SetGuidance("Flat surface Flux Scorer");
  qFlatSurfFluxCmd->
      SetGuidance("[usage] /score/quantiy/flatSurfFlux qname dflag");
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
  qSphereSurfCurrCmd = new G4UIcommand("/score/quantity/sphereSurfCurrent",this);
  qSphereSurfCurrCmd->SetGuidance("Sphere surface current Scorer");
  qSphereSurfCurrCmd->
      SetGuidance("[usage] /score/quantiy/sphereSurfCurrent qname dflag wflag aflag");
  qSphereSurfCurrCmd->SetGuidance("  qname  :(String) scorer name");
  qSphereSurfCurrCmd->SetGuidance("  dflag  :(Int) direction flag");
  qSphereSurfCurrCmd->SetGuidance("         : 0 = Both In and Out");
  qSphereSurfCurrCmd->SetGuidance("         : 1 = In only");
  qSphereSurfCurrCmd->SetGuidance("         : 2 = Out only");
  qSphereSurfCurrCmd->SetGuidance("  wflag  :(Bool) Weighted");
  qSphereSurfCurrCmd->SetGuidance("  aflag  :(Bool) DivideByArea");
  param = new G4UIparameter("qname",'s',false);
  qSphereSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qSphereSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("true");
  qSphereSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("aflag",'b',true);
  param->SetDefaultValue("truey");
  qSphereSurfCurrCmd->SetParameter(param);

  //
  qSphereSurfFluxCmd = new G4UIcommand("/score/quantity/sphereSurfFlux",this);
  qSphereSurfFluxCmd->SetGuidance("Sphere surface Flux Scorer");
  qSphereSurfFluxCmd->
      SetGuidance("[usage] /score/quantiy/sphereSurfFlux qname dflag");
  qSphereSurfFluxCmd->SetGuidance("  qname  :(String) scorer name");
  qSphereSurfFluxCmd->SetGuidance("  dflag  :(Int) direction flag");
  qSphereSurfFluxCmd->SetGuidance("         : 0 = Both In and Out");
  qSphereSurfFluxCmd->SetGuidance("         : 1 = In only");
  qSphereSurfFluxCmd->SetGuidance("         : 2 = Out only");
  param = new G4UIparameter("qname",'s',false);
  qSphereSurfFluxCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qSphereSurfFluxCmd->SetParameter(param);

  //
  qCylSurfCurrCmd = new G4UIcommand("/score/quantity/cylSurfCurrent",this);
  qCylSurfCurrCmd->SetGuidance("Cylinder surface current Scorer");
  qCylSurfCurrCmd->
      SetGuidance("[usage] /score/quantiy/cylSurfCurrent qname dflag wflag aflag");
  qCylSurfCurrCmd->SetGuidance("  qname  :(String) scorer name");
  qCylSurfCurrCmd->SetGuidance("  dflag  :(Int) direction flag");
  qCylSurfCurrCmd->SetGuidance("         : 0 = Both In and Out");
  qCylSurfCurrCmd->SetGuidance("         : 1 = In only");
  qCylSurfCurrCmd->SetGuidance("         : 2 = Out only");
  qCylSurfCurrCmd->SetGuidance("  wflag  :(Bool) Weighted");
  qCylSurfCurrCmd->SetGuidance("  aflag  :(Bool) DivideByArea");
  param = new G4UIparameter("qname",'s',false);
  qCylSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qCylSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("wflag",'b',true);
  param->SetDefaultValue("true");
  qCylSurfCurrCmd->SetParameter(param);
  param = new G4UIparameter("aflag",'b',true);
  param->SetDefaultValue("truey");
  qCylSurfCurrCmd->SetParameter(param);

  //
  qCylSurfFluxCmd = new G4UIcommand("/score/quantity/cylSurfFlux",this);
  qCylSurfFluxCmd->SetGuidance("Cylinder surface Flux Scorer");
  qCylSurfFluxCmd->
      SetGuidance("[usage] /score/quantiy/cylSurfFlux qname dflag");
  qCylSurfFluxCmd->SetGuidance("  qname  :(String) scorer name");
  qCylSurfFluxCmd->SetGuidance("  dflag  :(Int) direction flag");
  qCylSurfFluxCmd->SetGuidance("         : 0 = Both In and Out");
  qCylSurfFluxCmd->SetGuidance("         : 1 = In only");
  qCylSurfFluxCmd->SetGuidance("         : 2 = Out only");
  param = new G4UIparameter("qname",'s',false);
  qCylSurfFluxCmd->SetParameter(param);
  param = new G4UIparameter("dflag",'i',true);
  param->SetDefaultValue("0");
  qCylSurfFluxCmd->SetParameter(param);
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
      SetGuidance("[usage] /score/quantiy/trackCounter qname wflag");
  qTrackCountCmd->SetGuidance("  qname  :(String) scorer name");
  qTrackCountCmd->SetGuidance("  wflag  :(Bool) Weighted");
  param = new G4UIparameter("qname",'s',false);
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

  //
  // Filter commands 
  filterDir = new G4UIdirectory("/score/filter/");
  filterDir->SetGuidance("Filter for scoring");
  //
  fchargedCmd = new G4UIcmdWithAString("/score/filter/charged",this);
  fchargedCmd->SetGuidance("Charge filter ( charged )");
  fchargedCmd->SetParameterName("fname",false);
  //
  fneutralCmd = new G4UIcmdWithAString("/score/filter/neutral",this);
  fneutralCmd->SetGuidance("Charge filter ( Neutral )");
  fneutralCmd->SetParameterName("fname",false);
  //
  fkinECmd = new G4UIcommand("/score/filter/kinE",this);
  fkinECmd->SetGuidance("Kinetic Energy Filter");
  fkinECmd->SetGuidance("[usage] /score/filter/kinE fname Elow Ehigh unit");
  fkinECmd->SetGuidance("  fname     :(String) Filter Name ");
  fkinECmd->SetGuidance("  Elow      :(Double) Lower edge of kinetic energy");
  fkinECmd->SetGuidance("  Ehigh     :(Double) Higher edge of kinetic energy");
  fkinECmd->SetGuidance("  unit      :(String) unit of given kinetic energy");
  param = new G4UIparameter("fname",'s',false);
  fkinECmd->SetParameter(param);
  param = new G4UIparameter("elow",'d',false);
  fkinECmd->SetParameter(param);
  param = new G4UIparameter("ehigh",'d',false);
  fkinECmd->SetParameter(param);
  param = new G4UIparameter("unit",'s',false);
  fkinECmd->SetParameter(param);
  //
  fparticleCmd = new G4UIcommand("/score/filter/particle",this);
  fparticleCmd->SetGuidance("Touch particle filter into current quantity");
  fparticleCmd->SetGuidance("[usage] /score/filter/particle fname p0 .. pn");
  fparticleCmd->SetGuidance("  fname     :(String) Filter Name ");
  fparticleCmd->SetGuidance("  p0 .. pn  :(String) particle names");
  param = new G4UIparameter("fname",'s',false);
  fparticleCmd->SetParameter(param);
  param = new G4UIparameter("particlelist",'s',false);
  fparticleCmd->SetParameter(param);
  //
  //
  //
  fparticleKinECmd = new G4UIcommand("/score/filter/particleWithKinE",this);
  fparticleKinECmd->SetGuidance("Particle with kinetic energy filter");
  fparticleKinECmd->SetGuidance("[usage] /score/filter/particleWithKinE fname Elow Ehigh unit p0 .. pn");
  fparticleKinECmd->SetGuidance("  fname     :(String) Filter Name ");
  fparticleKinECmd->SetGuidance("  Elow      :(Double) Lower edge of kinetic energy");
  fparticleKinECmd->SetGuidance("  Ehigh     :(Double) Higher edge of kinetic energy");
  fparticleKinECmd->SetGuidance("  unit      :(String) unit of given kinetic energy");
  fparticleKinECmd->SetGuidance("  p0 .. pn  :(String) particle names");
  param = new G4UIparameter("fname",'s',false);
  fparticleKinECmd->SetParameter(param);
  param = new G4UIparameter("elow",'d',false);
  fparticleKinECmd->SetParameter(param);
  param = new G4UIparameter("ehigh",'d',false);
  fparticleKinECmd->SetParameter(param);
  param = new G4UIparameter("unit",'s',false);
  fparticleKinECmd->SetParameter(param);
  param = new G4UIparameter("name",'s',false);
  fparticleKinECmd->SetParameter(param);
  param = new G4UIparameter("particlelist",'s',false);
  fparticleKinECmd->SetParameter(param);
  //
  //

}

G4ScoringMessenger::~G4ScoringMessenger()
{
    delete scoreDir;
    delete listCmd;
    delete verboseCmd;
    //
    delete           meshCreateDir;
    delete           meshBoxCreateCmd;
    delete           meshTubsCreateCmd;
    delete           meshSphereCreateCmd;
    //
    delete          meshDir;
    delete          meshOpnCmd;
    //
    delete    meshClsCmd;
    delete    meshActCmd;
    //
    delete  mBoxSizeCmd;
    delete  mTubsSizeCmd;
    delete  mSphereSizeCmd;
    //
    delete      mBinCmd;
    //
    delete   mTransDir;
    delete   mTResetCmd;
    delete   mTXyzCmd;
    delete   mRotDir;
    delete   mRResetCmd;
    delete   mRotXCmd;
    delete   mRotYCmd;
    delete   mRotZCmd;
    //
    delete     dumpCmd;
    //
    delete         quantityDir;
    delete         qTouchCmd;
    //
    delete    qCellChgCmd;
    delete    qCellFluxCmd;
    delete    qPassCellFluxCmd;
    delete    qeDepCmd;
    delete    qdoseDepCmd;
    delete    qnOfStepCmd;
    delete    qnOfSecondaryCmd;
    //
    delete          qTrackLengthCmd;
    delete          qPassCellCurrCmd;
    delete          qPassTrackLengthCmd;
    delete          qFlatSurfCurrCmd;
    delete          qFlatSurfFluxCmd;
    delete          qSphereSurfCurrCmd;
    delete          qSphereSurfFluxCmd;
    delete          qCylSurfCurrCmd;
    delete          qCylSurfFluxCmd;
    delete          qNofCollisionCmd;
    delete          qPopulationCmd;
    delete          qTrackCountCmd;
    delete          qTerminationCmd;
    //
    delete   filterDir;
    delete   fchargedCmd;
    delete   fneutralCmd;
    delete   fkinECmd;
    delete   fparticleCmd;
    delete   fparticleKinECmd;
}

void G4ScoringMessenger::SetNewValue(G4UIcommand * command,G4String newVal)
{
  if(command==listCmd) { 
      fSMan->List(); 
  } else if(command==dumpCmd) { 
      fSMan->Dump(); 
  } else if(command==verboseCmd) { 
      fSMan->SetVerboseLevel(verboseCmd->GetNewIntValue(newVal)); 
  } else if(command==meshBoxCreateCmd) {
      G4VScoringMesh*  mesh = fSMan->FindMesh(newVal);
      if ( !mesh ){
	  mesh = new G4ScoringBox(newVal);
	  fSMan->RegisterScoringMesh(mesh);
      }else{
	  G4Exception("G4ScroingMessenger:: Mesh has already existed. Error!");
      }
  } else if(command==meshOpnCmd) {
      G4VScoringMesh* currentmesh = fSMan->GetCurrentMesh(); 
      if ( currentmesh ){
 	  G4Exception("G4ScroingMessenger:: Close current mesh first!. Error!");
      }
      G4VScoringMesh* mesh = fSMan->FindMesh(newVal); 
      if ( !mesh ){
 	  G4Exception("G4ScroingMessenger:: Mesh has not existed. Error!");
      }
  } else {
      //
      // Get Current Mesh
      //
      G4VScoringMesh* mesh = fSMan->GetCurrentMesh();
      //
      // Commands for Current Mesh
      if ( mesh ){
	  if(command==meshClsCmd) {
	      fSMan->CloseCurrentMesh();
	  } else if(command==meshActCmd) {
	      mesh->Activate(meshActCmd->GetNewBoolValue(newVal)); 
	  } else if(command==mBoxSizeCmd) {
	      MeshShape shape = mesh->GetShape();
	      if ( shape == boxMesh ){
		  G4ThreeVector size = mBoxSizeCmd->GetNew3VectorValue(newVal);
		  G4double vsize[3];
		  vsize[0] = size.x();
		  vsize[1] = size.y();
		  vsize[2] = size.z();
		  mesh->SetSize(vsize);
	      } else {
		  G4Exception("G4ScroingMessenger:: Mesh is not Box type. Error!");
	      }
	  } else if(command==mBinCmd) {
	      MeshBinCommand(mesh,newVal);
	  } else if(command==mTResetCmd) {
	      G4double centerPosition[3] ={ 0., 0., 0.};
	      mesh->SetCenterPosition(centerPosition);
	  } else if(command==mTXyzCmd) {
	      G4ThreeVector xyz = mTXyzCmd->GetNew3VectorValue(newVal);
	      G4double centerPosition[3];
	      centerPosition[0] = xyz.x();
	      centerPosition[1] = xyz.y();
	      centerPosition[2] = xyz.z();
	      mesh->SetCenterPosition(centerPosition);
	  } else if(command==mRResetCmd) {
	  } else if(command==mRotXCmd) {
	      G4double value = mRotXCmd->GetNewDoubleValue(newVal);
	      mesh->RotateX(value);
	  } else if(command==mRotYCmd) {
	      G4double value = mRotYCmd->GetNewDoubleValue(newVal);
	      mesh->RotateY(value);
	  } else if(command==mRotZCmd) {
	      G4double value = mRotZCmd->GetNewDoubleValue(newVal);
	      mesh->RotateZ(value);
	  } else if(command==qTouchCmd) {
	      mesh->SetCurrentPrimitiveScorer(newVal);
	  } else if(command== qCellChgCmd) {
	      mesh->SetPrimitiveScorer(new G4PSCellCharge3D(newVal));
	  } else if(command== qCellFluxCmd) {
	      mesh->SetPrimitiveScorer(new G4PSCellFlux3D(newVal));
	  } else if(command== qPassCellFluxCmd) {
	      mesh->SetPrimitiveScorer(new G4PSPassageCellFlux3D(newVal));
	  } else if(command==qeDepCmd) {
	      mesh->SetPrimitiveScorer(new G4PSEnergyDeposit3D(newVal));
	  } else if(command== qdoseDepCmd) {
	      mesh->SetPrimitiveScorer(new G4PSDoseDeposit3D(newVal));
	  } else if(command== qnOfStepCmd) {
	      mesh->SetPrimitiveScorer(new G4PSNofStep3D(newVal));
	  } else if(command== qTrackLengthCmd) {
	      PSTrackLength(mesh,newVal);
	  } else if(command== qnOfSecondaryCmd) {
	      mesh->SetPrimitiveScorer(new G4PSNofSecondary3D(newVal));
	  } else if(command==fparticleCmd) {
	      FParticleCommand(mesh,newVal);
	  }
      }else{
	  G4Exception("G4ScroingMessenger:: Current Mesh has not opened. Error!");
      }
  }
}

G4String G4ScoringMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String val;
  if(command==verboseCmd)
  { val = verboseCmd->ConvertToString(fSMan->GetVerboseLevel()); }

  return val;
}

void G4ScoringMessenger::MeshBinCommand(G4VScoringMesh* mesh,G4String newVal){
    G4Tokenizer next(newVal);
    G4int Ni = StoI(next());
    G4int Nj = StoI(next());
    G4int Nk = StoI(next());
    G4int nSegment[3];
    nSegment[0] = Ni;
    nSegment[1] = Nj;
    nSegment[2] = Nk;
    //
    mesh->SetNumberOfSegments(nSegment);
    //
    //
    /*
    G4int iAxis = 3;
    G4String Axis = next();
    if ( Axis.isNull() ) {
    } else {
	iAxis = StoI(Axis);
	//
	//==== Implementation for variable bin size Here
	//
	//  .........
    }
    */
}

void G4ScoringMessenger::PSTrackLength(G4VScoringMesh* mesh, G4String newValues){
    G4Tokenizer next(newValues);
    //
    G4String name = next();
    //
    G4bool weighted              = StoB(next());
    G4bool multiplyKineticEnergy = StoB(next());
    G4bool divideByVelocity      = StoB(next());
    //
    G4PSTrackLength3D* ps = new G4PSTrackLength3D(name);
    ps->Weighted(weighted);
    ps->MultiplyKineticEnergy(multiplyKineticEnergy);
    ps->DivideByVelocity(divideByVelocity);
    mesh->SetPrimitiveScorer(ps);
}

void G4ScoringMessenger::FParticleCommand(G4VScoringMesh* mesh, G4String newVal){
    G4cout << "####"<<newVal<<"#####"<<G4endl;
    G4Tokenizer next(newVal);
    //
    // Filter name
    G4String name = next();
    //
    // particle list
    std::vector<G4String> pnames;
    G4String pname;
    while ( !(pname = next()).isNull() ) {
	if ( pname.first('"')==0 ) {
	    G4cout << "  Hit first " <<G4endl;
	    pname.remove(0,1);
	}
	if ( (pname.last('"')) == (G4int)(pname.length()-1) ){
	    pname.remove(pname.length()-1,1);
	}
	pnames.push_back(pname);
    }
    //
    // Attach Filter
    mesh->SetFilter(new G4SDParticleFilter(name,pnames));
}    

