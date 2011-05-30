// Class Description
// Final state production model for a LEND (Low Energy Nuclear Data) 
// LEND is Geant4 interface for GIDI (General Interaction Data Interface) 
// which gives a discription of nuclear and atomic reactions, such as
//    Binary collision cross sections
//    Particle number multiplicity distributions of reaction products
//    Energy and angular distributions of reaction products
//    Derived calculational constants
// GIDI is developped at Lawrence Livermore National Laboratory
// Class Description - End

// 071025 First implementation done by T. Koi (SLAC/SCCS)
// 101118 Name modifications for release T. Koi (SLAC/PPA)

#include "G4LENDModel.hh"
#include "G4NistManager.hh"

G4LENDModel::G4LENDModel( G4String name )
:G4HadronicInteraction( name )
{

   SetMinEnergy( 0.*eV );
   SetMaxEnergy( 20.*MeV );

   default_evaluation = "endl99"; 
   allow_nat = false;
   allow_any = false;

   lend_manager = G4LENDManager::GetInstance();  

}

G4LENDModel::~G4LENDModel()
{
   for ( std::map< G4int , G4LENDUsedTarget* >::iterator 
         it = usedTarget_map.begin() ; it != usedTarget_map.end() ; it ++ )
   { 
      delete it->second;  
   }
}


void G4LENDModel::recreate_used_target_map()
{

   for ( std::map< G4int , G4LENDUsedTarget* >::iterator 
         it = usedTarget_map.begin() ; it != usedTarget_map.end() ; it ++ )
   { 
      delete it->second;  
   }
   usedTarget_map.clear();

   create_used_target_map();

}



void G4LENDModel::create_used_target_map()
{

   size_t numberOfElements = G4Element::GetNumberOfElements();
   static const G4ElementTable* theElementTable = G4Element::GetElementTable();

   for ( size_t i = 0 ; i < numberOfElements ; ++i )
   {

      const G4Element* anElement = (*theElementTable)[i];
      G4int numberOfIsotope = anElement->GetNumberOfIsotopes(); 

      if ( numberOfIsotope > 0 )
      {
      // User Defined Abundances   
         for ( G4int i_iso = 0 ; i_iso < numberOfIsotope ; i_iso++ )
         {
            G4int iZ = anElement->GetIsotope( i_iso )->GetZ();
            G4int iA = anElement->GetIsotope( i_iso )->GetN();

            G4LENDUsedTarget* aTarget = new G4LENDUsedTarget ( proj , default_evaluation , iZ , iA );  
            if ( allow_nat == true ) aTarget->AllowNat();
            if ( allow_any == true ) aTarget->AllowAny();
            usedTarget_map.insert( std::pair< G4int , G4LENDUsedTarget* > ( lend_manager->GetNucleusEncoding( iZ , iA ) , aTarget ) );
         }
      }
      else
      {
      // Natural Abundances   
         G4NistElementBuilder* nistElementBuild = lend_manager->GetNistElementBuilder();
         G4int iZ = int ( anElement->GetZ() );
         //G4cout << nistElementBuild->GetNumberOfNistIsotopes( int ( anElement->GetZ() ) ) << G4endl;
         G4int numberOfNistIso = nistElementBuild->GetNumberOfNistIsotopes( int ( anElement->GetZ() ) ); 

         for ( G4int i = 0 ; i < numberOfNistIso ; i++ )
         {
            //G4cout << nistElementBuild->GetIsotopeAbundance( iZ , nistElementBuild->GetNistFirstIsotopeN( iZ ) + i ) << G4endl;
            if ( nistElementBuild->GetIsotopeAbundance( iZ , nistElementBuild->GetNistFirstIsotopeN( iZ ) + i ) > 0 )
            {
               G4int iMass = nistElementBuild->GetNistFirstIsotopeN( iZ ) + i;  
               //G4cout << iZ << " " << nistElementBuild->GetNistFirstIsotopeN( iZ ) + i << " " << nistElementBuild->GetIsotopeAbundance ( iZ , iMass ) << G4endl;  

               G4LENDUsedTarget* aTarget = new G4LENDUsedTarget ( proj , default_evaluation , iZ , iMass );  
               if ( allow_nat == true ) aTarget->AllowNat();
               if ( allow_any == true ) aTarget->AllowAny();
               usedTarget_map.insert( std::pair< G4int , G4LENDUsedTarget* > ( lend_manager->GetNucleusEncoding( iZ , iMass ) , aTarget ) );

            }

         }

      }
   }



   //G4cout << "Dump UsedTarget for " << theModelName << G4endl;
   for ( std::map< G4int , G4LENDUsedTarget* >::iterator 
         it = usedTarget_map.begin() ; it != usedTarget_map.end() ; it ++ )
   {
      G4cout 
      << " " << it->second->GetWantedEvaluation() 
      << " " << it->second->GetWantedZ() 
      << " " << it->second->GetWantedA() 
      << " " << it->second->GetActualEvaluation() 
      << " " << it->second->GetActualZ() 
      << " " << it->second->GetActualA() 
      << " " << it->second->GetTarget() 
      << G4endl; 
   } 

}
  

  
#include "G4ParticleTable.hh"
  
G4HadFinalState * G4LENDModel::ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& aTarg )
{

   G4double temp = aTrack.GetMaterial()->GetTemperature();

   G4int iZ = int ( aTarg.GetZ() );
   G4int iA = int ( aTarg.GetN() );

   G4double ke = aTrack.GetKineticEnergy();

   G4HadFinalState* theResult = new G4HadFinalState();

   G4GIDI_target* aTarget = usedTarget_map.find( lend_manager->GetNucleusEncoding( iZ , iA ) )->second->GetTarget();

   G4double aMu = aTarget->getElasticFinalState( ke*MeV, temp, NULL, NULL );

   G4double phi = twopi*G4UniformRand();
   G4double theta = std::acos( aMu );
   //G4double sinth = std::sin( theta );

   G4ReactionProduct theNeutron( const_cast<G4ParticleDefinition *>( aTrack.GetDefinition() ) );
   theNeutron.SetMomentum( aTrack.Get4Momentum().vect() );
   theNeutron.SetKineticEnergy( ke );

   G4ReactionProduct theTarget( G4ParticleTable::GetParticleTable()->FindIon( iZ , iA , 0 , iZ ) );

   G4double mass = G4ParticleTable::GetParticleTable()->FindIon( iZ , iA , 0 , iZ )->GetPDGMass();

// add Thermal motion 
   G4double kT = k_Boltzmann*temp;
   G4ThreeVector v ( G4RandGauss::shoot() * std::sqrt( kT*mass ) 
                   , G4RandGauss::shoot() * std::sqrt( kT*mass ) 
                   , G4RandGauss::shoot() * std::sqrt( kT*mass ) );

   theTarget.SetMomentum( v );


     G4ThreeVector the3Neutron = theNeutron.GetMomentum();
     G4double nEnergy = theNeutron.GetTotalEnergy();
     G4ThreeVector the3Target = theTarget.GetMomentum();
     G4double tEnergy = theTarget.GetTotalEnergy();
     G4ReactionProduct theCMS;
     G4double totE = nEnergy+tEnergy;
     G4ThreeVector the3CMS = the3Target+the3Neutron;
     theCMS.SetMomentum(the3CMS);
     G4double cmsMom = std::sqrt(the3CMS*the3CMS);
     G4double sqrts = std::sqrt((totE-cmsMom)*(totE+cmsMom));
     theCMS.SetMass(sqrts);
     theCMS.SetTotalEnergy(totE);

       theNeutron.Lorentz(theNeutron, theCMS);
       theTarget.Lorentz(theTarget, theCMS);
       G4double en = theNeutron.GetTotalMomentum(); // already in CMS.
       G4ThreeVector cms3Mom=theNeutron.GetMomentum(); // for neutron direction in CMS
       G4double cms_theta=cms3Mom.theta();
       G4double cms_phi=cms3Mom.phi();
       G4ThreeVector tempVector;
       tempVector.setX(std::cos(theta)*std::sin(cms_theta)*std::cos(cms_phi)
                       +std::sin(theta)*std::cos(phi)*std::cos(cms_theta)*std::cos(cms_phi)
                       -std::sin(theta)*std::sin(phi)*std::sin(cms_phi)  );
       tempVector.setY(std::cos(theta)*std::sin(cms_theta)*std::sin(cms_phi)
                       +std::sin(theta)*std::cos(phi)*std::cos(cms_theta)*std::sin(cms_phi)
                       +std::sin(theta)*std::sin(phi)*std::cos(cms_phi)  );
       tempVector.setZ(std::cos(theta)*std::cos(cms_theta)
                       -std::sin(theta)*std::cos(phi)*std::sin(cms_theta)  );
       tempVector *= en;
       theNeutron.SetMomentum(tempVector);
       theTarget.SetMomentum(-tempVector);
       G4double tP = theTarget.GetTotalMomentum();
       G4double tM = theTarget.GetMass();
       theTarget.SetTotalEnergy(std::sqrt((tP+tM)*(tP+tM)-2.*tP*tM));
       theNeutron.Lorentz(theNeutron, -1.*theCMS);
       theTarget.Lorentz(theTarget, -1.*theCMS);

     theResult->SetEnergyChange(theNeutron.GetKineticEnergy());
     theResult->SetMomentumChange(theNeutron.GetMomentum().unit());
     G4DynamicParticle* theRecoil = new G4DynamicParticle;

     theRecoil->SetDefinition( G4ParticleTable::GetParticleTable()->FindIon( iZ, iA , 0, iZ ) );
     theRecoil->SetMomentum( theTarget.GetMomentum() );

     theResult->AddSecondary( theRecoil );

   return theResult; 

}
