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
//---------------------------------------------------------------------------
//
// ClassName:   test31Histo
//
//
// Author:      V.Ivanchenko 30/01/01
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "test31Histo.hh"
#include "Histo.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"
#include "G4Alpha.hh"
#include "G4LossTableManager.hh"
#include "G4ProductionCutsTable.hh"
#include "G4EmCalculator.hh"
#include "EmAnalysis.hh"
#include "G4EmCorrections.hh"
#include "G4NistManager.hh"
#include "G4BraggModel.hh"
#include "G4BetheBlochModel.hh"
#include <iomanip>
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

test31Histo* test31Histo::fManager = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

test31Histo* test31Histo::GetPointer()
{
  if(!fManager) {
    fManager = new test31Histo();
  }
  return fManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

test31Histo::test31Histo()
{
  verbose = 0;
  nHisto  = 0;
  maxEnergy = 0.0;
  nTuple = false;
  histo = Histo::GetInstance();
  //  ema = new EmAnalysis();
  histoID.resize(5);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

test31Histo::~test31Histo()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::BeginOfHisto(G4int num)
{
  if(0 < verbose) G4cout << "test31Histo # " << num << " started " << G4endl;
  zend     = 0.0;
  zend2    = 0.0;
  zEvt     = 0.0;
  etot     = 0.0;

  n_evt  = 0;
  n_elec = 0;
  n_posit= 0;
  n_gam  = 0;
  n_step = 0;

  n_charged_leak = 0;
  n_gam_leak = 0;
  n_charged_back = 0;
  n_gam_back = 0;

  n_mumu = 0;
  n_pipi = 0;

  if(0 < nHisto) {
    if(num == 0) bookHisto();
    histo->book();

    if(verbose > 0) {
      G4cout << "test31Histo: Histograms are booked and run has been started"
             << G4endl;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::EndOfHisto()
{

  G4cout << "test31Histo: End of run actions are started" << G4endl;

  // Zend average

  G4cout<<"===================================================================="<<G4endl;
  if(zEvt > 0.0) {
    zend  /= zEvt;
    zend2 /= zEvt;
    zend2 -= zend*zend;
    G4double sig = 0.0;
    if(zend2 > 0.) sig = std::sqrt(zend2);
    zend2 = sig / std::sqrt(zEvt);
    G4cout << std::setprecision(5) << "Range(mm)= " << zend/mm
           << "; Stragling(mm)= " << sig/mm
           << std::setprecision(2) << " +- " << zend2/mm
           << "    " << zEvt << " events for range" << G4endl;
  }
  G4double x = (G4double)n_evt;
  if(n_evt > 0) x = 1.0/x;
  etot *= x;
  G4double xe = x*(G4double)n_elec;
  G4double xg = x*(G4double)n_gam;
  G4double xp = x*(G4double)n_posit;
  G4double xs = x*(G4double)n_step;
  G4double xcl = x*(G4double)n_charged_leak;
  G4double xgl = x*(G4double)n_gam_leak;
  G4double xcb = x*(G4double)n_charged_back;
  G4double xgb = x*(G4double)n_gam_back;
  G4double xmu = x*(G4double)n_mumu;
  G4double xpi = x*(G4double)n_pipi;
  G4cout                    << "Number of events               " << n_evt <<G4endl;
  G4cout << std::setprecision(4) << "Average energy deposit         " << etot/MeV << " MeV" << G4endl;
  G4cout << std::setprecision(4) << "Average number of e-           " << xe << G4endl;
  G4cout << std::setprecision(4) << "Average number of gamma        " << xg << G4endl;
  G4cout << std::setprecision(4) << "Average number of e+           " << xp << G4endl;
  G4cout << std::setprecision(4) << "Average number of steps        " << xs << G4endl;
  G4cout << std::setprecision(4) << "Average number of leak charged " << xcl << G4endl;
  G4cout << std::setprecision(4) << "Average number of leak gamma   " << xgl << G4endl;
  G4cout << std::setprecision(4) << "Average number of back charged " << xcb << G4endl;
  G4cout << std::setprecision(4) << "Average number of back gamma   " << xgb << G4endl;
  G4cout << std::setprecision(4) << "Average number of mu+mu-       " << xmu << G4endl;
  G4cout << std::setprecision(4) << "Average number of pi+pi-       " << xpi << G4endl;
  G4cout<<"===================================================================="<<G4endl;

  TableControl();
  if(0 < nHisto) {

    // normalise histograms
    for(G4int i=0; i<nHisto; i++) {
      histo->scale(i,x);
    }
    histo->save();
  }

  //if(nHisto == 0) TableControl();
  //  CrossSections();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::SaveEvent()
{
  if(nTuple) histo->addRow(0);        
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::SaveToTuple(const G4String& parname, G4double val)
{
  if(nTuple) histo->fillTuple(0, parname, val);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::SaveToTuple(const G4String& parname,G4double val, G4double)
{
  if(nTuple) histo->fillTuple(0, parname, val);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::bookHisto()
{
  G4double zmax = (AbsorberThickness + gap) * NumberOfAbsorbers / mm;
  G4cout << "test31Histo: "
         << " AbsThick(mm)= " << AbsorberThickness/mm
         << " Nabs= " << NumberOfAbsorbers
         << " zmax= " << zmax
         << " nHisto= " << nHisto
         << G4endl;


  // Creating an 1-dimensional histograms in the root directory of the tree
  
  histoID[0] = histo->add1D("10",
    "Energy deposit (MeV) in absorber (mm)",NumberOfAbsorbers,0.0,zmax/mm,mm);

  histoID[1] =  histo->add1D("11",
    "Energy (MeV) of secondary electrons",50,0.0,maxEnergy/MeV,MeV);

  histoID[2] =  histo->add1D("12",
    "Theta (degrees) of delta-electrons",36,0.0,180.,degree);

  histoID[3] =  histo->add1D("13",
    "Energy (MeV) of secondary gamma",50,0.0,maxEnergy/MeV,MeV);

  histoID[4] =  histo->add1D("14",
    "Theta (degrees) of secondary gamma",36,0.0,180.,degree);

  if(nHisto < 5) {
    for(G4int i=nHisto; i<5; i++) {histo->activate(histoID[i], false);}
  }

  if(nTuple) 
    if(histo->addTuple( "100", "Range/Energy",
  "float tkin mass beta xend, yend, zend, ltpk, tend, teta, loss, dedx, back, leak, edep" ));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::AddEnergy(G4double edep, G4double z)
{
  etot += edep;
  if(0 < nHisto) histo->fill(histoID[0], z, edep/MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::AddEndPoint(G4double z)
{
  zend  += z;
  zend2 += z*z;
  zEvt  += 1.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::AddDeltaElectron(const G4DynamicParticle* elec)
{
  n_elec++;
  if(1 < nHisto) histo->fill(histoID[1],elec->GetKineticEnergy(),1.0);
  if(2 < nHisto) histo->fill(histoID[2],elec->GetMomentumDirection().theta(),1.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::AddPhoton(const G4DynamicParticle* ph)
{
  n_gam++;
  if(3 < nHisto) histo->fill(histoID[3],ph->GetKineticEnergy(),1.0);
  if(4 < nHisto) histo->fill(histoID[4],(ph->GetMomentumDirection()).theta(),1.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::AddParticleLeak(const G4DynamicParticle* dp)
{
  if(dp->GetDefinition() == G4Gamma::Gamma()) {
    n_gam_leak++;
  } else if (dp->GetCharge() != 0.0) {
    n_charged_leak++;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::AddParticleBack(const G4DynamicParticle* dp)
{
  if(dp->GetDefinition() == G4Gamma::Gamma()) {
    n_gam_back++;
  } else if (dp->GetCharge() != 0.0) {
    n_charged_back++;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::TableControl()
{
  G4NistManager*  mman = G4NistManager::Instance();
  G4EmCorrections* emc = G4LossTableManager::Instance()->EmCorrections();
  G4EmCalculator cal;
  cal.SetVerbose(0);

  // parameters
  G4double tmin = 1.*keV;
  G4double tmax = 1.*GeV;
  G4int    nbin = 60;
  //  G4int   index = 1;
  //  G4double cut  = 100.*GeV; 
  //G4ParticleDefinition* part = G4Proton::Proton();
  //  G4ParticleDefinition* part = G4Electron::Electron();
  //  G4ParticleDefinition* part = G4Alpha::Alpha();
  // cal.PrintDEDXTable(part);
  // cal.PrintRangeTable(part);
  // cal.PrintInverseRangeTable(part);

  //  G4String part_name = "proton";
     G4String part_name = "alpha";
     // G4String part_name = "C12[0.0]";
   //   G4String mat_name  = "Tangsten";
    G4String mat_name  = "G4_Al";
   //     G4String mat_name  = "Silicon";
   //  G4String mat_name  = "Gold";
    //   G4String mat_name  = "Iron";
   // G4String mat_name  = "Beryllium";
  G4String proc_name = "ionIoni";
  //G4String proc_name = "hLowEIoni";

  const G4ParticleDefinition* part = cal.FindParticle(part_name);
  const G4Material* mat = mman->FindOrBuildMaterial(mat_name);
  //  G4double fact = gram/(MeV*cm2*mat->GetDensity());

  G4double xmin = std::log10(tmin);
  G4double xmax = std::log10(tmax);
  G4double step = (xmax - xmin)/(G4double)nbin;
  G4double x    = xmin;
  G4cout << "====================================================================" << G4endl;
  G4cout << "   Tables control for  " << part_name << G4endl;
  G4cout << "   Material            " << mat_name << G4endl;
  G4cout << "====================================================================" << G4endl;

  for(G4int i=0; i<=nbin; i++) {
    /*    
    G4double e = std::pow(10.,x);
    G4double dedx0 = cal.GetDEDX(e,part,mat);
    G4double dedx = cal.ComputeDEDX(e,part_name,proc_name,mat_name,cut);
    G4cout << i << ".   e(MeV)= " << e/MeV 
           << ";  Computed  dedx(MeV*cm^2/g)= " << dedx*fact
           << ";  Tabled  dedx(MeV*cm^2/g)= " << dedx0*fact
           << G4endl;
    */
    x += step;
  }

  G4bool icorr = false;
  if(icorr) {
    G4cout << "====================================================================" << G4endl;
    G4cout << "             Ionisation Corrections" << G4endl;
    G4cout << "====================================================================" << G4endl;

    G4String nm[7] = {"G4_H", "G4_C", "G4_Al", "G4_Cu", "G4_Ag", "G4_Au", "G4_Pb"};
    G4double ek[17] = {1.0, 1.25, 1.5,1.75, 2.0, 2.5, 3.0, 3.5,  4.0, 4.5, 5.0, 6.5, 8.0,  12.5, 20.0, 100., 1000.};

    part = cal.FindParticle("proton");
    G4double mass = part->GetPDGMass();
  
    G4double L, L0, L1, L2, Spin, KS, LS, S, S0, del, mk, dedx, fac, fact, dc, nuc;
    for(G4int i=0; i<7; i++) {
      dc = 0.0;
      mat = mman->FindOrBuildMaterial(nm[i]);
      fact = gram/(MeV*cm2*mat->GetDensity());
      fac = 2.0*twopi_mc2_rcl2*(mat->GetElectronDensity())*fact;
      G4cout << "   New Material  " << mat->GetName() << G4endl;
      for(G4int j=0; j<17; j++) {
        G4double e = ek[j]*MeV;
        G4double tau = e/mass;
        G4double gamma = 1.0 + tau;
        G4double beta2 = tau*(tau + 2.0)/(gamma*gamma);
	//        G4double dedx0 = cal.ComputeDEDX(e,part,"hIoni",mat)*fact;
    
	L0   = emc->Bethe(part,mat,e);
	Spin = emc->SpinCorrection(part,mat,e);
	KS   = emc->KShellCorrection(part,mat,e);
	LS   = emc->LShellCorrection(part,mat,e);
	S    = emc->ShellCorrection(part,mat,e);
	S0   = emc->ShellCorrectionSTD(part,mat,e);
	L1   = emc->BarkasCorrection(part,mat,e);
	L2   = emc->BlochCorrection(part,mat,e);
        del  = 0.5*emc->DensityCorrection(part,mat,e);
        mk   = 0.5*emc->MottCorrection(part,mat,e);
        nuc  = fact*emc->NuclearDEDX(part,mat,e);
        L = L0 + L1 + L2 + mk - del -S;
        dedx = L*fac/beta2;
	// if(0 == j) dc = (dedx0 - dedx)*MeV*MeV;
	//	dedx += dc/(e*e);
        x = S + del - L1 - L2;
	G4cout << j+1 << ". " << ek[j] << " MeV "
	  // << " L0= " << L0
	  //     << " Spin= " << Spin
	       << " KShell= " << KS
	       << " LShell= " << LS
	       << " Sh= " << S 
	       << " Sh0= " << S0 
	       << " L1= " << L1
	       << " L2= " << L2
	       << " del= " << del
	       << " mott= " << mk
	       << " x= " << x
	       << " L= " << L
	       << " dedx= " << dedx
	  //     << " dedx0= " << dedx0
	       << " nuc= " << nuc
	       << G4endl;
      }
      G4cout << "====================================================================" << G4endl;
    }
  }

  G4cout << "====================================================================" << G4endl;

  G4bool ish = false;
  if (ish) {

    const G4ParticleDefinition* proton = cal.FindParticle("proton");
    G4BraggModel        bragg;
    G4BetheBlochModel   bethe;
    G4DataVector        empty;
    bragg.Initialise(proton, empty);
    bethe.Initialise(proton, empty);
    const G4Element* elm;
    const G4Material* ma;
   
    G4double e     = 2.0*MeV;
    G4double tau   = e/proton_mass_c2;
    G4double gam   = tau + 1.0;
    G4double bg2   = tau * (tau+2.0);
    G4double beta2 = bg2/(gam*gam);
    G4double eta   = beta2/(fine_structure_const*fine_structure_const);
    G4double fact  = 0.5 * beta2*eta*eta / twopi_mc2_rcl2;
    G4double dedx0, dedx1;

    for(G4int z=1; z<93; z++) {
      elm = mman->FindOrBuildElement(z, false);
      G4String nam = "G4_"+elm->GetSymbol();
      ma = mman->FindOrBuildMaterial(nam, false);
      //      G4cout << "Elm " << elm << "  mat " << ma << "   " << nam << G4endl;
    }
    for(G4int z=1; z<93; z++) {
      elm = mman->FindOrBuildElement(z, false);
      G4String nam = "G4_"+elm->GetSymbol();
      ma = mman->FindOrBuildMaterial(nam, false);
      dedx0 = bragg.ComputeDEDXPerVolume(ma,proton,e,GeV);
      dedx1 = bethe.ComputeDEDXPerVolume(ma,proton,e,GeV);
      G4cout << " " << (dedx1 - dedx0)*fact/(ma->GetElectronDensity()) << ",";
      if(z/10*10 == z) G4cout << G4endl;
    }
    G4cout << G4endl;
    G4double fe     = 8.0*MeV;
    G4double ftau   = fe/proton_mass_c2;
    G4double fgam   = ftau + 1.0;
    G4double fbg2   = ftau * (ftau+2.0);
    G4double fbeta2 = fbg2/(fgam*fgam);
    G4double ffact  = 0.5 * fbeta2 / twopi_mc2_rcl2;
    G4double fdedx0, fdedx1, s0, s1;
    G4cout << G4endl;
    for(G4int z=1; z<93; z++) {
      elm = mman->FindOrBuildElement(z, false);
      G4String nam = "G4_"+elm->GetSymbol();
      ma = mman->FindOrBuildMaterial(nam, false);
      fdedx0 = bragg.ComputeDEDXPerVolume(ma,proton,e,GeV)*ffact/ma->GetElectronDensity();
      fdedx1 = bethe.ComputeDEDXPerVolume(ma,proton,e,GeV)*ffact/ma->GetElectronDensity();
      s0 = fdedx1 - fdedx0;
      s1 = emc->ShellCorrectionSTD(proton,ma,fe);

      G4cout << " " << (s1*std::log(tau) - s0*std::log(ftau))/(ftau - tau) << ",";
      // << " s0= " << s0 << " s1= " << s1 << G4endl;;
      if(z/10*10 == z) G4cout << G4endl;
    }
    G4cout << G4endl;
  }

  G4bool ihist = false;
  if (ihist) {
    G4cout << "====================================================================" << G4endl;
    G4cout << "             Stopping Powers" << G4endl;
    G4cout << "====================================================================" << G4endl;

    G4String nm[7] = {"G4_Be", "G4_Al", "G4_Si", "G4_Ge", "G4_Fe", "G4_Ag", "G4_Au"};
    const G4String partc[2] = {"proton", "alpha"};
    const G4String proc[2] = {"hIoni", "ionIoni"};
    
    G4double e, se, sn, st, mce, mcn, mct; 
    char line[200];

    for(G4int ii=0; ii<2; ii++) {
    
      const G4ParticleDefinition* part = cal.FindParticle(partc[ii]);

      for(G4int i=0; i<7; i++) {
	mat = mman->FindOrBuildMaterial(nm[i]);
        G4cout << "  Particle  " << partc[ii] << " in  Material  " << mat->GetName() << G4endl;
	G4double fact = gram/(MeV*cm2*mat->GetDensity());
        std::ifstream* fin = new std::ifstream();
        std::string fname = "stopping/" + partc[ii] + "_" + nm[i];
        std::string fnamef= fname + ".txt";
        fin->open(fnamef.c_str());
        if( !fin->is_open()) {
          G4cout << "Input file <" << fname << "> does not exist! Exit" << G4endl;
          exit(1);
        }
        //G4int i1 = histo->addCloud1D(fname);
        for(G4int j=0; j<8; j++) {fin->getline( line, 200);}
        G4int i2 =0;
        do {
          i2++;
          (*fin) >> e >> se >> sn >> st;
          e *= MeV;
          mce = fact*(cal.ComputeDEDX(e,part,proc[ii],mat));
          mcn = fact*emc->NuclearDEDX(part,mat,e,false);
          mct = mce + mcn;
          G4double diff = 100.*(mct/st - 1.0);
	  /*
          G4cout << e/MeV 
                 << " NIST:  dedx= " << se
                 << " nuc= " << sn
                 << " tot= " << st
                 << " G4:  dedx= " << mce
                 << " nuc= " << mcn
                 << " tot= " << mct
                 << " diff= " << diff << " %"
                 << G4endl;
	  */
          if(ii==0 && i==0) G4cout << " " << e;
	  else              G4cout << " " << -diff;
          //histo->fill(i1,e,diff);
              
        } while ( std::fabs(e - 1000.) > MeV);
	  G4cout << G4endl;
	  G4cout << "========= n= " << i2 << " ===========================================================" << G4endl;
        fin->close();
      }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void test31Histo::CountProcess(const G4String& name)
{
  //  G4cout << "### " << name << G4endl;
  if(name == "AnnihiToMuPair") n_mumu++;  
  else if(name == "ee2hadr") n_pipi++;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
