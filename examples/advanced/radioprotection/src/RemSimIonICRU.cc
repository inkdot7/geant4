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
#include "RemSimIonICRU.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4MultipleScattering.hh"
#include "G4hLowEnergyIonisation.hh"
#include "G4hLowEnergyLoss.hh"

RemSimIonICRU::RemSimIonICRU(const G4String& name): G4VPhysicsConstructor(name)
{ }

RemSimIonICRU::~RemSimIonICRU()
{ }

void RemSimIonICRU::ConstructProcess()
{
  theParticleIterator -> reset();

  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator -> value();
      G4ProcessManager* manager = particle -> GetProcessManager();
      G4String particleName = particle -> GetParticleName();
      G4double charge = particle -> GetPDGCharge();

      if (charge!= 0.0)
	{
      if (particleName == "proton" || particleName == "alpha"
          || particleName == "IonO16" || 
          particleName == "IonC12"
          || particleName == "IonFe52" || particleName == "IonSi28") 
	{
	  G4hLowEnergyIonisation* ionisation = new G4hLowEnergyIonisation();
          G4VProcess*  multipleScattering = new G4MultipleScattering(); 
          manager->AddProcess(ionisation,-1,2,2);
          manager->AddProcess(multipleScattering,-1,1,1);  	
	}
  
      else if( (particleName != "e+") &&  (particleName != "e-") && 
	      (particleName != "mu+") && (particleName != "mu-"))
	{ 
	  if(  (!particle->IsShortLived()) &&
	       (particle->GetParticleName() != "chargedgeantino"))
	    {
	      G4MultipleScattering* aMultipleScattering = 
                                                 new G4MultipleScattering();

	      G4hLowEnergyIonisation* ahadronLowEIon =
                                                 new G4hLowEnergyIonisation();

	      manager->AddProcess(aMultipleScattering,-1,1,1);
	      manager->AddProcess(ahadronLowEIon,     -1,2,2);   
	    }
	}
	}
    }
}
