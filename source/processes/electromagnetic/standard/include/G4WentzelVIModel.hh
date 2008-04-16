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
// $Id: G4WentzelVIModel.hh,v 1.2 2008-04-16 10:17:03 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
//
// GEANT4 Class header file
//
//
// File name:     G4WentzelVIModel
//
// Author:        V.Ivanchenko 
//
// Creation date: 09.04.2008 from G4MuMscModel
//
// Modifications:
//
//
// Class Description:
//
// Implementation of the model of multiple scattering based on
// G.Wentzel, Z. Phys. 40 (1927) 590.
// H.W.Lewis, Phys Rev 78 (1950) 526.
// J.M. Fernandez-Varea et al., NIM B73 (1993) 447.
// L.Urban, CERN-OPEN-2006-077.

// -------------------------------------------------------------------
//

#ifndef G4WentzelVIModel_h
#define G4WentzelVIModel_h 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4VMscModel.hh"
#include "G4PhysicsTable.hh"
#include "G4MscStepLimitType.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4NistManager.hh"

class G4LossTableManager;
class G4ParticleChangeForMSC;
class G4SafetyHelper;
class G4ParticleDefinition;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4WentzelVIModel : public G4VMscModel
{

public:

  G4WentzelVIModel(G4double thetaMax = 0.5,
		   G4double tMax = TeV*TeV, 
		   const G4String& nam = "WentzelVIUni");

  virtual ~G4WentzelVIModel();

  void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  G4double ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
				      G4double KineticEnergy,
				      G4double AtomicNumber,
				      G4double AtomicWeight=0., 
				      G4double cut = DBL_MAX,
				      G4double emax= DBL_MAX);

  void SampleScattering(const G4DynamicParticle*, G4double safety);

  void SampleSecondaries(std::vector<G4DynamicParticle*>*, 
			 const G4MaterialCutsCouple*,
			 const G4DynamicParticle*,
			 G4double,
			 G4double);

  G4double ComputeTruePathLengthLimit(const G4Track& track,
				      G4PhysicsTable* theLambdaTable,
				      G4double currentMinimalStep);

  G4double ComputeGeomPathLength(G4double truePathLength);

  G4double ComputeTrueStepLength(G4double geomStepLength);

private:

  G4double ComputeXSectionPerVolume();

  void ComputeMaxElectronScattering(G4double cut);

  inline G4double GetLambda(G4double kinEnergy);

  inline void SetupParticle(const G4ParticleDefinition*);

  inline void SetupKinematic(G4double kinEnergy, G4double cut);
  
  inline void SetupTarget(G4double Z, G4double A, G4double kinEnergy);

  inline void DefineMaterial(const G4MaterialCutsCouple*);

  inline G4double FormFactorMev2(G4double Z, G4double A);

  //  hide assignment operator
  G4WentzelVIModel & operator=(const  G4WentzelVIModel &right);
  G4WentzelVIModel(const  G4WentzelVIModel&);

  const G4ParticleDefinition* theProton;
  const G4ParticleDefinition* theElectron;
  const G4ParticleDefinition* thePositron;

  G4ParticleChangeForMSC*   fParticleChange;

  G4SafetyHelper*           safetyHelper;
  G4PhysicsTable*           theLambdaTable;
  G4PhysicsTable*           theLambda2Table;
  G4LossTableManager*       theManager;
  const G4DataVector*       currentCuts;

  G4NistManager*            fNistManager;

  G4double numlimit;
  G4double tlimitminfix;
  G4double invsqrt12;
  G4double lowBinEnergy;
  G4double highBinEnergy;

  // cash
  G4double preKinEnergy;
  G4double xSection;
  G4double ecut;
  G4double lambda0;
  G4double tPathLength;
  G4double zPathLength;
  G4double lambdaeff;
  G4double currentRange; 
  G4double par1;
  G4double par2;
  G4double par3;

  G4double xsece1;
  G4double xsece2;
  G4double xsecn2;
  G4double zcorr;
  G4double xsecn[40];

  G4int    nbins;
  G4int    nwarnings;
  G4int    nwarnlimit;

  G4int    currentMaterialIndex;

  const G4MaterialCutsCouple* currentCouple;
  const G4Material* currentMaterial;

  // single scattering parameters
  G4double coeff;
  G4double constn;
  G4double cosThetaMin;
  G4double cosThetaMax;
  G4double cosThetaLimit;
  G4double cosTetMaxNuc;
  G4double cosTetMaxNuc2;
  G4double cosTetMaxElec;
  G4double cosTetMaxHad;
  G4double q2Limit;
  G4double alpha2;
  G4double a0;

  // projectile
  const G4ParticleDefinition* particle;

  G4double chargeSquare;
  G4double spin;
  G4double mass;
  G4double tkin;
  G4double mom2;
  G4double invbeta2;
  G4double etag;

  // target
  G4double targetZ;
  G4double targetA;
  G4double screenZ;
  G4double formfactA;
  G4double FF[100];

  // flags
  G4bool   isInitialized;
  G4bool   newrun;
  G4bool   inside;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
void G4WentzelVIModel::DefineMaterial(const G4MaterialCutsCouple* cup) 
{ 
  if(cup != currentCouple) {
    currentCouple = cup;
    currentMaterial = cup->GetMaterial();
    currentMaterialIndex = currentCouple->GetIndex(); 
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
G4double G4WentzelVIModel::GetLambda(G4double e)
{
  G4double x;
  if(theLambdaTable) {
    G4bool b;
    x = ((*theLambdaTable)[currentMaterialIndex])->GetValue(e, b);
  } else {
    x = CrossSection(currentCouple,particle,e);
  }
  if(x > DBL_MIN) x = 1./x;
  else            x = DBL_MAX;
  return x;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline 
void G4WentzelVIModel::SetupParticle(const G4ParticleDefinition* p)
{
  // Initialise mass and charge
  if(p != particle) {
    particle = p;
    mass = particle->GetPDGMass();
    spin = particle->GetPDGSpin();
    G4double q = particle->GetPDGCharge()/eplus;
    chargeSquare = q*q;
    tkin = 0.0;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void G4WentzelVIModel::SetupKinematic(G4double ekin, G4double cut)
{
  if(ekin != tkin || ecut != cut) {
    cosTetMaxNuc = cosThetaLimit;
    if(cut > ekin) cosTetMaxNuc = ekin*(1.0 + cosThetaLimit)/cut - 1.0;  
    tkin  = ekin;
    mom2  = tkin*(tkin + 2.0*mass);
    invbeta2 = 1.0 +  mass*mass/mom2;
    ComputeMaxElectronScattering(cut);
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
  
inline void G4WentzelVIModel::SetupTarget(G4double Z, G4double A, G4double e)
{
  if(Z != targetZ || A != targetA || e != etag) {
    etag    = e; 
    targetZ = Z;
    targetA = A;
    G4double x = fNistManager->GetZ13(Z);
    screenZ = a0*x*x*(1.13 + 3.76*invbeta2*Z*Z*chargeSquare*alpha2)/mom2;
    //    screenZ = a0*x*x*(1.13 + 3.76*Z*Z*chargeSquare*alpha2)/mom2;
    formfactA = mom2*FormFactorMev2(Z, A);
  } 
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
  
inline G4double G4WentzelVIModel::FormFactorMev2(G4double Z, G4double A)
{
  // A.V. Butkevich et al., NIM A 488 (2002) 282
  G4int iz = G4int(Z);
  if(iz > 99) iz = 99;
  G4double res = FF[iz];
  if(res == 0.0) { 
    res = constn*std::exp(0.54*fNistManager->GetLOGA(A));
    FF[iz] = res;
  }
  cosTetMaxHad = 1.0 - 0.5*q2Limit/(res*mom2);
  if(particle == theProton && 1.5 > A && cosTetMaxHad < 0.0)cosTetMaxHad = 0.0;
  return res;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

