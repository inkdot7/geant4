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
// HadrontherapyRunAction.hh
// See more at: http://g4advancedexamples.lngs.infn.it/Examples/hadrontherapy

#ifndef HadrontherapyRunAction_h
#define HadrontherapyRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "globals.hh"

#include "TCanvas.h"
#include "TGraph.h"
class G4Run;
class HadrontherapyAnalysisManager;
class HadrontherapyDetectorConstruction;
class HadrontherapyRunMessenger;
class HadrontherapyFactory;

class HadrontherapyRunAction : public G4UserRunAction
{
public:
  HadrontherapyRunAction();
  ~HadrontherapyRunAction();

public:
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run* );
  void SelectEnergy(G4int); 

  void AddEMProcess();
  // Counts the number of electromagnetic processes
  // of primary particles in the phantom

  void AddHadronicProcess();
  // Counts the number of hadronic processes 
  // of primary particles in the phantom

private:  
  TCanvas *c1;
  TGraph *graph;
  G4int electromagnetic;
  G4int hadronic;
};
#endif



