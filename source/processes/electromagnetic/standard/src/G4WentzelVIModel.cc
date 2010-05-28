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
// $Id: G4WentzelVIModel.cc,v 1.56 2010-05-28 16:00:10 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:   G4WentzelVIModel
//
// Author:      V.Ivanchenko 
//
// Creation date: 09.04.2008 from G4MuMscModel
//
// Modifications:
// 27-05-2010 V.Ivanchenko added G4WentzelOKandVIxSection class to
//              compute cross sections and sample scattering angle
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4WentzelVIModel.hh"
#include "Randomize.hh"
#include "G4ParticleChangeForMSC.hh"
#include "G4PhysicsTableHelper.hh"
#include "G4ElementVector.hh"
#include "G4ProductionCutsTable.hh"
#include "G4LossTableManager.hh"
#include "G4Pow.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;

G4WentzelVIModel::G4WentzelVIModel(const G4String& nam) :
  G4VMscModel(nam),
  theLambdaTable(0),
  numlimit(0.2),
  currentCouple(0),
  cosThetaMin(1.0),
  isInitialized(false),
  inside(false)
{
  invsqrt12 = 1./sqrt(12.);
  tlimitminfix = 1.e-6*mm;
  lowEnergyLimit = 1.0*eV;
  particle = 0;
  nelments = 5;
  xsecn.resize(nelments);
  prob.resize(nelments);
  theManager = G4LossTableManager::Instance();
  fG4pow = G4Pow::GetInstance();
  wokvi = new G4WentzelOKandVIxSection();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4WentzelVIModel::~G4WentzelVIModel()
{
  delete wokvi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4WentzelVIModel::Initialise(const G4ParticleDefinition* p,
				  const G4DataVector& cuts)
{
  // reset parameters
  SetupParticle(p);
  currentRange = 0.0;
  cosThetaMax = cos(PolarAngleLimit());
  wokvi->Initialise(p, cosThetaMax);
  /*
  G4cout << "G4WentzelVIModel: factorA2(GeV^2) = " << factorA2/(GeV*GeV)
         << "  1-cos(ThetaLimit)= " << 1 - cosThetaMax 
	 << G4endl;
  */
  currentCuts = &cuts;

  // set values of some data members
  if(!isInitialized) {
    isInitialized = true;
    fParticleChange = GetParticleChangeForMSC();
    InitialiseSafetyHelper();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4WentzelVIModel::ComputeCrossSectionPerAtom( 
                             const G4ParticleDefinition* p,
			     G4double kinEnergy,
			     G4double Z, G4double,
			     G4double cutEnergy, G4double)
{
  G4double xsec = 0.0;
  if(p != particle) { SetupParticle(p); }
  if(kinEnergy < lowEnergyLimit) { return xsec; }
  DefineMaterial(CurrentCouple());
  cosTetMaxNuc = wokvi->SetupKinematic(kinEnergy, currentMaterial);
  if(cosTetMaxNuc < 1.0) {
    cosTetMaxNuc = wokvi->SetupTarget(G4int(Z), cutEnergy);
    xsec = wokvi->ComputeTransportCrossSectionPerAtom(cosTetMaxNuc);
  /*     
    G4cout << "G4WentzelVIModel::CS: Z= " << G4int(Z) << " e(MeV)= " << kinEnergy 
	   << " 1-cosN= " << 1 - costm << " xsec(bn)= " << xsec/barn
	   << " " << particle->GetParticleName() << G4endl;
  */
  }
  return xsec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4WentzelVIModel::ComputeTruePathLengthLimit(
                             const G4Track& track,
			     G4PhysicsTable* theTable,
			     G4double currentMinimalStep)
{
  G4double tlimit = currentMinimalStep;
  const G4DynamicParticle* dp = track.GetDynamicParticle();
  G4StepPoint* sp = track.GetStep()->GetPreStepPoint();
  G4StepStatus stepStatus = sp->GetStepStatus();
  //G4cout << "G4WentzelVIModel::ComputeTruePathLengthLimit stepStatus= " 
  //	 << stepStatus << G4endl;

  // initialisation for 1st step  
  if(stepStatus == fUndefined) {
    inside = false;
    SetupParticle(dp->GetDefinition());
  }

  // initialisation for each step, lambda may be computed from scratch
  preKinEnergy  = dp->GetKineticEnergy();
  DefineMaterial(track.GetMaterialCutsCouple());
  theLambdaTable = theTable;
  lambdaeff = GetLambda(preKinEnergy);
  currentRange = 
    theManager->GetRangeFromRestricteDEDX(particle,preKinEnergy,currentCouple);

  // extra check for abnormal situation
  // this check needed to run MSC with eIoni and eBrem inactivated
  if(tlimit > currentRange) { tlimit = currentRange; }

  // stop here if small range particle
  if(inside) { return tlimit; }

  // pre step
  G4double presafety = sp->GetSafety();

  // compute presafety again if presafety <= 0 and no boundary
  // i.e. when it is needed for optimization purposes
  if(stepStatus != fGeomBoundary && presafety < tlimitminfix) {
    presafety = ComputeSafety(sp->GetPosition(), tlimit); 
  }
  /*
  G4cout << "e(MeV)= " << preKinEnergy/MeV
	 << "  " << particle->GetParticleName() 
	 << " CurLimit(mm)= " << tlimit/mm <<" safety(mm)= " << presafety/mm
	 << " R(mm)= " <<currentRange/mm
	 << " L0(mm^-1)= " << lambdaeff*mm 
	 <<G4endl;
  */
  // far from geometry boundary
  if(currentRange < presafety) {
    inside = true;  
    return tlimit;
  }

  // natural limit for high energy
  cosTetMaxNuc = wokvi->SetupKinematic(preKinEnergy, currentMaterial);
  G4double rlimit = std::max(facrange*currentRange, 
			     0.7*(1.0 - cosTetMaxNuc)*lambdaeff);

  // low-energy e-
  if(cosThetaMax > cosTetMaxNuc) {
    rlimit = std::min(rlimit, facsafety*presafety);
  }
   
  // cut correction
  G4double rcut = currentCouple->GetProductionCuts()->GetProductionCut(1);
  //G4cout << "rcut= " << rcut << " rlimit= " << rlimit << " presafety= " << presafety 
  // << " 1-cosThetaMax= " <<1-cosThetaMax << " 1-cosTetMaxNuc= " << 1-cosTetMaxNuc
  // << G4endl;
  if(rcut > rlimit) { rlimit = std::min(rlimit, rcut*sqrt(rlimit/rcut)); }
  //    if(rcut > rlimit) { rlimit = rcut*fG4pow->A13(rlimit/rcut); }

  if(rlimit < tlimit) { tlimit = rlimit; }

  tlimit = std::max(tlimit, tlimitminfix);

  // step limit in infinite media
  tlimit = std::min(tlimit, 20*currentMaterial->GetRadlen());
  /*  
  G4cout << particle->GetParticleName() << " e= " << preKinEnergy
	 << " L0= " << lambdaeff << " R= " << currentRange
	 << "tlimit= " << tlimit  
  	 << " currentMinimalStep= " << currentMinimalStep << G4endl;
  */
  return tlimit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4WentzelVIModel::ComputeGeomPathLength(G4double truelength)
{
  tPathLength  = truelength;
  zPathLength  = tPathLength;

  if(lambdaeff > 0.0) {
    G4double tau = tPathLength/lambdaeff;
    //G4cout << "ComputeGeomPathLength: tLength= " << tPathLength
    //	 << " Leff= " << lambdaeff << " tau= " << tau << G4endl; 
    // small step
    if(tau < numlimit) {
      zPathLength *= (1.0 - 0.5*tau + tau*tau/6.0);

      // medium step
    } else {
      G4double e1 = 0.0;
      if(currentRange > tPathLength) {
	e1 = theManager->GetEnergy(particle,
				   currentRange-tPathLength,
				   currentCouple);
      }
      e1 = 0.5*(e1 + preKinEnergy);
      cosTetMaxNuc = wokvi->SetupKinematic(e1, currentMaterial);
      lambdaeff = GetLambda(e1);
      zPathLength = lambdaeff*(1.0 - exp(-tPathLength/lambdaeff));
    }
  }
  //G4cout<<"Comp.geom: zLength= "<<zPathLength<<" tLength= "<<tPathLength<<G4endl;
  return zPathLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4WentzelVIModel::ComputeTrueStepLength(G4double geomStepLength)
{
  // initialisation of single scattering x-section
  xtsec = 0.0;

  // pathalogical case
  if(lambdaeff <= 0.0) { 
    zPathLength = geomStepLength;
    tPathLength = geomStepLength;
    return tPathLength;
  }

  G4double tau = geomStepLength/lambdaeff;

  // step defined other than transportation 
  if(geomStepLength != zPathLength) { 

    // step defined by transportation 
    zPathLength = geomStepLength;
    tPathLength = zPathLength*(1.0 + 0.5*tau + tau*tau/3.0); 

    // energy correction for a big step
    if(tau > numlimit) {
      G4double e1 = 0.0;
      if(currentRange > tPathLength) {
	e1 = theManager->GetEnergy(particle,
				   currentRange-tPathLength,
				   currentCouple);
      }
      e1 = 0.5*(e1 + preKinEnergy);
      cosTetMaxNuc = wokvi->SetupKinematic(e1, currentMaterial);
      lambdaeff = GetLambda(e1);
      tau = zPathLength/lambdaeff;
      
      if(tau < 0.999999) { tPathLength = -lambdaeff*log(1.0 - tau); } 
      else               { tPathLength = currentRange; }
    }
  }

  // check of step length
  // define threshold angle between single and multiple scattering 
  cosThetaMin = 1.0 - 1.5*tPathLength/lambdaeff;

  // recompute transport cross section - do not change energy
  // anymore - cannot be applied for big steps
  if(cosThetaMin > cosTetMaxNuc && tau < numlimit) {
   
    // new computation
    G4double xsec = ComputeXSectionPerVolume();
    //G4cout << "%%%% xsec= " << xsec << "  xtsec= " << xtsec << G4endl;
    if(xtsec > 0.0) {
      if(xsec > 0.0) { lambdaeff = 1./xsec; }
      else           { lambdaeff = DBL_MAX; }

      tau = zPathLength*xsec;
      tPathLength = zPathLength*(1.0 + 0.5*tau + tau*tau/3.0); 
    } 
  } else { cosThetaMin = cosTetMaxNuc; }

  if(tPathLength > currentRange) { tPathLength = currentRange; }
  if(tPathLength < zPathLength)  { tPathLength = zPathLength; }
  /*    
  G4cout <<"Comp.true: zLength= "<<zPathLength<<" tLength= "<<tPathLength
	 <<" Leff(mm)= "<<lambdaeff/mm<<" sig0(1/mm)= " << xtsec <<G4endl;
  G4cout << particle->GetParticleName() << " 1-cosThetaMin= " << 1-cosThetaMin
	 << " 1-cosTetMaxNuc= " << 1-cosTetMaxNuc 
	 << " e(MeV)= " << preKinEnergy/MeV << G4endl;
  */
  return tPathLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4WentzelVIModel::SampleScattering(const G4DynamicParticle* dynParticle,
					G4double safety)
{
  //G4cout << "!##! G4WentzelVIModel::SampleScattering for " 
  //	 << particle->GetParticleName() << G4endl;

  // ignore scattering for zero step length and energy below the limit
  if(dynParticle->GetKineticEnergy() < lowEnergyLimit || 
     tPathLength <= DBL_MIN || lambdaeff <= DBL_MIN) 
    { return; }
  
  G4double invlambda = 0.0;
  if(lambdaeff < DBL_MAX) { invlambda = 0.5/lambdaeff; }

  // use average kinetic energy over the step
  G4double cut = (*currentCuts)[currentMaterialIndex];
  /*     
  G4cout <<"SampleScat: E0(MeV)= "<< preKinEnergy/MeV
	 << " Leff= " << lambdaeff <<" sig0(1/mm)= " << xtsec 
	 << " x1= " <<  tPathLength*invlambda << " safety= " << safety << G4endl;
  */

  G4double length = tPathLength;
  G4double lengthlim = tPathLength*1.e-6;

  // step limit due msc
  G4double x0 = length;
  // large scattering angle case - two step approach
  if(tPathLength*invlambda > 0.5 && length > tlimitminfix) { x0 *= 0.5; } 

  // step limit due single scattering
  G4double x1 = length;
  if(xtsec > 0.0) { x1 = -log(G4UniformRand())/xtsec; }

  const G4ElementVector* theElementVector = 
    currentMaterial->GetElementVector();
  G4int nelm = currentMaterial->GetNumberOfElements();

  // geometry
  G4double sint, cost, phi;
  G4ThreeVector oldDirection = dynParticle->GetMomentumDirection();
  G4ThreeVector temp(0.0,0.0,1.0);

  // current position and direction relative to the end point
  // because of magnetic field geometry is computed relatively to the 
  // end point of the step 
  G4ThreeVector dir(0.0,0.0,1.0);
  G4ThreeVector pos(0.0,0.0,-zPathLength);
  G4double mscfac = zPathLength/tPathLength;

  // start a loop 
  do {
    G4double step = x0; 
    G4bool singleScat = false;

    // single scattering case
    if(x1 < x0) { 
      step = x1;
      singleScat = true;
    }

    // new position
    pos += step*mscfac*dir;

    // added multiple scattering
    G4double z; 
    G4double tet2 = step*invlambda;  
    do { z = -tet2*log(G4UniformRand()); } while (z >= 1.0); 

    cost = 1.0 - 2.0*z;
    sint = sqrt((1.0 - cost)*(1.0 + cost));
    phi  = twopi*G4UniformRand();
    G4double vx1 = sint*cos(phi);
    G4double vy1 = sint*sin(phi);

    // lateral displacement  
    if (latDisplasment && safety > tlimitminfix) {
      G4double rms = invsqrt12*sqrt(2.0*tet2);
      G4double dx = step*(0.5*vx1 + rms*G4RandGauss::shoot(0.0,1.0));
      G4double dy = step*(0.5*vy1 + rms*G4RandGauss::shoot(0.0,1.0));
      G4double dz;
      G4double d = (dx*dx + dy*dy)/(step*step);
      if(d < numlimit)  { dz = -0.5*step*d*(1.0 + 0.25*d); }
      else if(d < 1.0)  { dz = -step*(1.0 - sqrt(1.0 - d));}
      else              { dx = dy = dz = 0.0; }

      // change position
      temp.set(dx,dy,dz);
      temp.rotateUz(dir); 
      pos += temp;
    }

    // direction is changed
    temp.set(vx1,vy1,cost);
    temp.rotateUz(dir);
    dir = temp;

    if(singleScat) {

      // select element
      G4int i = 0;
      if(nelm > 1) {
	G4double qsec = G4UniformRand()*xtsec;
	for (; i<nelm; ++i) { if(xsecn[i] >= qsec) { break; } }
	if(i >= nelm) { i = nelm - 1; }
      }
      G4double cosTetM = 
	wokvi->SetupTarget(G4int((*theElementVector)[i]->GetZ()), cut);
      temp = wokvi->SampleSingleScattering(cosThetaMin, cosTetM, prob[i]);
      temp.rotateUz(dir);

      // renew direction
      dir = temp;

      // new single scatetring
      x1 = -log(G4UniformRand())/xtsec; 
    }

    // update step
    length -= step;

  } while (length > lengthlim);
    
  dir.rotateUz(oldDirection);
  pos.rotateUz(oldDirection);

  //G4cout << "G4WentzelVIModel sampling of scattering is done" << G4endl;
  // end of sampling -------------------------------

  fParticleChange->ProposeMomentumDirection(dir);

  // lateral displacement  
  if (latDisplasment) {
    G4double r = pos.mag();

    /*    
    G4cout << " r(mm)= " << r << " safety= " << safety
           << " trueStep(mm)= " << tPathLength
           << " geomStep(mm)= " << zPathLength
           << G4endl;
    */

    if(r > tlimitminfix) {
      pos /= r;
      ComputeDisplacement(fParticleChange, pos, r, safety);
    }
  }
  //G4cout << "G4WentzelVIModel::SampleScattering end" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4WentzelVIModel::ComputeXSectionPerVolume()
{
  // prepare recomputation of x-sections
  const G4ElementVector* theElementVector = currentMaterial->GetElementVector();
  const G4double* theAtomNumDensityVector = 
    currentMaterial->GetVecNbOfAtomsPerVolume();
  G4int nelm = currentMaterial->GetNumberOfElements();
  if(nelm > nelments) {
    nelments = nelm;
    xsecn.resize(nelm);
    prob.resize(nelm);
  }
  G4double cut = (*currentCuts)[currentMaterialIndex];
  cosTetMaxNuc = wokvi->SetupKinematic(preKinEnergy, currentMaterial);

  // check consistency
  xtsec = 0.0;
  if(cosTetMaxNuc > cosThetaMin) { return 0.0; }

  // loop over elements
  G4double xs = 0.0;
  for (G4int i=0; i<nelm; ++i) {
    G4double costm = 
      wokvi->SetupTarget(G4int((*theElementVector)[i]->GetZ()), cut);
    G4double density = theAtomNumDensityVector[i];

    G4double esec = 0.0;
    if(costm < cosThetaMin) {  

      // recompute the transport x-section
      xs += density*wokvi->ComputeTransportCrossSectionPerAtom(cosThetaMin);

      // recompute the total x-section
      G4double nsec = wokvi->ComputeNuclearCrossSection(cosThetaMin, costm);
      esec = wokvi->ComputeElectronCrossSection(cosThetaMin, costm);
      nsec += esec;
      if(nsec > 0.0) { esec /= nsec; }
      xtsec += nsec*density;
    }
    xsecn[i] = xtsec;
    prob[i]  = esec;
    //G4cout << i << "  xs= " << xs << " xtsec= " << xtsec << " 1-cosThetaMin= " << 1-cosThetaMin 
    //	   << " 1-cosTetMaxNuc2= " <<1-cosTetMaxNuc2<< G4endl;
  }
  
  //G4cout << "ComputeXS result:  xsec(1/mm)= " << xs 
  //	 << " txsec(1/mm)= " << xtsec <<G4endl; 
  return xs;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
