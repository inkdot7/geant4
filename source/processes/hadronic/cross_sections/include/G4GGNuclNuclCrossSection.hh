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
// Calculation of the nucleus-nucleus total, inelastic, production, 
// elastic and quasi-elastic  cross-sections
// based on parametrisations of nucleon-nucleon
// cross-sections  in 
// the framework of simplified Glauber-Gribov approach
//
//
//
//
//
// 24.11.08 V. Grichine - first implementation based on G4GlauberGribovCrossSection
//
//

#ifndef G4GGNuclNuclCrossSection_h
#define G4GGNuclNuclCrossSection_h

#include "globals.hh"
#include "G4Proton.hh"
#include "G4Nucleus.hh"

#include "G4VCrossSectionDataSet.hh"

class G4ParticleDefinition;

class G4GGNuclNuclCrossSection : public G4VCrossSectionDataSet
{
public:

  G4GGNuclNuclCrossSection ();
  virtual ~G4GGNuclNuclCrossSection ();
   
  virtual
  G4bool IsApplicable(const G4DynamicParticle* aDP, const G4Element*);

  virtual
  G4bool IsZAApplicable(const G4DynamicParticle* aDP, G4double Z, G4double A);

  virtual
  G4double GetCrossSection(const G4DynamicParticle*, 
			   const G4Element*, 
			   G4double aTemperature = 0.0);

  virtual
  G4double GetIsoZACrossSection(const G4DynamicParticle*, 
				G4double Z, G4double A, 
				G4double aTemperature = 0.0);

  G4double GetCoulombBarier(const G4DynamicParticle*, 
				G4double Z, G4double A, G4double pR, G4double tR);

  virtual
  void BuildPhysicsTable(const G4ParticleDefinition&)
  {}

  virtual
  void DumpPhysicsTable(const G4ParticleDefinition&) 
  {G4cout << "G4NuclNuclCrossSection: uses Glauber-Gribov formula"<<G4endl;}

  G4double GetRatioSD(const G4DynamicParticle*, G4double At, G4double Zt);
  G4double GetRatioQE(const G4DynamicParticle*, G4double At, G4double Zt);

  G4double GetHadronNucleonXsc(const G4DynamicParticle*, const G4Element*);
  G4double GetHadronNucleonXsc(const G4DynamicParticle*, G4double At, G4double Zt);

  G4double GetHadronNucleonXscPDG(const G4DynamicParticle*, const G4Element*);
  G4double GetHadronNucleonXscPDG(const G4DynamicParticle*, G4double At, G4double Zt);

  // G4double GetHadronNucleonXscNS(const G4DynamicParticle*, const G4Element*);
  // G4double GetHadronNucleonXscNS(const G4DynamicParticle*,G4double At, G4double Zt);

  G4double GetHadronNucleonXscNS(G4ParticleDefinition*,G4double pTkin, G4ParticleDefinition*);

  // G4double GetHNinelasticXsc(const G4DynamicParticle*, const G4Element*);
  // G4double GetHNinelasticXsc(const G4DynamicParticle*, G4double At, G4double Zt);

  G4double GetHNinelasticXscVU(const G4DynamicParticle*, G4double At, G4double Zt);

  G4double CalculateEcmValue ( const G4double , const G4double , const G4double ); 

  G4double CalcMandelstamS( const G4double , const G4double , const G4double );

  G4double GetElasticGlauberGribov(const G4DynamicParticle*,G4double Z, G4double A);
  G4double GetInelasticGlauberGribov(const G4DynamicParticle*,G4double Z, G4double A);

  G4double GetTotalGlauberGribovXsc()    { return fTotalXsc;     }; 
  G4double GetElasticGlauberGribovXsc()  { return fElasticXsc;   }; 
  G4double GetInelasticGlauberGribovXsc(){ return fInelasticXsc; }; 
  G4double GetProductionGlauberGribovXsc(){ return fProductionXsc; }; 
  G4double GetDiffractionGlauberGribovXsc(){ return fDiffractionXsc; }; 
  G4double GetRadiusConst()              { return fRadiusConst;  }; 

  G4double GetNucleusRadius(const G4DynamicParticle*, const G4Element*);


  G4double GetNucleusRadius(G4double At);
  G4double GetNucleusRadiusGG(G4double At);
  G4double GetNucleusRadiusDE(G4double At);


  inline void SetEnergyLowerLimit(G4double E ){fLowerLimit=E;};

private:

  const G4double fUpperLimit;
  G4double fLowerLimit; 
  const G4double fRadiusConst;
 
  G4double fTotalXsc, fElasticXsc, fInelasticXsc, fProductionXsc, fDiffractionXsc;
  G4double fHadronNucleonXsc;
 
  G4ParticleDefinition* theProton;
  G4ParticleDefinition* theNeutron;

};

////////////////////////////////////////////////////////////////
//
// Inlines

inline
G4double G4GGNuclNuclCrossSection::GetElasticGlauberGribov(
	 const G4DynamicParticle* dp, G4double Z, G4double A)
{
  GetIsoZACrossSection(dp, Z, A);
  return fElasticXsc;
}

/////////////////////////////////////////////////////////////////

inline
G4double G4GGNuclNuclCrossSection::GetInelasticGlauberGribov(
         const G4DynamicParticle* dp, G4double Z, G4double A)
{
  GetIsoZACrossSection(dp, Z, A);
  return fInelasticXsc;
}



#endif
