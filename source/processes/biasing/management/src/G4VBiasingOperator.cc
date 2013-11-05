#include "G4VBiasingOperator.hh"
#include "G4VBiasingOperation.hh"
#include "G4VParticleChange.hh"
#include "G4BiasingTrackData.hh"
#include "G4BiasingTrackDataStore.hh"


std::map< const G4LogicalVolume*, G4VBiasingOperator* > G4VBiasingOperator::fLogicalToSetupMap;
std::vector < G4VBiasingOperator* > G4VBiasingOperator::fOperators;


G4VBiasingOperator::G4VBiasingOperator(G4String name)
  : fName(name)
{
  fOperators.push_back(this);
}

G4VBiasingOperator::~G4VBiasingOperator()
{
}

void G4VBiasingOperator::AttachTo(const G4LogicalVolume* logical)
{
  std::map< const G4LogicalVolume*, G4VBiasingOperator* >::iterator it;
  it = fLogicalToSetupMap.find(logical);
  if ( it == fLogicalToSetupMap.end() ) fLogicalToSetupMap[logical] = this;
  else if ( (*it).second != this ) G4cout << "G4VBiasingOperator::AttachTo() : logical volume already used." << G4endl;
}


G4VBiasingOperator* G4VBiasingOperator::GetBiasingOperator(const G4LogicalVolume* logical)
{
  std::map< const G4LogicalVolume*, G4VBiasingOperator* >::iterator it;
  it = fLogicalToSetupMap.find(logical);
  if ( it == fLogicalToSetupMap.end() ) return 0;
  else return (*it).second;
}

G4VBiasingOperation* G4VBiasingOperator::GetProposedOccurenceBiasingOperation(const G4Track* track, const G4BiasingProcessInterface* callingProcess)
{
  fOccurenceBiasingOperation = ProposeOccurenceBiasingOperation(track, callingProcess);
  // G4cout << GetName() << " Occurence : ";
  // if ( fOccurenceBiasingOperation ) G4cout << fOccurenceBiasingOperation->GetName() << G4endl;
  //  else G4cout << " (none) " << G4endl;
  return fOccurenceBiasingOperation;
}

G4VBiasingOperation* G4VBiasingOperator::GetProposedFinalStateBiasingOperation(const G4Track* track, const G4BiasingProcessInterface* callingProcess)
{
  fFinalStateBiasingOperation = ProposeFinalStateBiasingOperation(track, callingProcess); 
   // G4cout << GetName() << " Final State : ";
   // if ( fFinalStateBiasingOperation ) G4cout << fFinalStateBiasingOperation->GetName() << G4endl;
   // else G4cout << " (none) " << G4endl;
  return fFinalStateBiasingOperation;
}

G4VBiasingOperation* G4VBiasingOperator::GetProposedNonPhysicsBiasingOperation(const G4Track* track, const G4BiasingProcessInterface* callingProcess)
{
  fNonPhysicsBiasingOperation = ProposeNonPhysicsBiasingOperation(track, callingProcess);
   // G4cout << GetName() << " Non Physics : ";
   // if ( fNonPhysicsBiasingOperation ) G4cout << fNonPhysicsBiasingOperation->GetName() << G4endl;
   // else G4cout << " (none) " << G4endl;
  return fNonPhysicsBiasingOperation;
}

// void G4VBiasingOperator::RegisterSecondariesBirth( G4VBiasingOperation* operation, const G4VParticleChange* pChange )
// {
//   for (G4int i2nd = 0; i2nd < pChange->GetNumberOfSecondaries (); i2nd++)
//     RegisterSecondaryBirth( operation, pChange->GetSecondary (i2nd) );
// }

// void G4VBiasingOperator::RegisterSecondaryBirth( G4VBiasingOperation* operation, const G4Track* track )
// {
//   //  G4cout << " --> Registering operation, track = " << operation->GetName() << " " << track << G4endl;
//   fSecondaryBirthOperation[track] = operation;
// }

const G4VBiasingOperation* G4VBiasingOperator::GetBirthOperation( const G4Track* track )
{
  const G4BiasingTrackData* biasingData = G4BiasingTrackDataStore::GetInstance()->GetBiasingTrackData(track);
  if ( biasingData != 0 ) return biasingData->GetBirthOperation();
  else return 0;
}


void G4VBiasingOperator::ReportOperationApplied( const G4BiasingProcessInterface*  callingProcess,
						 G4BiasingAppliedCase                 biasingCase,
						 G4VBiasingOperation*             operationApplied,
						 const G4VParticleChange*   particleChangeProduced )
{
  fPreviousBiasingAppliedCase = biasingCase;
  // if ( operationApplied != 0 )
  //   {
  //     if ( operationApplied->GetRememberSecondaries() )
  // 	{
  // 	  for (G4int i2nd = 0; i2nd < particleChangeProduced->GetNumberOfSecondaries (); i2nd++)
  // 	    new G4BiasingTrackData( particleChangeProduced->GetSecondary (i2nd),
  // 				    operationApplied,
  // 				    this,
  // 				    callingProcess);
  // 	}
  //   }
  fPreviousAppliedOccurenceBiasingOperation  = 0;
  fPreviousAppliedFinalStateBiasingOperation = 0;
  fPreviousAppliedNonPhysicsBiasingOperation = 0;
  switch ( biasingCase )
    {
    case BAC_None:
      break;
    case BAC_NonPhysics:
      fPreviousAppliedNonPhysicsBiasingOperation = operationApplied ;
      break;
    case BAC_DenyInteraction:
      fPreviousAppliedOccurenceBiasingOperation  = operationApplied;
      break;
    case BAC_FinalState:
      fPreviousAppliedFinalStateBiasingOperation = operationApplied;
      break;
    case BAC_Occurence:
      G4cout << " !!!! Logic problem !!!! " << G4endl;
      break;
    default:
      G4cout << " !!!! serious logic problem !!! " << G4endl;
    }
  OperationApplied( callingProcess, biasingCase, operationApplied, particleChangeProduced );
}

void G4VBiasingOperator::ReportOperationApplied( const G4BiasingProcessInterface*               callingProcess,
						 G4BiasingAppliedCase                              biasingCase,
						 G4VBiasingOperation*                occurenceOperationApplied,
						 G4double                        weightForOccurenceInteraction,
						 G4VBiasingOperation*               finalStateOperationApplied,
						 const G4VParticleChange*               particleChangeProduced )
{
  fPreviousBiasingAppliedCase = biasingCase;
  // G4VBiasingOperation* operation(finalStateOperationApplied != 0 ? finalStateOperationApplied : occurenceOperationApplied);
  // G4bool remember = occurenceOperationApplied->GetRememberSecondaries();
  // if ( finalStateOperationApplied != 0 ) remember = remember || finalStateOperationApplied->GetRememberSecondaries();
  // if ( remember )
  //   {
  //     for (G4int i2nd = 0; i2nd < particleChangeProduced->GetNumberOfSecondaries (); i2nd++)
  // 	new G4BiasingTrackData( particleChangeProduced->GetSecondary (i2nd),
  // 				operation,
  // 				this,
  // 				callingProcess);
  //   }
  fPreviousAppliedOccurenceBiasingOperation  =  occurenceOperationApplied;
  fPreviousAppliedFinalStateBiasingOperation = finalStateOperationApplied;
  OperationApplied( callingProcess, biasingCase, occurenceOperationApplied, weightForOccurenceInteraction, finalStateOperationApplied, particleChangeProduced );
}


void G4VBiasingOperator::ExitingBiasing( const G4Track* track, const G4BiasingProcessInterface* callingProcess )
{
  ExitBiasing( track, callingProcess );
  
  // -- reset all data members:
  fOccurenceBiasingOperation                  = 0 ;
  fFinalStateBiasingOperation                 = 0 ;
  fNonPhysicsBiasingOperation                 = 0 ;
  fPreviousProposedOccurenceBiasingOperation  = 0 ;
  fPreviousProposedFinalStateBiasingOperation = 0 ;
  fPreviousProposedNonPhysicsBiasingOperation = 0 ;
  fPreviousAppliedOccurenceBiasingOperation   = 0 ;
  fPreviousAppliedFinalStateBiasingOperation  = 0 ;
  fPreviousAppliedNonPhysicsBiasingOperation  = 0 ;
  fPreviousBiasingAppliedCase                 = BAC_None ;
}


void G4VBiasingOperator::RememberSecondaries( const G4BiasingProcessInterface*         callingProcess,
					      const G4VBiasingOperation*             operationApplied,
					      const G4VParticleChange*         particleChangeProduced)
{
  for (G4int i2nd = 0; i2nd < particleChangeProduced->GetNumberOfSecondaries (); i2nd++)
    new G4BiasingTrackData( particleChangeProduced->GetSecondary (i2nd),
			    operationApplied,
			    this,
			    callingProcess);
}

void G4VBiasingOperator::ForgetTrack( const G4Track* track )
{
  G4BiasingTrackData* biasingData = G4BiasingTrackDataStore::GetInstance()->GetBiasingTrackData(track);
  if ( biasingData != 0 ) delete biasingData;
}


// -- dummy empty implementations to allow letting arguments visible in the .hh
// -- but avoiding annoying warning messages about unused variables
// -- methods to inform operator that its biasing control is over:
void G4VBiasingOperator::ExitBiasing( const G4Track*, const G4BiasingProcessInterface*)
{}
void G4VBiasingOperator::OperationApplied( const G4BiasingProcessInterface*, G4BiasingAppliedCase,
					   G4VBiasingOperation*, const G4VParticleChange* )
{}
void G4VBiasingOperator::OperationApplied( const G4BiasingProcessInterface*, G4BiasingAppliedCase,
					   G4VBiasingOperation*,  G4double,
					   G4VBiasingOperation*, const G4VParticleChange* )
{}
