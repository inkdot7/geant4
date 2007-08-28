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
// $Id: G4QLowEnergy.cc,v 1.1 2007-08-28 15:48:15 mkossov Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//      ---------------- G4QLowEnergy class -----------------
//                 by Mikhail Kossov, Aug 2007.
// G4QLowEnergy class of the CHIPS Simulation Branch in GEANT4
// ---------------------------------------------------------------
// ****************************************************************************************
// ********** This CLASS is temporary moved from the "chips/interface" directory *********
// ****************************************************************************************

//#define debug
//#define pdebug
//#define tdebug
//#define nandebug
//#define ppdebug

#include "G4QLowEnergy.hh"

// Initialization of static vectors
G4int    G4QLowEnergy::nPartCWorld=152;  // #of particles initialized in CHIPS
std::vector<G4int> G4QLowEnergy::ElementZ; // Z of element(i) in theLastCalc
std::vector<G4double> G4QLowEnergy::ElProbInMat; // SumProbOfElem in Material
std::vector<std::vector<G4int>*> G4QLowEnergy::ElIsoN;// N of isotope(j), E(i)
std::vector<std::vector<G4double>*>G4QLowEnergy::IsoProbInEl;//SumProbIsotE(i)

// Constructor
G4QLowEnergy::G4QLowEnergy(const G4String& processName):G4VDiscreteProcess(processName)
{
#ifdef debug
  G4cout<<"G4QLowEnergy::Constructor is called processName="<<processName<<G4endl;
#endif
  if (verboseLevel>0) G4cout<<GetProcessName()<<" process is created "<<G4endl;

  G4QCHIPSWorld::Get()->GetParticles(nPartCWorld); // Create CHIPS World (234 part. max)
}

// Destructor
G4QLowEnergy::~G4QLowEnergy() {}


G4LorentzVector G4QLowEnergy::GetEnegryMomentumConservation(){return EnMomConservation;}

G4int G4QLowEnergy::GetNumberOfNeutronsInTarget() {return nOfNeutrons;}

// output of the function must be in units of length! L=1/sig_V,sig_V=SUM(n(j,i)*sig(j,i)),
// where n(i,j) is a number of nuclei of the isotop j of the element i in V=1(lengtUnit^3)
// ********** All CHIPS cross sections are calculated in the surface units ************
G4double G4QLowEnergy::GetMeanFreePath(const G4Track&Track, G4double Q, G4ForceCondition*F)
{
  *F = NotForced;
  const G4DynamicParticle* incidentParticle = Track.GetDynamicParticle();
  G4ParticleDefinition* incidentParticleDefinition=incidentParticle->GetDefinition();
  if( !IsApplicable(*incidentParticleDefinition))
    G4cout<<"-Warning-G4QLowEnergy::GetMeanFreePath for notImplemented Particle"<<G4endl;
  // Calculate the mean Cross Section for the set of Elements(*Isotopes) in the Material
  G4double Momentum = incidentParticle->GetTotalMomentum(); // 3-momentum of the Particle
#ifdef debug
  G4double KinEn = incidentParticle->GetKineticEnergy();
  G4cout<<"G4QLowEnergy::GetMeanFreePath:Prpj, kinE="<<KinEn<<", Mom="<<Momentum<<G4endl;
#endif
  const G4Material* material = Track.GetMaterial();        // Get the current material
  const G4double* NOfNucPerVolume = material->GetVecNbOfAtomsPerVolume();
  const G4ElementVector* theElementVector = material->GetElementVector();
  G4int nE=material->GetNumberOfElements();
#ifdef debug
  G4cout<<"G4QLowEnergy::GetMeanFreePath:"<<nE<<" Elems"<<G4endl;
#endif
  G4int pPDG=0;
  // @@ At present it is made only for n & p, but can be extended if inXS are available
  if     (incidentParticleDefinition == G4Proton::Proton()  ) pPDG=2212;
  else if(incidentParticleDefinition == G4Neutron::Neutron()) pPDG=2112;
  else G4cout<<"G4QLowEnergy::GetMeanFreePath: only nA & pA are implemented"<<G4endl;
  
  G4QIsotope* Isotopes = G4QIsotope::Get(); // Pointer to the G4QIsotopes singleton
  G4double sigma=0.;                        // Sums over elements for the material
  G4int IPIE=IsoProbInEl.size();            // How many old elements?
  if(IPIE) for(G4int ip=0; ip<IPIE; ++ip)   // Clean up the SumProb's of Isotopes (SPI)
  {
    std::vector<G4double>* SPI=IsoProbInEl[ip]; // Pointer to the SPI vector
    SPI->clear();
    delete SPI;
    std::vector<G4int>* IsN=ElIsoN[ip];     // Pointer to the N vector
    IsN->clear();
    delete IsN;
  }
  ElProbInMat.clear();                      // Clean up the SumProb's of Elements (SPE)
  ElementZ.clear();                         // Clear the body vector for Z of Elements
  IsoProbInEl.clear();                      // Clear the body vector for SPI
  ElIsoN.clear();                           // Clear the body vector for N of Isotopes
  for(G4int i=0; i<nE; ++i)
  {
    G4Element* pElement=(*theElementVector)[i]; // Pointer to the current element
    G4int Z = static_cast<G4int>(pElement->GetZ()); // Z of the Element
    ElementZ.push_back(Z);                  // Remember Z of the Element
    G4int isoSize=0;                        // The default for the isoVectorLength is 0
    G4int indEl=0;                          // Index of non-natural element or 0(default)
    G4IsotopeVector* isoVector=pElement->GetIsotopeVector(); // Get the predefined IsoVect
    if(isoVector) isoSize=isoVector->size();// Get size of the existing isotopeVector
#ifdef debug
    G4cout<<"G4QLowEnergy::GetMeanFreePath: isovector Length="<<isoSize<<G4endl;
#endif
    if(isoSize)                             // The Element has non-trivial abundance set
    {
      indEl=pElement->GetIndex()+1;         // Index of the non-trivial element is an order
#ifdef debug
      G4cout<<"G4QLowEn::GetMFP:iE="<<indEl<<",def="<<Isotopes->IsDefined(Z,indEl)<<G4endl;
#endif
      if(!Isotopes->IsDefined(Z,indEl))     // This index is not defined for this Z: define
      {
        std::vector<std::pair<G4int,G4double>*>* newAbund =
                                               new std::vector<std::pair<G4int,G4double>*>;
        G4double* abuVector=pElement->GetRelativeAbundanceVector();
        for(G4int j=0; j<isoSize; j++)      // Calculation of abundance vector for isotopes
        {
          G4int N=pElement->GetIsotope(j)->GetN()-Z; // N means A=N+Z !
          if(pElement->GetIsotope(j)->GetZ()!=Z)G4cerr<<"G4QDiffract::GetMeanFreePath: Z="
                                         <<pElement->GetIsotope(j)->GetZ()<<"#"<<Z<<G4endl;
          G4double abund=abuVector[j];
								  std::pair<G4int,G4double>* pr= new std::pair<G4int,G4double>(N,abund);
#ifdef debug
          G4cout<<"G4QLowEnergy::GetMeanFreePath:pair#"<<j<<",N="<<N<<",a="<<abund<<G4endl;
#endif
          newAbund->push_back(pr);
						  }
#ifdef debug
        G4cout<<"G4QLowEnergy::GetMeanFreePath: pairVectLength="<<newAbund->size()<<G4endl;
#endif
        indEl=G4QIsotope::Get()->InitElement(Z,indEl,newAbund); // definition of the newInd
        for(G4int k=0; k<isoSize; k++) delete (*newAbund)[k];   // Cleaning temporary
        delete newAbund; // Was "new" in the beginning of the name space
      }
    }
    std::vector<std::pair<G4int,G4double>*>* cs= Isotopes->GetCSVector(Z,indEl);//CSPointer
    std::vector<G4double>* SPI = new std::vector<G4double>; // Pointer to the SPI vector
    IsoProbInEl.push_back(SPI);
    std::vector<G4int>* IsN = new std::vector<G4int>; // Pointer to the N vector
    ElIsoN.push_back(IsN);
    G4int nIs=cs->size();                   // A#Of Isotopes in the Element
#ifdef debug
    G4cout<<"G4QLowEnergy::GetMFP:***=>,#isot="<<nIs<<", Z="<<Z<<", indEl="<<indEl<<G4endl;
#endif
    G4double susi=0.;                       // sum of CS over isotopes
    if(nIs) for(G4int j=0; j<nIs; j++)      // Calculate CS for eachIsotope of El
    {
      std::pair<G4int,G4double>* curIs=(*cs)[j]; // A pointer, which is used twice
      G4int N=curIs->first;                 // #of Neuterons in the isotope j of El i
      IsN->push_back(N);                    // Remember Min N for the Element
#ifdef debug
      G4cout<<"G4QLowE::GMFP:true,P="<<Momentum<<",Z="<<Z<<",N="<<N<<",PDG="<<pPDG<<G4endl;
#endif
		    G4bool ccsf=true;
      if(Q==-27.) ccsf=false;
#ifdef debug
      G4cout<<"G4QLowEnergy::GMFP: GetCS #1 j="<<j<<G4endl;
#endif
      G4double CSI=CalculateXS(Momentum, Z, N, pPDG); // XS(j,i) for theIsotope

#ifdef debug
      G4cout<<"G4QLowEnergy::GetMeanFreePath: jI="<<j<<", Zt="<<Z<<", Nt="<<N<<", Mom="
            <<Momentum<<", XSec="<<CSI/millibarn<<G4endl;
#endif
      curIs->second = CSI;
      susi+=CSI;                            // Make a sum per isotopes
      SPI->push_back(susi);                 // Remember summed cross-section
    } // End of temporary initialization of the cross sections in the G4QIsotope singeltone
    sigma+=Isotopes->GetMeanCrossSection(Z,indEl)*NOfNucPerVolume[i];//SUM(MeanCS*NOfNperV)
#ifdef debug
    G4cout<<"G4QLowEnergy::GetMeanFreePath:<XS>="<<Isotopes->GetMeanCrossSection(Z,indEl)
          <<",AddSigm="<<Isotopes->GetMeanCrossSection(Z,indEl)*NOfNucPerVolume[i]<<G4endl;
#endif
    ElProbInMat.push_back(sigma);
  } // End of LOOP over Elements
  // Check that cross section is not zero and return the mean free path
#ifdef debug
  G4cout<<"G4QLowEnergy::GetMeanFreePath: MeanFreePath="<<1./sigma<<G4endl;
#endif
  if(sigma > 0.) return 1./sigma;                 // Mean path [distance] 
  return DBL_MAX;
}

G4bool G4QLowEnergy::IsApplicable(const G4ParticleDefinition& particle) 
{
  if      (particle == *(        G4Proton::Proton()        )) return true;
  else if (particle == *(       G4Neutron::Neutron()       )) return true;
  //else if (particle == *(     G4MuonMinus::MuonMinus()     )) return true; 
  //else if (particle == *(       G4TauPlus::TauPlus()       )) return true;
  //else if (particle == *(      G4TauMinus::TauMinus()      )) return true;
  //else if (particle == *(      G4Electron::Electron()      )) return true;
  //else if (particle == *(      G4Positron::Positron()      )) return true;
  //else if (particle == *(         G4Gamma::Gamma()         )) return true;
  //else if (particle == *(      G4MuonPlus::MuonPlus()      )) return true;
  //else if (particle == *(G4AntiNeutrinoMu::AntiNeutrinoMu())) return true;
  //else if (particle == *(    G4NeutrinoMu::NeutrinoMu()    )) return true;
  //else if (particle == *(     G4PionMinus::PionMinus()     )) return true;
  //else if (particle == *(      G4PionPlus::PionPlus()      )) return true;
  //else if (particle == *(      G4KaonPlus::KaonPlus()      )) return true;
  //else if (particle == *(     G4KaonMinus::KaonMinus()     )) return true;
  //else if (particle == *(  G4KaonZeroLong::KaonZeroLong()  )) return true;
  //else if (particle == *( G4KaonZeroShort::KaonZeroShort() )) return true;
  //else if (particle == *(        G4Lambda::Lambda()        )) return true;
  //else if (particle == *(     G4SigmaPlus::SigmaPlus()     )) return true;
  //else if (particle == *(    G4SigmaMinus::SigmaMinus()    )) return true;
  //else if (particle == *(     G4SigmaZero::SigmaZero()     )) return true;
  //else if (particle == *(       G4XiMinus::XiMinus()       )) return true;
  //else if (particle == *(        G4XiZero::XiZero()        )) return true;
  //else if (particle == *(    G4OmegaMinus::OmegaMinus()    )) return true;
  //else if (particle == *(   G4AntiNeutron::AntiNeutron()   )) return true;
  //else if (particle == *(    G4AntiProton::AntiProton()    )) return true;
#ifdef debug
  G4cout<<"***>>G4QLowEnergy::IsApplicable: projPDG="<<particle.GetPDGEncoding()<<G4endl;
#endif
  return false;
}

G4VParticleChange* G4QLowEnergy::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  static const G4double mProt= G4QPDGCode(2212).GetMass()/MeV; // CHIPS proton Mass in MeV
  static const G4double mNeut= G4QPDGCode(2112).GetMass()/MeV; // CHIPS neutron Mass in MeV
  static const G4double mLamb= G4QPDGCode(3122).GetMass()/MeV; // CHIPS Lambda Mass in MeV
  static const G4double mDeut= G4QPDGCode(2112).GetNuclMass(1,1,0)/MeV;
  static const G4double mTrit= G4QPDGCode(2112).GetNuclMass(1,2,0)/MeV;
  static const G4double mHel3= G4QPDGCode(2112).GetNuclMass(2,1,0)/MeV;
  static const G4double mAlph= G4QPDGCode(2112).GetNuclMass(2,2,0)/MeV;
  static const G4ThreeVector zeroMom(0.,0.,0.);
  static G4ParticleDefinition* aGamma    = G4Gamma::Gamma();
  static G4ParticleDefinition* aProton   = G4Proton::Proton();
  static G4ParticleDefinition* aNeutron  = G4Neutron::Neutron();
  static G4ParticleDefinition* aLambda   = G4Lambda::Lambda();
  static G4ParticleDefinition* aDeuteron = G4Deuteron::Deuteron();
  static G4ParticleDefinition* aTriton   = G4Triton::Triton();
  static G4ParticleDefinition* aHe3      = G4He3::He3();
  static G4ParticleDefinition* anAlpha   = G4Alpha::Alpha();
  static const G4int nCh=26;                         // #of combinations
  //
  //-------------------------------------------------------------------------------------
  static G4bool CWinit = true;                       // CHIPS Warld needs to be initted
  if(CWinit)
		{
    CWinit=false;
    G4QCHIPSWorld::Get()->GetParticles(nPartCWorld); // Create CHIPS World (234 part.max)
  }
  //-------------------------------------------------------------------------------------
  const G4DynamicParticle* projHadron = track.GetDynamicParticle();
  const G4ParticleDefinition* particle=projHadron->GetDefinition();
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: Before the GetMeanFreePath is called In4M="
        <<projHadron->Get4Momentum()<<" of PDG="<<particle->GetPDGEncoding()<<", Type="
        <<particle->GetParticleType()<<",SubType="<<particle->GetParticleSubType()<<G4endl;
#endif
  G4ForceCondition cond=NotForced;
  GetMeanFreePath(track, -27., &cond);                  // @@ ?? jus to update parameters?
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: After GetMeanFreePath is called"<<G4endl;
#endif
  G4LorentzVector proj4M=(projHadron->Get4Momentum())/MeV; // Convert to MeV!
  G4double momentum = projHadron->GetTotalMomentum()/MeV; // 3-momentum of the Proj in MeV
  G4double Momentum = proj4M.rho();                   // @@ Just for the test purposes
  if(std::fabs(Momentum-momentum)>.000001)
    G4cerr<<"-Warning-G4QLowEnergy::PostStepDoIt:P_IU="<<Momentum<<"#"<<momentum<<G4endl;
#ifdef pdebug
  G4cout<<"G4QLowEnergy::PostStepDoIt: pP(IU)="<<Momentum<<"="<<momentum<<",proj4M/m="
        <<proj4M<<proj4M.m()<<G4endl;
#endif
  if (!IsApplicable(*particle))  // Check applicability
  {
    G4cerr<<"G4QLowEnergy::PostStepDoIt: Only NA is implemented."<<G4endl;
    return 0;
  }
  const G4Material* material = track.GetMaterial();      // Get the current material
  const G4ElementVector* theElementVector = material->GetElementVector();
  G4int nE=material->GetNumberOfElements();
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: "<<nE<<" elements in the material."<<G4endl;
#endif
  G4int projPDG=0;                           // PDG Code prototype for the captured hadron
  // Not all these particles are implemented yet (see Is Applicable)
  if      (particle ==          G4Proton::Proton()         ) projPDG= 2212;
  else if (particle ==         G4Neutron::Neutron()        ) projPDG= 2112;
  //else if (particle ==       G4PionMinus::PionMinus()      ) projPDG= -211;
  //else if (particle ==        G4PionPlus::PionPlus()       ) projPDG=  211;
  //else if (particle ==        G4KaonPlus::KaonPlus()       ) projPDG=  321;
  //else if (particle ==       G4KaonMinus::KaonMinus()      ) projPDG= -321;
  //else if (particle ==    G4KaonZeroLong::KaonZeroLong()   ) projPDG=  130;
  //else if (particle ==   G4KaonZeroShort::KaonZeroShort()  ) projPDG=  310;
  //else if (particle ==        G4MuonPlus::MuonPlus()       ) projPDG=  -13;
  //else if (particle ==       G4MuonMinus::MuonMinus()      ) projPDG=   13;
  //else if (particle ==      G4NeutrinoMu::NeutrinoMu()     ) projPDG=   14;
  //else if (particle ==  G4AntiNeutrinoMu::AntiNeutrinoMu() ) projPDG=  -14;
  //else if (particle ==        G4Electron::Electron()       ) projPDG=   11;
  //else if (particle ==        G4Positron::Positron()       ) projPDG=  -11;
  //else if (particle ==       G4NeutrinoE::NeutrinoE()      ) projPDG=   12;
  //else if (particle ==   G4AntiNeutrinoE::AntiNeutrinoE()  ) projPDG=  -12;
  //else if (particle ==           G4Gamma::Gamma()          ) projPDG=   22;
  //else if (particle ==         G4TauPlus::TauPlus()        ) projPDG=  -15;
  //else if (particle ==        G4TauMinus::TauMinus()       ) projPDG=   15;
  //else if (particle ==     G4NeutrinoTau::NeutrinoTau()    ) projPDG=   16;
  //else if (particle == G4AntiNeutrinoTau::AntiNeutrinoTau()) projPDG=  -16;
  //else if (particle ==          G4Lambda::Lambda()         ) projPDG= 3122;
  //else if (particle ==       G4SigmaPlus::SigmaPlus()      ) projPDG= 3222;
  //else if (particle ==      G4SigmaMinus::SigmaMinus()     ) projPDG= 3112;
  //else if (particle ==       G4SigmaZero::SigmaZero()      ) projPDG= 3212;
  //else if (particle ==         G4XiMinus::XiMinus()        ) projPDG= 3312;
  //else if (particle ==          G4XiZero::XiZero()         ) projPDG= 3322;
  //else if (particle ==      G4OmegaMinus::OmegaMinus()     ) projPDG= 3334;
  //else if (particle ==     G4AntiNeutron::AntiNeutron()    ) projPDG=-2112;
  //else if (particle ==      G4AntiProton::AntiProton()     ) projPDG=-2212;
#ifdef debug
  G4int prPDG=particle->GetPDGEncoding();
		G4cout<<"G4QLowEnergy::PostStepDoIt: projPDG="<<projPDG<<", stPDG="<<prPDG<<G4endl;
#endif
  if(!projPDG)
  {
    G4cerr<<"-Warning-G4QLowEnergy::PostStepDoIt:UndefProjHadron(PDG=0) ->ret 0"<<G4endl;
    return 0;
  }
		// Element treatment
  G4int EPIM=ElProbInMat.size();
#ifdef debug
		G4cout<<"G4QLowEn::PostStDoIt: m="<<EPIM<<", n="<<nE<<",T="<<ElProbInMat[EPIM-1]<<G4endl;
#endif
  G4int i=0;
  if(EPIM>1)
  {
    G4double rnd = ElProbInMat[EPIM-1]*G4UniformRand();
    for(i=0; i<nE; ++i)
		  {
#ifdef debug
				  G4cout<<"G4QLowEn::PostStepDoIt: EPM["<<i<<"]="<<ElProbInMat[i]<<", r="<<rnd<<G4endl;
#endif
      if (rnd<ElProbInMat[i]) break;
    }
    if(i>=nE) i=nE-1;                        // Top limit for the Element
  }
  G4Element* pElement=(*theElementVector)[i];
  G4int tZ=static_cast<G4int>(pElement->GetZ());
#ifdef debug
				G4cout<<"G4QLowEnergy::PostStepDoIt: i="<<i<<", Z(element)="<<tZ<<G4endl;
#endif
  if(tZ<=0)
  {
    G4cerr<<"-Warning-G4QLowEnergy::PostStepDoIt: Element with Z="<<tZ<<G4endl;
    if(tZ<0) return 0;
  }
  std::vector<G4double>* SPI = IsoProbInEl[i];// Vector of summedProbabilities for isotopes
  std::vector<G4int>* IsN = ElIsoN[i];     // Vector of "#of neutrons" in the isotope El[i]
  G4int nofIsot=SPI->size();               // #of isotopes in the element i
#ifdef debug
		G4cout<<"G4QLowEnergy::PostStepDoIt: nI="<<nofIsot<<", T="<<(*SPI)[nofIsot-1]<<G4endl;
#endif
  G4int j=0;
  if(nofIsot>1)
  {
    G4double rndI=(*SPI)[nofIsot-1]*G4UniformRand(); // Randomize the isotop of the Element
    for(j=0; j<nofIsot; ++j)
    {
#ifdef debug
				  G4cout<<"G4QLowEnergy::PostStepDoIt: SP["<<j<<"]="<<(*SPI)[j]<<",r="<<rndI<<G4endl;
#endif
      if(rndI < (*SPI)[j]) break;
    }
    if(j>=nofIsot) j=nofIsot-1;            // Top limit for the isotope
  }
  G4int tN =(*IsN)[j]; ;                    // Randomized number of neutrons
#ifdef debug
		G4cout<<"G4QLowEnergy::PostStepDoIt: j="<<i<<", N(isotope)="<<tN<<", MeV="<<MeV<<G4endl;
#endif
  if(tN<0)
  {
    G4cerr<<"-Warning-G4QLowEnergy::PostStepDoIt: Isotope Z="<<tZ<<" has 0>N="<<tN<<G4endl;
    return 0;
  }
  nOfNeutrons=tN;                           // Remember it for the energy-momentum check
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: N="<<tN<<" for element with Z="<<tZ<<G4endl;
#endif
  if(tN<0)
  {
    G4cerr<<"*Warning*G4QLowEnergy::PostStepDoIt:Element with N="<<tN<< G4endl;
    return 0;
  }
  aParticleChange.Initialize(track);
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: track is initialized"<<G4endl;
#endif
  G4double localtime = track.GetGlobalTime();
  G4ThreeVector position = track.GetPosition();
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: before Touchable extraction"<<G4endl;
#endif
  G4TouchableHandle trTouchable = track.GetTouchableHandle();
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: Touchable is extracted"<<G4endl;
#endif
  G4QPDGCode targQPDG(90000000+tZ*1000+tN);  // @@ use G4Ion and get rid of CHIPS World
  G4double tM=targQPDG.GetMass();            // CHIPS target nucleus mass in MeV
  G4int pL=particle->GetQuarkContent(3)-particle->GetAntiQuarkContent(3); // Strangeness
  G4int pZ=static_cast<G4int>(particle->GetPDGCharge());  // Charge of the projectile
  G4int pN=particle->GetBaryonNumber()-pZ-pL;// #of neutrons in projectile
  G4double pM=targQPDG.GetNuclMass(pZ,pN,0); // CHIPS projectile nucleus mass in MeV
  G4double cosp=-14*Momentum*(tM-pM)/tM/pM;  // Asymmetry power for angular distribution
#ifdef debug
  G4cout<<"G4QLowEnergy::PoStDoIt: Proj("<<pZ<<","<<pN<<","<<pL<<")p="<<pM<<",Targ=("<<tZ
        <<","<<tN<<"), cosp="<<cosp<<G4endl;
#endif
  G4double kinEnergy= projHadron->GetKineticEnergy()*MeV; // Kin energy in MeV (Is *MeV n?)
  G4ParticleMomentum dir = projHadron->GetMomentumDirection();// It is a unit three-vector
  G4LorentzVector tot4M=proj4M+G4LorentzVector(0.,0.,0.,tM); // Total 4-mom of the reaction
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt: tM="<<tM<<",p4M="<<proj4M<<",t4M="<<tot4M<<G4endl;
#endif
  EnMomConservation=tot4M;                 // Total 4-mom of reaction for E/M conservation
  // @@ Probably this is not necessary any more
#ifdef debug
  G4cout<<"G4QLE::PSDI:false,P="<<Momentum<<",Z="<<pZ<<",N="<<pN<<",PDG="<<projPDG<<G4endl;
#endif
  G4double xSec=CalculateXS(Momentum, tZ, tN, projPDG); // Recalculate CrossSection
#ifdef debug
  G4cout<<"G4QLowEn::PSDI:PDG="<<projPDG<<",P="<<Momentum<<",XS="<<xSec/millibarn<<G4endl;
#endif
#ifdef nandebug
  if(xSec>0. || xSec<0. || xSec==0);
  else  G4cout<<"-Warning-G4QLowEnergy::PostSDI: *NAN* xSec="<<xSec/millibarn<<G4endl;
#endif
  // @@ check a possibility to separate p, n, or alpha (!)
  if(xSec <= 0.) // The cross-section iz 0 -> Do Nothing
  {
#ifdef pdebug
    G4cerr<<"-Warning-G4QLowEnergy::PSDoIt:*Zero cross-section* PDG="<<projPDG
          <<",Z="<<tZ<<",tN="<<tN<<",P="<<Momentum<<G4endl;
#endif
    //Do Nothing Action insead of the reaction
    aParticleChange.ProposeEnergy(kinEnergy);
    aParticleChange.ProposeLocalEnergyDeposit(0.);
    aParticleChange.ProposeMomentumDirection(dir) ;
    return G4VDiscreteProcess::PostStepDoIt(track,step);
  }
  // Kill interacting hadron
  aParticleChange.ProposeEnergy(0.);
  aParticleChange.ProposeTrackStatus(fStopAndKill);
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: Projectile track is killed"<<G4endl;
#endif
  // CASP algorithm implementation --- STARTS HERE --- All calculations are in IU --------
  G4double totM=tot4M.m(); // total CMS mass of the reaction
		G4int totN=tN+pN;
  G4int totZ=tZ+pZ;
  // @@ Here mass[i] can be calculated if mass=0
  G4double mass[nCh]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
                      0.,0.,0.,0.,0.,0.};
  mass[0] = targQPDG.GetNuclMass(totZ,totN,0); // gamma+gamma
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: Nuclear Mass="<<mass[0]<<G4endl;
#endif
  if (totN>0 && totZ>0)
  {
    mass[1] = targQPDG.GetNuclMass(totZ,totN-1,0); // gamma+neutron
    mass[2] = targQPDG.GetNuclMass(totZ-1,totN,0); // gamma+proton
  }
  if (totZ>0&&totN>1||totN>0&&totZ>1) mass[3] = targQPDG.GetNuclMass(totZ-1,totN-1,0);//g+d
  if (totZ>0&&totN>2||totN>1&&totZ>1) mass[4] = targQPDG.GetNuclMass(totZ-1,totN-2,0);//g+t
  if (totZ>2&&totN>0||totN>1&&totZ>1) mass[5] = targQPDG.GetNuclMass(totZ-2,totN-1,0);//g+3
  if (totZ>2&&totN>1||totN>2&&totZ>1) mass[6] = targQPDG.GetNuclMass(totZ-2,totN-2,0);//g+a
  if (totZ>0&&totN>1||totN>2) mass[7] = targQPDG.GetNuclMass(totZ,totN-2,0); // n+n
  mass[ 8] = mass[3]; // neutron+proton (the same as a deuteron)
  if (totZ>1&&totN>0||totZ>2) mass[9] = targQPDG.GetNuclMass(totZ-2,totN,0); // p+p
  mass[10] = mass[5]; // proton+deuteron (the same as He3)
  mass[11] = mass[4]; // neutron+deuteron (the same as t)
  mass[12] = mass[6]; // deuteron+deuteron (the same as alpha)
  mass[13] = mass[6]; // proton+tritium (the same as alpha)
  if (totN>3||totN>2&&totZ>0) mass[14] = targQPDG.GetNuclMass(totZ-1,totN-3,0); // n+t
  if (totZ>3||totZ>2&&totN>0) mass[15] = targQPDG.GetNuclMass(totZ-3,totN-1,0); // He3+p
  mass[16] = mass[6]; // neutron+He3 (the same as alpha)
  if (totZ>3&&totN>1||totZ>2&&totN>2) mass[17] = targQPDG.GetNuclMass(totZ-3,totN-2,0);//pa
  if (totZ>1&&totN>3||totZ>2&&totN>2) mass[18] = targQPDG.GetNuclMass(totZ-2,totN-3,0);//na
  if(pL>0)
  {
    G4int pL1=pL-1;
    if(totN>0||totZ>0) mass[19] = targQPDG.GetNuclMass(totZ  ,totN  ,pL1);// Lambda+gamma
    if(totN>0&&totZ>0||        totZ>1)mass[20]=targQPDG.GetNuclMass(totZ-1,totN  ,pL1);//Lp
    if(totN>0&&totZ>0||totN>0        )mass[21]=targQPDG.GetNuclMass(totZ  ,totN-1,pL1);//Ln
    if(totN>1&&totZ>0||totN>0&&totZ>1)mass[22]=targQPDG.GetNuclMass(totZ-1,totN-1,pL1);//Ld
    if(totN>2&&totZ>0||totN>1&&totZ>1)mass[23]=targQPDG.GetNuclMass(totZ-1,totN-2,pL1);//Lt
    if(totN>0&&totZ>2||totN>1&&totZ>1)mass[24]=targQPDG.GetNuclMass(totZ-2,totN-1,pL1);//L3
    if(totN>1&&totZ>2||totN>2&&totZ>1)mass[25]=targQPDG.GetNuclMass(totZ-2,totN-2,pL1);//La
  }
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: Residual masses are calculated"<<G4endl;
#endif
  G4double tA=tZ+tN; 
  G4double pA=pZ+pN; 
  G4double prZ=pZ/pA+tZ/tA;
  G4double prN=pN/pA+tN/tA;
  G4double prD=prN*prZ;
  G4double prA=prD*prD;
  G4double prH=prD*prZ;
  G4double prT=prD*prN;
  G4double fhe3=6.*pow(tA,.33333333);
  G4double prL=0.;
  if(pL>0) prL=pL/pA;
  G4double qval[nCh]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,
                      0.,0.,0.,0.,0.,0.};
  qval[ 0] =  totM - mass[ 0];
  qval[ 1] = (totM - mass[ 1] - mNeut)*prN;
  qval[ 2] = (totM - mass[ 2] - mProt)*prZ;
  qval[ 3] = (totM - mass[ 3] - mDeut)*prD/3.;
  qval[ 4] = (totM - mass[ 4] - mTrit)*prT/6.;
  qval[ 5] = (totM - mass[ 5] - mHel3)*prH/fhe3;
  qval[ 6] = (totM - mass[ 6] - mAlph)*prA/9.;
  qval[ 7] = (totM - mass[ 7] - mNeut - mNeut)*prN*prN;
  qval[ 8] = (totM - mass[ 8] - mNeut - mProt)*prD;
  qval[ 9] = (totM - mass[ 9] - mProt - mProt)*prZ*prZ;
  qval[10] = (totM - mass[10] - mProt - mDeut)*prH/3.;
  qval[11] = (totM - mass[11] - mNeut - mDeut)*prT/3.;
  qval[12] = (totM - mass[12] - mDeut - mDeut)*prA/3./3.;
  qval[13] = (totM - mass[13] - mProt - mTrit)*prA/6.;
  qval[14] = (totM - mass[14] - mNeut - mTrit)*prT*prN/6.;
  qval[15] = (totM - mass[15] - mProt - mHel3)*prH*prZ/fhe3;
  qval[16] = (totM - mass[16] - mNeut - mHel3)*prA/fhe3;
  qval[17] = (totM - mass[17] - mProt - mAlph)*prZ*prA/9.;
  qval[18] = (totM - mass[18] - mNeut - mAlph)*prN*prA/9.;
  if(pZ>0)
  {
    qval[19] = (totM - mass[19] - mLamb)*prL;
    qval[20] = (totM - mass[20] - mProt - mLamb)*prL*prZ;
    qval[21] = (totM - mass[21] - mNeut - mLamb)*prL*prN;
    qval[22] = (totM - mass[22] - mDeut - mLamb)*prL*prD/2.;
    qval[23] = (totM - mass[23] - mTrit - mLamb)*prL*prT/3.;
    qval[24] = (totM - mass[24] - mHel3 - mLamb)*prL*prH/fhe3;
    qval[25] = (totM - mass[25] - mAlph - mLamb)*prL*prA/4;
  }
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: Q-values are calculated, tgA="<<tA<<"prA="<<pA<<G4endl;
#endif

  if( !pZ && pN==1)
  {
    if(G4UniformRand()>(tA-1.)*(tA-1.)/52900.) qval[0] = 0.0;
    if(G4UniformRand()>kinEnergy/7.925*tA) qval[2]=qval[3]=qval[4]=qval[5]=qval[9]=0.;
  }
  else qval[0] = 0.0;
  
  G4double qv = 0.0;                        // Total sum of probabilities (q-values)
  for(G4int i=0; i<nCh; ++i )
  {
    if( mass[i] < 500.*MeV ) qval[i] = 0.0; // Close A/Z impossible channels
    if( qval[i] < 0.0 )      qval[i] = 0.0; // Close the splitting impossible channels
    qv += qval[i];
  }
  // Select the channel
  G4double qv1 = 0.0;
  G4double ran = G4UniformRand();
  G4int index  = 0;
  for( index=0; index<nCh; ++index )
  {
    if( qval[index] > 0.0 )
    {
      qv1 += qval[index]/qv;
      if( ran <= qv1 ) break;
    }
  }
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: index="<<index<<" < "<<nCh<<G4endl;
#endif
  if(index == nCh)
  {
    G4cout<<"***G4QLowEnergy::PoStDI:Decay is impossible,totM="<<totM<<",GSM="<<tM<<G4endl;
    throw G4QException("G4QLowEnergy::PostStepDoIt: Can't decay the Compound");
  }
  // @@ Convert it to G4QHadrons    
  G4DynamicParticle* ResSec = new G4DynamicParticle;
  G4DynamicParticle* FstSec = new G4DynamicParticle;
  G4DynamicParticle* SecSec = new G4DynamicParticle;
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: Dynamic particles are created"<<G4endl;
#endif

  G4LorentzVector res4Mom(zeroMom,mass[index]*MeV); // The recoil nucleus prototype
  G4double mF=0.;
  G4double mS=0.;
  G4int rA=totZ+totN;
  G4int rZ=totZ;
  G4int rL=pL;
  switch( index )
  {
   case 0:
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aGamma );
     SecSec->SetDefinition( aGamma );
     break;
   case 1:
     rA-=1;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aGamma );
     mF=mNeut; // First hadron 4-momentum
     break;
   case 2:
     rZ-=1;
     rA-=1;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aProton );
     SecSec->SetDefinition( aGamma );
     mF=mProt; // First hadron 4-momentum
     break;
   case 3:
     rZ-=1;
     rA-=2;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aDeuteron );
     SecSec->SetDefinition( aGamma );
     mF=mDeut; // First hadron 4-momentum
     break;
   case 4:
     rZ-=1;
     rA-=3;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aTriton );
     SecSec->SetDefinition( aGamma );
     mF=mTrit; // First hadron 4-momentum
     break;
   case 5:
     rZ-=2;
     rA-=3;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aHe3);
     SecSec->SetDefinition( aGamma );
     mF=mHel3; // First hadron 4-momentum
     break;
   case 6:
     rZ-=2;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( anAlpha );
     SecSec->SetDefinition( aGamma );
     mF=mAlph; // First hadron 4-momentum
     break;
   case 7:
     rA-=2;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aNeutron );
     mF=mNeut; // First hadron 4-momentum
     mS=mNeut; // Second hadron 4-momentum
     break;
   case 8:
     rZ-=1;
     rA-=2;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aProton );
     mF=mNeut; // First hadron 4-momentum
     mS=mProt; // Second hadron 4-momentum
     break;
   case 9:
     rZ-=2;
     rA-=2;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aProton );
     SecSec->SetDefinition( aProton );
     mF=mProt; // First hadron 4-momentum
     mS=mProt; // Second hadron 4-momentum
     break;
   case 10:
     rZ-=2;
     rA-=3;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aProton );
     SecSec->SetDefinition( aDeuteron );
     mF=mProt; // First hadron 4-momentum
     mS=mDeut; // Second hadron 4-momentum
     break;
   case 11:
     rZ-=1;
     rA-=3;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aDeuteron );
     mF=mNeut; // First hadron 4-momentum
     mS=mDeut; // Second hadron 4-momentum
     break;
   case 12:
     rZ-=2;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aDeuteron );
     SecSec->SetDefinition( aDeuteron );
     mF=mDeut; // First hadron 4-momentum
     mS=mDeut; // Second hadron 4-momentum
     break;
   case 13:
     rZ-=2;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aProton );
     SecSec->SetDefinition( aTriton );
     mF=mProt; // First hadron 4-momentum
     mS=mTrit; // Second hadron 4-momentum
     break;
   case 14:
     rZ-=1;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aTriton );
     mF=mNeut; // First hadron 4-momentum
     mS=mTrit; // Second hadron 4-momentum
     break;
   case 15:
     rZ-=3;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aProton);
     SecSec->SetDefinition( aHe3 );
     mF=mProt; // First hadron 4-momentum
     mS=mHel3; // Second hadron 4-momentum
     break;
   case 16:
     rZ-=2;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aHe3 );
     mF=mNeut; // First hadron 4-momentum
     mS=mHel3; // Second hadron 4-momentum
     break;
   case 17:
     rZ-=3;
     rA-=5;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aProton );
     SecSec->SetDefinition( anAlpha );
     mF=mProt; // First hadron 4-momentum
     mS=mAlph; // Second hadron 4-momentum
     break;
   case 18:
     rZ-=2;
     rA-=5;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,pL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( anAlpha );
     mF=mNeut; // First hadron 4-momentum
     mS=mAlph; // Second hadron 4-momentum
     break;
   case 19:
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( aLambda );
     SecSec->SetDefinition( aGamma );
     mF=mLamb; // First hadron 4-momentum
     break;
   case 20:
     rZ-=1;
     rA-=1;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( aProton );
     SecSec->SetDefinition( aLambda );
     mF=mProt; // First hadron 4-momentum
     mS=mLamb; // Second hadron 4-momentum
     break;
   case 21:
     rA-=1;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( aNeutron );
     SecSec->SetDefinition( aLambda );
     mF=mNeut; // First hadron 4-momentum
     mS=mLamb; // Second hadron 4-momentum
     break;
   case 22:
     rZ-=1;
     rA-=2;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( aDeuteron );
     SecSec->SetDefinition( aLambda );
     mF=mDeut; // First hadron 4-momentum
     mS=mLamb; // Second hadron 4-momentum
     break;
   case 23:
     rZ-=1;
     rA-=3;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( aTriton );
     SecSec->SetDefinition( aLambda );
     mF=mTrit; // First hadron 4-momentum
     mS=mLamb; // Second hadron 4-momentum
     break;
   case 24:
     rZ-=2;
     rA-=3;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( aHe3 );
     SecSec->SetDefinition( aLambda );
     mF=mHel3; // First hadron 4-momentum
     mS=mLamb; // Second hadron 4-momentum
     break;
   case 25:
     rZ-=2;
     rA-=4;
     ResSec->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon(rZ,rA,rL,0) );
     FstSec->SetDefinition( anAlpha );
     SecSec->SetDefinition( aLambda );
     mF=mAlph; // First hadron 4-momentum
     mS=mLamb; // Second hadron 4-momentum
     break;
  }
#ifdef debug
  G4cout<<"G4QLowEn::PSDI: mF="<<mF<<", mS="<<mS<<G4endl;
#endif
  G4LorentzVector fst4Mom(zeroMom,mF); // Prototype of the first hadron 4-momentum
  G4LorentzVector snd4Mom(zeroMom,mS); // Prototype of the second hadron 4-momentum
  G4LorentzVector dir4Mom=tot4M;       // Prototype of the resN decay direction 4-momentum
  dir4Mom.setE(tot4M.e()/2.);          // Get half energy and total 3-momentum
  // @@ Can be repeated to take into account the Coulomb Barrier
  if(!G4QHadron(tot4M).CopDecayIn3(fst4Mom,snd4Mom,res4Mom,dir4Mom,cosp))
  {                   //                                          
		  G4cerr<<"**G4LowEnergy::PoStDoIt:i="<<index<<",tM="<<totM<<"->M1="<<res4Mom.m()<<"+M2="
     <<fst4Mom.m()<<"+M3="<<snd4Mom.m()<<"=="<<res4Mom.m()+fst4Mom.m()+snd4Mom.m()<<G4endl;
    throw G4QException("G4QLowEnergy::PostStepDoIt: Can't decay the Compound");
		}                   //                                          
#ifdef debug
  G4cout<<"G4QLowEn::PSDI:r4M="<<res4Mom<<",f4M="<<fst4Mom<<",s4M="<<snd4Mom<<G4endl;
#endif
  ResSec->Set4Momentum(res4Mom);
  FstSec->Set4Momentum(fst4Mom);
  SecSec->Set4Momentum(snd4Mom);
  //
  G4Track* aNewTrack = new G4Track(ResSec, localtime, position );
  aNewTrack->SetTouchableHandle(trTouchable);
  aParticleChange.AddSecondary( aNewTrack );
  aNewTrack = new G4Track(FstSec, localtime, position );
  aNewTrack->SetTouchableHandle(trTouchable);
  aParticleChange.AddSecondary( aNewTrack );
  aNewTrack = new G4Track(SecSec, localtime, position );
  aNewTrack->SetTouchableHandle(trTouchable);
  aParticleChange.AddSecondary( aNewTrack );
  // CASP algorithm implementation --- STOPS HERE
#ifdef debug
  G4cout<<"G4QLowEnergy::PostStepDoIt:*** PostStepDoIt is done ***"<<G4endl;
#endif
  return G4VDiscreteProcess::PostStepDoIt(track, step);
}

G4double G4QLowEnergy::CalculateXS(G4double p, G4int Z, G4int N, G4int PDG) 
{
  static G4bool first=true;
  static G4VQCrossSection* CSmanager;
  if(first)                              // Connection with a singletone
  {
    CSmanager=G4QIonIonCrossSection::GetPointer();
    first=false;
  }
#ifdef debug
		G4cout<<"G4QLowE::CXS: *DONE* p="<<p<<",Z="<<Z<<",N="<<N<<",PDG="<<PDG<<G4endl;
#endif
  return CSmanager->GetCrossSection(true, p, Z, N, PDG);
}
