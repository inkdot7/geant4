#ifndef G4LENDManager_h
#define G4LENDManager_h 1

// Class Description
// Manager of LEND (Low Energy Nuclear Data) target (nucleus) 
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

#include "G4LENDHeader.hh"
#include "G4ParticleDefinition.hh"
#include "G4NistElementBuilder.hh" 
#include "G4IonTable.hh"

#include <map>

struct lend_target
{
// properties for the traget
   G4GIDI* lend;
   G4GIDI_target* target;
   
   G4ParticleDefinition* proj;
   G4int target_code;
   G4String evaluation;
};



class G4LENDManager 
{

      static G4LENDManager* lend_manager;
   
   protected: 
      G4LENDManager();
      G4LENDManager( const G4LENDManager& ){;};

      ~G4LENDManager();

   public:
      static G4LENDManager* GetInstance() 
      {
         if ( lend_manager == NULL) lend_manager = new G4LENDManager();
         return lend_manager;
      };

      G4GIDI_target* GetLENDTarget( G4ParticleDefinition* , G4String , G4int iZ , G4int iA , G4int iM = 0 );
      std::vector< G4String > IsLENDTargetAvailable( G4ParticleDefinition* , G4int iZ , G4int iA , G4int iM = 0 );
      G4int GetNucleusEncoding ( G4int iZ , G4int iA ) // GroundState only
      { return ionTable->GetNucleusEncoding( iZ , iA ); };

      G4NistElementBuilder* GetNistElementBuilder(){ return nistElementBuilder; };
   
   private:

      std::vector< lend_target > v_lend_target; 

      //        proj                              
      std::map< G4ParticleDefinition* , G4GIDI* > proj_lend_map; 

//    std::map< G4ParticleDefinition* , std::vector< G4int > > proj_checked_map; 

      G4IonTable* ionTable;

      G4NistElementBuilder* nistElementBuilder; 

      void printBanner();

};

#endif
