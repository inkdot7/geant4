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
// $Id: RE02IonPhysics.cc,v 1.1 2005-11-24 01:44:18 asaim Exp $
// --------------------------------------------------------------
//
// 13-Oct-2003 Add Comment for Ionisation of Generic Ion by T. Koi 
// 05-Jan-2004 Change G. Ion Ionisation from G4hIonisation
//                                      to G4ionIonisation T. Koi
// 18-Nov-2005 Add Inelastic process with G4BinaryLightIonReaction.
//                                            T. Aso

#include "RE02IonPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>


RE02IonPhysics::RE02IonPhysics(const G4String& name)
                 :  G4VPhysicsConstructor(name)
{
}

RE02IonPhysics::~RE02IonPhysics()
{
}

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4ProcessManager.hh"


void RE02IonPhysics::ConstructProcess()
{
   G4ProcessManager * pManager = 0;


   // Generic Ion
   pManager = G4GenericIon::GenericIon()->GetProcessManager();

   // add process
   G4VProcess* thegionMultipleScattering = new G4MultipleScattering();
   //
   // G4hIonization may be not able to use for Geanric Ion in future 
   // Please take care using this physics list after v5.2.p02 
   // G4VProcess* thegionIonisation        = new G4hIonisation();
   //
   // From V6.0 hIonisation does not work for GenericIon 
   G4VProcess* thegionIonisation        = new G4ionIonisation();

   // Inelastic process
   G4HadronInelasticProcess* thegionInelastic =
       new G4HadronInelasticProcess("IonInelastic",G4GenericIon::GenericIon());
   thegionInelastic->AddDataSet(new G4TripathiCrossSection);
   thegionInelastic->AddDataSet(new G4IonsShenCrossSection);
   G4BinaryLightIonReaction* thegionBCModel = new G4BinaryLightIonReaction;
   thegionInelastic->RegisterMe(thegionBCModel);

   //
   pManager->AddProcess(thegionIonisation);
   pManager->AddProcess(thegionMultipleScattering);
   pManager->AddDiscreteProcess(thegionInelastic);
   //
   // set ordering for AlongStepDoIt
   pManager->SetProcessOrdering(thegionMultipleScattering, idxAlongStep,1);
   pManager->SetProcessOrdering(thegionIonisation,        idxAlongStep,2);
   //
   // set ordering for PostStepDoIt
   pManager->SetProcessOrdering(thegionMultipleScattering, idxPostStep,1);
   pManager->SetProcessOrdering(thegionIonisation,        idxPostStep,2);


   // Deuteron
   pManager = G4Deuteron::Deuteron()->GetProcessManager();

   // add process
   G4HadronElasticProcess* thedueElasticProcess
                         = new G4HadronElasticProcess();
   G4LElastic* thedueElasticModel = new G4LElastic();
   thedueElasticProcess->RegisterMe(thedueElasticModel);
   pManager->AddDiscreteProcess(thedueElasticProcess);

   G4DeuteronInelasticProcess* theDeuteronInelasticProcess
                         = new G4DeuteronInelasticProcess();

   G4LEDeuteronInelastic* theDeuteronLEPModel = new G4LEDeuteronInelastic();
   theDeuteronInelasticProcess->RegisterMe(theDeuteronLEPModel);
   pManager->AddDiscreteProcess(theDeuteronInelasticProcess);

   G4VProcess* thedueMultipleScattering = new G4MultipleScattering();
   G4VProcess* thedueIonisation        = new G4hIonisation();
   //
   pManager->AddProcess(thedueIonisation);
   pManager->AddProcess(thedueMultipleScattering);
   //
   // set ordering for AlongStepDoIt
   pManager->SetProcessOrdering(thedueMultipleScattering, idxAlongStep,1);
   pManager->SetProcessOrdering(thedueIonisation,        idxAlongStep,2);
   //
   // set ordering for PostStepDoIt
   pManager->SetProcessOrdering(thedueMultipleScattering, idxPostStep,1);
   pManager->SetProcessOrdering(thedueIonisation,        idxPostStep,2);


   // Triton
   pManager = G4Triton::Triton()->GetProcessManager();

   // add process
   G4HadronElasticProcess* thetriElasticProcess
                         = new G4HadronElasticProcess();
   G4LElastic* thetriElasticModel = new G4LElastic();
   thetriElasticProcess->RegisterMe(thetriElasticModel);
   pManager->AddDiscreteProcess(thetriElasticProcess);

   G4TritonInelasticProcess* theTritonInelasticProcess
                         = new G4TritonInelasticProcess();

   G4LETritonInelastic* theTritonLEPModel = new G4LETritonInelastic();
   theTritonInelasticProcess->RegisterMe(theTritonLEPModel);
   pManager->AddDiscreteProcess(theTritonInelasticProcess);

   G4VProcess* thetriMultipleScattering = new G4MultipleScattering();
   G4VProcess* thetriIonisation        = new G4hIonisation();
   //
   pManager->AddProcess(thetriIonisation);
   pManager->AddProcess(thetriMultipleScattering);
   //
   // set ordering for AlongStepDoIt
   pManager->SetProcessOrdering(thetriMultipleScattering, idxAlongStep,1);
   pManager->SetProcessOrdering(thetriIonisation,        idxAlongStep,2);
   //
   // set ordering for PostStepDoIt
   pManager->SetProcessOrdering(thetriMultipleScattering, idxPostStep,1);
   pManager->SetProcessOrdering(thetriIonisation,        idxPostStep,2);


   // Alpha
   pManager = G4Alpha::Alpha()->GetProcessManager();

   // add processes
   G4HadronElasticProcess* thealElasticProcess
                         = new G4HadronElasticProcess();
   G4LElastic* thealElasticModel = new G4LElastic();
   thealElasticProcess->RegisterMe(thealElasticModel);
   pManager->AddDiscreteProcess(thealElasticProcess);

   G4AlphaInelasticProcess* theAlphaInelasticProcess
                         = new G4AlphaInelasticProcess();

   G4LEAlphaInelastic* theAlphaLEPModel = new G4LEAlphaInelastic();
   theAlphaInelasticProcess->RegisterMe(theAlphaLEPModel);
   pManager->AddDiscreteProcess(theAlphaInelasticProcess);

   G4VProcess* thealpMultipleScattering = new G4MultipleScattering();
   G4VProcess* thealpIonisation        = new G4hIonisation();
   //
   pManager->AddProcess(thealpIonisation);
   pManager->AddProcess(thealpMultipleScattering);
   //
   // set ordering for AlongStepDoIt
   pManager->SetProcessOrdering(thealpMultipleScattering, idxAlongStep,1);
   pManager->SetProcessOrdering(thealpIonisation,        idxAlongStep,2);
   //
   // set ordering for PostStepDoIt
   pManager->SetProcessOrdering(thealpMultipleScattering, idxPostStep,1);
   pManager->SetProcessOrdering(thealpIonisation,        idxPostStep,2);


   // He3
   pManager = G4He3::He3()->GetProcessManager();

   // add processes
   G4HadronElasticProcess* thehe3ElasticProcess
                         = new G4HadronElasticProcess();
   G4LElastic* thehe3ElasticModel = new G4LElastic();
   thehe3ElasticProcess->RegisterMe(thehe3ElasticModel);
   pManager->AddDiscreteProcess(thehe3ElasticProcess);

   G4VProcess* thehe3MultipleScattering = new G4MultipleScattering();
   G4VProcess* thehe3Ionisation        = new G4hIonisation();
   //
   pManager->AddProcess(thehe3Ionisation);
   pManager->AddProcess(thehe3MultipleScattering);
   //
   // set ordering for AlongStepDoIt
   pManager->SetProcessOrdering(thehe3MultipleScattering, idxAlongStep,1);
   pManager->SetProcessOrdering(thehe3Ionisation,        idxAlongStep,2);
   //
   // set ordering for PostStepDoIt
   pManager->SetProcessOrdering(thehe3MultipleScattering, idxPostStep,1);
   pManager->SetProcessOrdering(thehe3Ionisation,        idxPostStep,2);

}



