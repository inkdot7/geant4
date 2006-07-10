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
//
// $Id: G4PhysicalVolumeModel.cc,v 1.49 2006-07-10 15:59:59 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// John Allison  31st December 1997.
// Model for physical volumes.

#include "G4PhysicalVolumeModel.hh"

#include "G4ModelingParameters.hh"
#include "G4VGraphicsScene.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4BoundingSphereScene.hh"
#include "G4PhysicalVolumeSearchScene.hh"
#include "G4TransportationManager.hh"
#include "G4Polyhedron.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UnitsTable.hh"

#include <sstream>

G4bool G4PhysicalVolumeModel::G4PhysicalVolumeNodeID::operator<
  (const G4PhysicalVolumeModel::G4PhysicalVolumeNodeID& right) const
{
  if (fpPV < right.fpPV) return true;
  if (fpPV == right.fpPV) {
    if (fCopyNo < right.fCopyNo) return true;
    if (fCopyNo == right.fCopyNo)
      return fNonCulledDepth < right.fNonCulledDepth;
  }
  return false;
}

std::ostream& operator<<
  (std::ostream& os, const G4PhysicalVolumeModel::G4PhysicalVolumeNodeID node)
{
  G4VPhysicalVolume* pPV = node.GetPhysicalVolume();
  if (pPV) {
    os << pPV->GetName()
       << ':' << node.GetCopyNo()
       << '[' << node.GetNonCulledDepth() << ']';
  } else {
    os << "Null node";
  }
  return os;
}

G4PhysicalVolumeModel::G4PhysicalVolumeModel
(G4VPhysicalVolume*          pVPV,
 G4int                       requestedDepth,
 const G4Transform3D& modelTransformation,
 const G4ModelingParameters* pMP,
 G4bool useFullExtent):
  G4VModel        (modelTransformation, pMP),
  fpTopPV         (pVPV),
  fTopPVName      (pVPV -> GetName ()),
  fTopPVCopyNo    (pVPV -> GetCopyNo ()),
  fRequestedDepth (requestedDepth),
  fUseFullExtent  (useFullExtent),
  fCurrentDepth   (0),
  fpCurrentPV     (0),
  fpCurrentLV     (0),
  fpCurrentMaterial (0),
  fCurtailDescent (false),
  fpClippingPolyhedron (0)
{
  std::ostringstream o;
  o << fpTopPV -> GetCopyNo ();
  fGlobalTag = fpTopPV -> GetName () + "." + o.str();
  fGlobalDescription = "G4PhysicalVolumeModel " + fGlobalTag;

  CalculateExtent ();
}

G4PhysicalVolumeModel::~G4PhysicalVolumeModel () {}

void G4PhysicalVolumeModel::CalculateExtent ()
{
  if (fUseFullExtent) {
    fExtent = fpTopPV -> GetLogicalVolume () -> GetSolid () -> GetExtent ();
  }
  else {
    G4BoundingSphereScene bsScene(this);
    const G4int tempRequestedDepth = fRequestedDepth;
    fRequestedDepth = -1;  // Always search to all depths to define extent.
    const G4ModelingParameters* tempMP = fpMP;
    G4ModelingParameters mParams
      (0,      // No default vis attributes needed.
       G4ModelingParameters::wf,  // wireframe (not relevant for this).
       true,   // Global culling.
       true,   // Cull invisible volumes.
       false,  // Density culling.
       0.,     // Density (not relevant if density culling false).
       true,   // Cull daughters of opaque mothers.
       24);    // No of sides (not relevant for this operation).
    fpMP = &mParams;
    DescribeYourselfTo (bsScene);
    G4double radius = bsScene.GetRadius();
    if (radius < 0.) {  // Nothing in the scene.
      fExtent = fpTopPV -> GetLogicalVolume () -> GetSolid () -> GetExtent ();
    } else {
      // Transform back to coordinates relative to the top
      // transformation, which is in G4VModel::fTransform.  This makes
      // it conform to all models, which are defined by a
      // transformation and an extent relative to that
      // transformation...
      G4Point3D centre = bsScene.GetCentre();
      centre.transform(fTransform.inverse());
      fExtent = G4VisExtent(centre, radius);
    }
    fpMP = tempMP;
    fRequestedDepth = tempRequestedDepth;
  }
}

void G4PhysicalVolumeModel::DescribeYourselfTo
(G4VGraphicsScene& sceneHandler)
{
  if (!fpMP) G4Exception
    ("G4PhysicalVolumeModel::DescribeYourselfTo: No modeling parameters.");

  // For safety...
  fCurrentDepth = 0;

  G4Transform3D startingTransformation = fTransform;

  VisitGeometryAndGetVisReps
    (fpTopPV,
     fRequestedDepth,
     startingTransformation,
     sceneHandler);

  // Clear data...
  fCurrentDepth     = 0;
  fpCurrentPV       = 0;
  fpCurrentLV       = 0;
  fpCurrentMaterial = 0;
  fDrawnPVPath.clear();
}

G4String G4PhysicalVolumeModel::GetCurrentTag () const
{
  if (fpCurrentPV) {
    std::ostringstream o;
    o << fpCurrentPV -> GetCopyNo ();
    return fpCurrentPV -> GetName () + "." + o.str();
  }
  else {
    return "WARNING: NO CURRENT VOLUME - global tag is " + fGlobalTag;
  }
}

G4String G4PhysicalVolumeModel::GetCurrentDescription () const
{
  return "G4PhysicalVolumeModel " + GetCurrentTag ();
}

void G4PhysicalVolumeModel::VisitGeometryAndGetVisReps
(G4VPhysicalVolume* pVPV,
 G4int requestedDepth,
 const G4Transform3D& theAT,
 G4VGraphicsScene& sceneHandler)
{
  // Visits geometry structure to a given depth (requestedDepth), starting
  //   at given physical volume with given starting transformation and
  //   describes volumes to the scene handler.
  // requestedDepth < 0 (default) implies full visit.
  // theAT is the Accumulated Transformation.

  // Find corresponding logical volume and (later) solid, storing in
  // local variables to preserve re-entrancy.
  G4LogicalVolume* pLV  = pVPV -> GetLogicalVolume ();

  G4VSolid* pSol;
  G4Material* pMaterial;

  if (!(pVPV -> IsReplicated ())) {
    // Non-replicated physical volume.
    pSol = pLV -> GetSolid ();
    pMaterial = pLV -> GetMaterial ();
    DescribeAndDescend (pVPV, requestedDepth, pLV, pSol, pMaterial,
			theAT, sceneHandler);
  }
  else {
    // Replicated or parametrised physical volume.
    EAxis axis;
    G4int nReplicas;
    G4double width;
    G4double offset;
    G4bool consuming;
    pVPV -> GetReplicationData (axis, nReplicas, width,  offset, consuming);
    G4VPVParameterisation* pP = pVPV -> GetParameterisation ();
    if (pP) {  // Parametrised volume.
      for (int n = 0; n < nReplicas; n++) {
	pSol = pP -> ComputeSolid (n, pVPV);
	pMaterial = pP -> ComputeMaterial (n, pVPV);
	pP -> ComputeTransformation (n, pVPV);
	pSol -> ComputeDimensions (pP, n, pVPV);
	pVPV -> SetCopyNo (n);
	DescribeAndDescend (pVPV, requestedDepth, pLV, pSol, pMaterial,
			    theAT, sceneHandler);
      }
    }
    else {  // Plain replicated volume.  From geometry_guide.txt...
      // The replica's positions are claculated by means of a linear formula.
      // Replication may occur along:
      // 
      // o Cartesian axes (kXAxis,kYAxis,kZAxis)
      // 
      //   The replications, of specified width have coordinates of
      //   form (-width*(nReplicas-1)*0.5+n*width,0,0) where n=0.. nReplicas-1
      //   for the case of kXAxis, and are unrotated.
      // 
      // o Radial axis (cylindrical polar) (kRho)
      // 
      //   The replications are cons/tubs sections, centred on the origin
      //   and are unrotated.
      //   They have radii of width*n+offset to width*(n+1)+offset
      //                      where n=0..nReplicas-1
      // 
      // o Phi axis (cylindrical polar) (kPhi)
      //   The replications are `phi sections' or wedges, and of cons/tubs form
      //   They have phi of offset+n*width to offset+(n+1)*width where
      //   n=0..nReplicas-1
      // 
      pSol = pLV -> GetSolid ();
      pMaterial = pLV -> GetMaterial ();
      G4ThreeVector originalTranslation = pVPV -> GetTranslation ();
      G4RotationMatrix* pOriginalRotation = pVPV -> GetRotation ();
      G4double originalRMin = 0., originalRMax = 0.;
      if (axis == kRho && pSol->GetEntityType() == "G4Tubs") {
	originalRMin = ((G4Tubs*)pSol)->GetInnerRadius();
	originalRMax = ((G4Tubs*)pSol)->GetOuterRadius();
      }
      G4bool visualisable = true;
      for (int n = 0; n < nReplicas; n++) {
	G4ThreeVector translation;  // Null.
	G4RotationMatrix rotation;  // Null - life long enough for visualizing.
	G4RotationMatrix* pRotation = 0;
	switch (axis) {
	default:
	case kXAxis:
	  translation = G4ThreeVector (-width*(nReplicas-1)*0.5+n*width,0,0);
	  break;
	case kYAxis:
	  translation = G4ThreeVector (0,-width*(nReplicas-1)*0.5+n*width,0);
	  break;
	case kZAxis:
	  translation = G4ThreeVector (0,0,-width*(nReplicas-1)*0.5+n*width);
	  break;
	case kRho:
	  if (pSol->GetEntityType() == "G4Tubs") {
	    ((G4Tubs*)pSol)->SetInnerRadius(width*n+offset);
	    ((G4Tubs*)pSol)->SetOuterRadius(width*(n+1)+offset);
	  } else {
	    G4cout <<
	      "G4PhysicalVolumeModel::VisitGeometryAndGetVisReps: WARNING:"
	      "\n  built-in replicated volumes replicated in radius for "
		   << pSol->GetEntityType() <<
	      "-type\n  solids (your solid \""
		   << pSol->GetName() <<
	      "\") are not visualisable."
		   << G4endl;
	    visualisable = false;
	  }
	  break;
	case kPhi:
	  rotation.rotateZ (-(offset+(n+0.5)*width));
	  // Minus Sign because for the physical volume we need the
	  // coordinate system rotation.
	  pRotation = &rotation;
	  break;
	} 
	pVPV -> SetTranslation (translation);
	pVPV -> SetRotation    (pRotation);
	pVPV -> SetCopyNo (n);
	if (visualisable) {
	  DescribeAndDescend (pVPV, requestedDepth, pLV, pSol, pMaterial,
			    theAT, sceneHandler);
	}
      }
      // Restore originals...
      pVPV -> SetTranslation (originalTranslation);
      pVPV -> SetRotation    (pOriginalRotation);
      if (axis == kRho && pSol->GetEntityType() == "G4Tubs") {
	((G4Tubs*)pSol)->SetInnerRadius(originalRMin);
	((G4Tubs*)pSol)->SetOuterRadius(originalRMax);
      }
    }
  }

  return;
}

void G4PhysicalVolumeModel::DescribeAndDescend
(G4VPhysicalVolume* pVPV,
 G4int requestedDepth,
 G4LogicalVolume* pLV,
 G4VSolid* pSol,
 G4Material* pMaterial,
 const G4Transform3D& theAT,
 G4VGraphicsScene& sceneHandler)
{
  // Maintain useful data members...
  fpCurrentPV = pVPV;
  fpCurrentLV = pLV;
  fpCurrentMaterial = pMaterial;

  const G4RotationMatrix objectRotation = pVPV -> GetObjectRotationValue ();
  const G4ThreeVector&  translation     = pVPV -> GetTranslation ();
  G4Transform3D theLT (G4Transform3D (objectRotation, translation));

  // Compute the accumulated transformation...
  // Note that top volume's transformation relative to the world
  // coordinate system is specified in theAT == startingTransformation
  // = fTransform (see DescribeYourselfTo), so first time through the
  // volume's own transformation, which is only relative to its
  // mother, i.e., not relative to the world coordinate system, should
  // not be accumulated.
  G4Transform3D theNewAT (theAT);
  if (fCurrentDepth != 0) theNewAT = theAT * theLT;

  /********************************************************
  G4cout << "G4PhysicalVolumeModel::DescribeAndDescend: "
	 << pVPV -> GetName () << "." << pVPV -> GetCopyNo ();
  G4cout << "\n  theAT: ";
  G4cout << "\n    Rotation: ";
  G4RotationMatrix rotation = theAT.getRotation ();
  G4cout << rotation.thetaX() << ", "
	 << rotation.phiX() << ", "
	 << rotation.thetaY() << ", "
	 << rotation.phiY() << ", "
	 << rotation.thetaZ() << ", "
	 << rotation.phiZ();
  G4cout << "\n    Translation: " << theAT.getTranslation();
  G4cout << "\n  theNewAT: ";
  G4cout << "\n    Rotation: ";
  rotation = theNewAT.getRotation ();
  G4cout << rotation.thetaX() << ", "
	 << rotation.phiX() << ", "
	 << rotation.thetaY() << ", "
	 << rotation.phiY() << ", "
	 << rotation.thetaZ() << ", "
	 << rotation.phiZ();
  G4cout << "\n    Translation: " << theNewAT.getTranslation();
  G4cout << G4endl;
  **********************************************************/

  // Make decision to draw...
  const G4VisAttributes* pVisAttribs = pLV->GetVisAttributes();
  if (!pVisAttribs) pVisAttribs = fpMP->GetDefaultVisAttributes();
  // Beware - pVisAttribs might still be zero - create a temporary default one...
  G4bool visAttsCreated = false;
  if (!pVisAttribs) {
    pVisAttribs = new G4VisAttributes;
    visAttsCreated = true;
  }

  G4bool thisToBeDrawn = true;

  // There are various reasons why this volume
  // might not be drawn...
  G4bool culling = fpMP->IsCulling();
  G4bool cullingInvisible = fpMP->IsCullingInvisible();
  G4bool markedVisible = pVisAttribs->IsVisible();
  G4bool cullingLowDensity = fpMP->IsDensityCulling();
  G4double density = pMaterial->GetDensity();
  G4double densityCut = fpMP -> GetVisibleDensity ();

  // 1) Global culling is on....
  if (culling) {
    // 2) Culling of invisible volumes is on...
    if (cullingInvisible) {
      // 3) ...and the volume is marked not visible...
      if (!markedVisible) thisToBeDrawn = false;
    }
    // 4) Or culling of low density volumes is on...
    if (cullingLowDensity) {
      // 5) ...and density is less than cut value...
      if (density < densityCut) thisToBeDrawn = false;
    }
  }

  if (thisToBeDrawn) {

    // Update path of drawn physical volumes...
    G4int copyNo = fpCurrentPV->GetCopyNo();
    fDrawnPVPath.push_back
      (G4PhysicalVolumeNodeID(fpCurrentPV,copyNo,fCurrentDepth));

    DescribeSolid (theNewAT, pSol, pVisAttribs, sceneHandler);

  }

  // Make decision to draw daughters, if any.  There are various
  // reasons why daughters might not be drawn...

  // First, reasons that do not depend on culling policy...
  G4int nDaughters = pLV->GetNoDaughters();
  G4bool daughtersToBeDrawn = true;
  // 1) There are no daughters...
  if (!nDaughters) daughtersToBeDrawn = false;
  // 2) We are at the limit if requested depth...
  else if (requestedDepth == 0) daughtersToBeDrawn = false;
  // 3) The user has asked that the descent be curtailed...
  else if (fCurtailDescent) daughtersToBeDrawn = false;

  // Now, reasons that depend on culling policy...
  else {
    G4bool culling = fpMP->IsCulling();
    G4bool cullingInvisible = fpMP->IsCullingInvisible();
    G4bool daughtersInvisible = pVisAttribs->IsDaughtersInvisible();
    // Culling of covered daughters request.  This is computed in
    // G4VSceneHandler::CreateModelingParameters() depending on view
    // parameters...
    G4bool cullingCovered = fpMP->IsCullingCovered();
    G4bool surfaceDrawing =
      fpMP->GetDrawingStyle() == G4ModelingParameters::hsr ||
      fpMP->GetDrawingStyle() == G4ModelingParameters::hlhsr;    
    if (pVisAttribs->IsForceDrawingStyle()) {
      switch (pVisAttribs->GetForcedDrawingStyle()) {
      default:
      case G4VisAttributes::wireframe: surfaceDrawing = false; break;
      case G4VisAttributes::solid: surfaceDrawing = true; break;
      }
    }
    G4bool opaque = pVisAttribs->GetColour().GetAlpha() >= 1.;
    // 4) Global culling is on....
    if (culling) {
      // 5) ..and culling of invisible volumes is on...
      if (cullingInvisible) {
	// 6) ...and the mother requests daughters invisible
	if (daughtersInvisible) daughtersToBeDrawn = false;
      }
      // 7) Or culling of covered daughters is requested...
      if (cullingCovered) {
	// 8) ...and surface drawing is operating...
	if (surfaceDrawing) {
	  // 9) ...but only if mother is visible...
	  if (thisToBeDrawn) {
	    // 10) ...and opaque...
	      if (opaque) daughtersToBeDrawn = false;
	  }
	}
      }
    }
  }

  // Vis atts for this volume no longer needed if created...
  if (visAttsCreated) delete pVisAttribs;

  if (daughtersToBeDrawn) {
    for (G4int iDaughter = 0; iDaughter < nDaughters; iDaughter++) {
      G4VPhysicalVolume* pVPV = pLV -> GetDaughter (iDaughter);
      // Descend the geometry structure recursively...
      fCurrentDepth++;
      VisitGeometryAndGetVisReps
	(pVPV, requestedDepth - 1, theNewAT, sceneHandler);
      fCurrentDepth--;
    }
  }

  // Reset for normal descending of next volume at this level...
  fCurtailDescent = false;

  // Pop item from path of drawn physical volumes...
  if (thisToBeDrawn) {
    fDrawnPVPath.pop_back();
  }
}

void G4PhysicalVolumeModel::DescribeSolid
(const G4Transform3D& theAT,
 G4VSolid* pSol,
 const G4VisAttributes* pVisAttribs,
 G4VGraphicsScene& sceneHandler)
{
  sceneHandler.PreAddSolid (theAT, *pVisAttribs);
  if (fpClippingPolyhedron)  // Clip and force polyhedral representation...
    {
      G4Polyhedron clipper(*fpClippingPolyhedron);  // Local copy.
      clipper.Transform(theAT.inverse());

      G4Polyhedron::SetNumberOfRotationSteps (fpMP->GetNoOfSides());
      G4Polyhedron* pClippee = pSol->GetPolyhedron();
      G4Polyhedron::ResetNumberOfRotationSteps ();
      if (pClippee)  // Solid can provide.
	{
	  G4Polyhedron clipped(pClippee->subtract(clipper));
	  if(clipped.IsErrorBooleanProcess())
	    {
	      G4cout <<
 "WARNING: G4PhysicalVolumeModel::DescribeSolid: polyhedron for solid\n  \""
		     << pSol->GetName() <<
 "\" skipped due to error during Boolean processing."
		     << G4endl;
	    }
	  else
	    {
	      clipped.SetVisAttributes(pVisAttribs);
	      sceneHandler.BeginPrimitives(theAT);
	      sceneHandler.AddPrimitive(clipped);
	      sceneHandler.EndPrimitives();
	    }
	}
    }
  else  // Standard treatment...
    {
      pSol -> DescribeYourselfTo (sceneHandler);
      
    }
  sceneHandler.PostAddSolid ();
}

G4bool G4PhysicalVolumeModel::Validate (G4bool warn)
{
  G4VPhysicalVolume* world =
    G4TransportationManager::GetTransportationManager ()
    -> GetNavigatorForTracking () -> GetWorldVolume ();
  // The idea now is to seek a PV with the same name and copy no
  // in the hope it's the same one!!
  if (warn) {
    G4cout << "G4PhysicalVolumeModel::Validate() called." << G4endl;
  }
  G4PhysicalVolumeModel searchModel (world);
  G4PhysicalVolumeSearchScene searchScene
    (&searchModel, fTopPVName, fTopPVCopyNo);
  G4ModelingParameters mp;  // Default modeling parameters for this search.
  mp.SetDefaultVisAttributes(fpMP? fpMP->GetDefaultVisAttributes(): 0);
  searchModel.SetModelingParameters (&mp);
  searchModel.DescribeYourselfTo (searchScene);
  G4VPhysicalVolume* foundVolume = searchScene.GetFoundVolume ();
  if (foundVolume) {
    if (warn) {
      G4cout << "  Volume of the same name and copy number (\""
	     << fTopPVName << "\", copy " << fTopPVCopyNo
	     << ") still exists and is being used."
	"\n  Be warned that this does not necessarily guarantee it's the same"
	"\n  volume you originally specified in /vis/scene/add/."
	     << G4endl;
    }
    fpTopPV = foundVolume;
    CalculateExtent ();
    return true;
  }
  else {
    if (warn) {
      G4cout << "  A volume of the same name and copy number (\""
	     << fTopPVName << "\", copy " << fTopPVCopyNo
	     << ") no longer exists."
	     << G4endl;
    }
    return false;
  }
}

const std::map<G4String,G4AttDef>* G4PhysicalVolumeModel::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
      = G4AttDefStore::GetInstance("G4PhysicalVolumeModel", isNew);
    if (isNew) {
      (*store)["PVol"] =
	G4AttDef("PVol","Physical Volume","Bookkeeping","","G4String");
      (*store)["Copy"] =
	G4AttDef("Copy","Physical Volume Copy No.","Bookkeeping","","G4int");
      (*store)["LVol"] =
	G4AttDef("LVol","Logical Volume","Bookkeeping","","G4String");
      (*store)["Region"] =
	G4AttDef("Region","Cuts Region","Bookkeeping","","G4String");
      (*store)["RootRegion"] =
	G4AttDef("RootRegion","Root Region (0/1 = false/true)","Bookkeeping","","G4bool");
      (*store)["Solid"] =
	G4AttDef("Solid","Solid Name","Bookkeeping","","G4String");
      (*store)["EType"] =
	G4AttDef("EType","Entity Type","Bookkeeping","","G4String");
      (*store)["Material"] =
	G4AttDef("Material","Material Name","Bookkeeping","","G4String");
      (*store)["Density"] =
	G4AttDef("Density","Material Density","Physics","G4BestUnit","G4double");
      (*store)["State"] =
	G4AttDef("State","Material State (enum undefined,solid,liquid,gas)","Bookkeeping","","G4String");
      (*store)["Radlen"] =
	G4AttDef("Radlen","Material Radiation Length","Physics","G4BestUnit","G4double");
    }
    return store;
}

std::vector<G4AttValue>* G4PhysicalVolumeModel::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    std::ostringstream oss;
    values->push_back(G4AttValue("PVol", fpCurrentPV->GetName(),""));
    oss << fpCurrentPV->GetCopyNo();
    values->push_back(G4AttValue("Copy", oss.str(),""));
    values->push_back(G4AttValue("LVol", fpCurrentLV->GetName(),""));
    values->push_back(G4AttValue("Region", fpCurrentLV->GetRegion()->GetName(),""));
    oss.str(""); oss << fpCurrentLV->IsRootRegion();
    values->push_back(G4AttValue("RootRegion", oss.str(),""));
    G4VSolid* pSol = fpCurrentLV->GetSolid();
    values->push_back(G4AttValue("Solid", pSol->GetName(),""));
    values->push_back(G4AttValue("EType", pSol->GetEntityType(),""));
    values->push_back(G4AttValue("Material", fpCurrentMaterial->GetName(),""));
    values->push_back(G4AttValue("Density", G4BestUnit(fpCurrentMaterial->GetDensity(),"Volumic Mass"),""));
    oss.str(""); oss << fpCurrentMaterial->GetState();
    values->push_back(G4AttValue("State", oss.str(),""));
    values->push_back(G4AttValue("Radlen", G4BestUnit(fpCurrentMaterial->GetRadlen(),"Length"),""));
    return values;
}
