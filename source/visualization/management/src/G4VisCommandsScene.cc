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
//
// $Id: G4VisCommandsScene.cc,v 1.21 2001-08-05 19:02:22 johna Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

// /vis/scene commands - John Allison  9th August 1998

#include "G4VisCommandsScene.hh"

#include "G4VisManager.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalVolumeModel.hh"
#include "G4ApplicationState.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ios.hh"
#include "g4std/strstream"

G4VVisCommandScene::G4VVisCommandScene () {}

G4VVisCommandScene::~G4VVisCommandScene () {}

G4String G4VVisCommandScene::CurrentSceneName () {
  const G4Scene* pScene = fpVisManager -> GetCurrentScene ();
  G4String currentSceneName;
  if (pScene) currentSceneName = pScene -> GetName ();
  return currentSceneName;
}

void G4VVisCommandScene::UpdateCandidateLists () {

  G4SceneList& sceneList = fpVisManager -> SetSceneList ();
  G4int iScene, nScenes = sceneList.size ();
  G4String nameList;
  for (iScene = 0; iScene < nScenes; iScene++) {
    nameList += sceneList [iScene] -> GetName () + " ";
  }
  nameList = nameList.strip ();
  sceneNameCommandsIterator i;
  for (i = sceneNameCommands.begin (); i != sceneNameCommands.end (); ++i) {
    (*i)->GetParameter (0) -> SetParameterCandidates (nameList);
  }
}

void G4VVisCommandScene::UpdateVisManagerScene
(const G4String& sceneName) {

  G4SceneList& sceneList = fpVisManager -> SetSceneList ();

  G4int iScene, nScenes = sceneList.size ();
  for (iScene = 0; iScene < nScenes; iScene++) {
    if (sceneList [iScene] -> GetName () == sceneName) break;
  }

  G4Scene* pScene = 0;  // Zero unless scene has been found...
  if (iScene < nScenes) {
    pScene = sceneList [iScene];
  }
  fpVisManager -> SetCurrentScene (pScene);

  G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");
}

////////////// /vis/scene/create ///////////////////////////////////////

G4VisCommandSceneCreate::G4VisCommandSceneCreate (): fId (0) {
  G4bool omitable, currentAsDefault;
  fpCommand = new G4UIcmdWithAString ("/vis/scene/create", this);
  fpCommand -> SetGuidance ("/vis/scene/create [<scene-name>]");
  fpCommand -> SetGuidance ("Creates an empty scene.");
  fpCommand -> SetGuidance ("Invents a name if not supplied.");
  fpCommand -> SetGuidance ("This scene becomes current.");
  fpCommand -> SetParameterName ("scene-name",
				 omitable = true,
				 currentAsDefault = true);
  sceneNameCommands.push_back (fpCommand);
}

G4VisCommandSceneCreate::~G4VisCommandSceneCreate () {
  delete fpCommand;
}

G4String G4VisCommandSceneCreate::NextName () {
  char nextName [20];
  G4std::ostrstream ost (nextName, 20);
  ost << "scene-" << fId  << G4std::ends;
  return nextName;
}

G4String G4VisCommandSceneCreate::GetCurrentValue (G4UIcommand* command) {
  return NextName ();
}

void G4VisCommandSceneCreate::SetNewValue (G4UIcommand* command,
					   G4String newValue) {

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();

  G4String& newName = newValue;
  G4String nextName = NextName ();

  if (newName == "") {
    newName = nextName;
  }
  if (newName == nextName) fId++;

  G4SceneList& sceneList = fpVisManager -> SetSceneList ();
  G4int iScene, nScenes = sceneList.size ();
  for (iScene = 0; iScene < nScenes; iScene++) {
    if (sceneList [iScene] -> GetName () == newName) break;
  }
  if (iScene < nScenes) {
    if (verbosity >= G4VisManager::warnings) {
      G4cout << "WARNING: Scene \"" << newName << "\" already exists."
	     << G4endl;
    }
  }
  else {

    sceneList.push_back (new G4Scene (newName));
    // Adds empty scene data object to list.

    UpdateVisManagerScene (newName);
    if (verbosity >= G4VisManager::confirmations) {
      G4cout << "New empty scene \"" << newName << "\" created." << G4endl;
    }

    UpdateCandidateLists ();
  }
}

////////////// /vis/scene/edit ///////////////////////////////////////

G4VisCommandSceneEdit::G4VisCommandSceneEdit () {
  G4bool omitable, currentAsDefault;
  fpCommand = new G4UIcmdWithAString ("/vis/scene/edit", this);
  fpCommand -> SetGuidance ("/vis/scene/edit [<scene-name>]");
  fpCommand -> SetGuidance ("Edits a scene (default current scene).");
  fpCommand -> SetGuidance ("This scene becomes current.");
  fpCommand -> SetParameterName ("scene-name",
				 omitable = true,
				 currentAsDefault = true);
  sceneNameCommands.push_back (fpCommand);
}

G4VisCommandSceneEdit::~G4VisCommandSceneEdit () {
  delete fpCommand;
}

G4String G4VisCommandSceneEdit::GetCurrentValue (G4UIcommand* command) {
  return CurrentSceneName ();
}

void G4VisCommandSceneEdit::SetNewValue (G4UIcommand* command,
					   G4String newValue) {

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();

  G4String sceneName;
  G4std::istrstream is ((char*)newValue.data());
  is >> sceneName;

  G4SceneList& sceneList = fpVisManager -> SetSceneList ();
  G4int iScene, nScenes = sceneList.size ();
  G4Scene* pScene;
  for (iScene = 0; iScene < nScenes; iScene++) {
    pScene = sceneList [iScene];
    if (pScene -> GetName () == sceneName) break;
  }
  if (iScene >= nScenes) {
    if (verbosity >= G4VisManager::warnings) {
      G4cout << "WARNING: G4VisCommandSceneEdit::SetNewValue: Scene \""
	     << sceneName << "\" not found."
	"\n  /vis/scene/list to see scenes."
	     << G4endl;
    }
    return;
  }
  else {
    if (verbosity >= G4VisManager::confirmations) {
      G4cout << "Scene \"" << sceneName << "\" contains:" << G4endl;
      G4String uiCommand ("/vis/scene/list ");
      uiCommand += sceneName;
      uiCommand += " confirmations";
      G4UImanager::GetUIpointer () -> ApplyCommand (uiCommand);
      G4cout <<
      "BUT...YOU CAN DO NOTHING YET - /vis/scene/edit FACILITY IN PREPARATION."
	     << G4endl;
    }
  }

  UpdateVisManagerScene (sceneName);
}

////////////// /vis/scene/list ///////////////////////////////////////

G4VisCommandSceneList::G4VisCommandSceneList () {
  G4bool omitable;
  fpCommand = new G4UIcommand ("/vis/scene/list", this);
  fpCommand -> SetGuidance ("/vis/scene/list [<scene-name>] [<verbosity>]");
  fpCommand -> SetGuidance ("Lists scene(s).");
  fpCommand -> SetGuidance ("<scene-name> default is \"all\"");
  fpCommand -> SetGuidance
    ("See /vis/verbose for definition of verbosity.");
  G4UIparameter* parameter;
  parameter = new G4UIparameter ("scene-name", 's',
				 omitable = true);
  parameter -> SetCurrentAsDefault (false);
  parameter -> SetDefaultValue ("all");
  fpCommand -> SetParameter (parameter);
  parameter = new G4UIparameter ("verbosity", 's',
				 omitable = true);
  parameter -> SetCurrentAsDefault (false);
  parameter -> SetDefaultValue (0);
  fpCommand -> SetParameter (parameter);
  sceneNameCommands.push_back (fpCommand);
}

G4VisCommandSceneList::~G4VisCommandSceneList () {
  delete fpCommand;
}

G4String G4VisCommandSceneList::GetCurrentValue (G4UIcommand* command) {
  return "";
}

void G4VisCommandSceneList::SetNewValue (G4UIcommand* command,
					 G4String newValue) {
  G4String name, verbosityString;
  G4std::istrstream is ((char*)newValue.data());
  is >> name >> verbosityString;
  G4VisManager::Verbosity verbosity =
    fpVisManager->GetVerbosityValue(verbosityString);

  G4SceneList& sceneList = fpVisManager -> SetSceneList ();
  G4int iScene, nScenes = sceneList.size ();
  G4bool found = false;
  for (iScene = 0; iScene < nScenes; iScene++) {
    G4Scene* pScene = sceneList [iScene];
    const G4String& iName = pScene -> GetName ();
    if (name != "all") {
      if (name != iName) continue;
    }
    found = true;
    G4cout << "Scene name: \"" << iName << "\"";
    if (verbosity >= G4VisManager::confirmations) {
      G4int i;
      G4cout << "\n  Run-duration models:";
      G4int nRunModels = pScene -> GetRunDurationModelList ().size ();
      if (nRunModels == 0) {
	G4cout << " none.";
      }
      for (i = 0; i < nRunModels; i++) {
	G4VModel* pModel = pScene -> GetRunDurationModelList () [i];
	G4cout << "\n    " << pModel -> GetGlobalDescription ();
      }
      G4cout << "\n  End-of-event models:";
      G4int nEOEModels = pScene -> GetEndOfEventModelList ().size ();
      if (nEOEModels == 0) {
	G4cout << " none.";
      }
      for (i = 0; i < nEOEModels; i++) {
	G4VModel* pModel = pScene -> GetEndOfEventModelList () [i];
	G4cout << "\n    " << pModel -> GetGlobalDescription ();
      }
    }
    if (verbosity >= G4VisManager::parameters) {
      G4cout << "\n  " << *sceneList [iScene];
    }
    G4cout << G4endl;
  }
  if (!found) {
    G4cout << "No scenes found";
    if (name != "all") {
      G4cout << " of name \"" << name << "\"";
    }
    G4cout << "." << G4endl;
  }
}

////////////// /vis/scene/notifyHandlers /////////////////////////

G4VisCommandSceneNotifyHandlers::G4VisCommandSceneNotifyHandlers () {
  G4bool omitable, currentAsDefault;
  fpCommand = new G4UIcmdWithAString ("/vis/scene/notifyHandlers", this);
  fpCommand -> SetGuidance ("/vis/scene/notifyHandlers [<scene-name>]");
  fpCommand -> SetGuidance
    ("Notifies scene handlers of possible changes of scene.");
  fpCommand -> SetGuidance ("<scene-name> default is current scene name.");
  fpCommand -> SetGuidance
    ("This command does not change current scene, scene handler or viewer.");
  fpCommand -> SetParameterName ("scene-name",
				 omitable = true,
				 currentAsDefault = true);
  sceneNameCommands.push_back (fpCommand);
}

G4VisCommandSceneNotifyHandlers::~G4VisCommandSceneNotifyHandlers () {
  delete fpCommand;
}

G4String G4VisCommandSceneNotifyHandlers::GetCurrentValue
(G4UIcommand* command) {
  return CurrentSceneName ();
}

void G4VisCommandSceneNotifyHandlers::SetNewValue (G4UIcommand* command,
						   G4String newValue) {

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();
  G4bool warn(verbosity >= G4VisManager::warnings);

  G4String& sceneName = newValue;
  const G4SceneList& sceneList = fpVisManager -> GetSceneList ();
  G4SceneHandlerList& sceneHandlerList =
    fpVisManager -> SetAvailableSceneHandlers ();

  // Check scene name.
  G4bool successful(true);
  const G4int nScenes = sceneList.size ();
  G4int iScene;
  for (iScene = 0; iScene < nScenes; iScene++) {
    G4Scene* scene = sceneList [iScene];
    if (sceneName == scene -> GetName ()) {
      successful = scene -> AddWorldIfEmpty (warn);
      break;
    }
  }
  if (iScene >= nScenes ) {
    if (verbosity >= G4VisManager::warnings) {
      G4cout << "WARNING: Scene \"" << sceneName << "\" not found."
	"\n  /vis/scene/list to see scenes."
	     << G4endl;
    }
    return;
  }
  if (!successful) {
    if (verbosity >= G4VisManager::errors) {
      G4cout <<
	"ERROR: Scene is empty.  Perhaps no geometry exists."
	"\n  Try /run/initialize."
	     << G4endl;
    }
    return;
  }

  // For each scene handler, if it contains the scene, clear and
  // rebuild the graphical database, then for each viewer set (make
  // current), clear, (re)draw, and show.
  const G4int nSceneHandlers = sceneHandlerList.size ();
  for (G4int iSH = 0; iSH < nSceneHandlers; iSH++) {
    G4VSceneHandler* aSceneHandler = sceneHandlerList [iSH];
    G4Scene* aScene = aSceneHandler -> GetScene ();
    if (aScene) {
      const G4String& aSceneName = aScene -> GetName ();
      if (sceneName == aSceneName) {
	// Clear store and force a rebuild...
	aSceneHandler -> ClearStore ();
	aSceneHandler -> ClearTransientStore ();
	G4ViewerList& viewerList = aSceneHandler -> SetViewerList ();
	const G4int nViewers = viewerList.size ();
	for (G4int iV = 0; iV < nViewers; iV++) {
	  G4VViewer* aViewer = viewerList [iV];
	  aViewer -> ClearView ();
	  aViewer -> SetView ();
	  aViewer -> DrawView ();
	  // Triggers rebuild of graphical database by notifying the scene
	  // handler.  The viewer is supposed to be smart enough to know
	  // when not to do this.  E.g., the second viewer of a scene
	  // handler does not do it.
	  if (verbosity >= G4VisManager::confirmations) {
	    G4cout << "Viewer \"" << aViewer -> GetName ()
		   << "\" of scene handler \"" << aSceneHandler -> GetName ()
		   << "\"\n  refreshed at request of scene \"" << sceneName
		   << "\"." << G4endl;
	  }
	}
      }
    }
    else {
      if (verbosity >= G4VisManager::warnings) {
	G4cout << "WARNING: G4VisCommandSceneNotifyHandlers: scene handler \""
	       << aSceneHandler->GetName()
	       << "\" has a null scene."
	       << G4endl;
      }
    }
  }
}

////////////// /vis/scene/remove ///////////////////////////////////////

G4VisCommandSceneRemove::G4VisCommandSceneRemove () {
  G4bool omitable, currentAsDefault;
  fpCommand = new G4UIcmdWithAString ("/vis/scene/remove", this);
  fpCommand -> SetGuidance ("/vis/scene/remove <scene-name>");
  fpCommand -> SetGuidance ("Removes scenes.");
  fpCommand -> SetGuidance
    ("/vis/scene/list to see possibilities.");
  fpCommand -> SetParameterName ("scene-name",
				 omitable = false,
				 currentAsDefault = true);
  sceneNameCommands.push_back (fpCommand);
}

G4VisCommandSceneRemove::~G4VisCommandSceneRemove () {
  delete fpCommand;
}

G4String G4VisCommandSceneRemove::GetCurrentValue (G4UIcommand* command) {
  return CurrentSceneName ();
}

void G4VisCommandSceneRemove::SetNewValue (G4UIcommand* command,
					   G4String newValue) {

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();

  G4String& removeName = newValue;
  const G4String& currentSceneName =
    fpVisManager -> GetCurrentScene () -> GetName ();
  G4SceneList& sceneList = fpVisManager -> SetSceneList ();
  G4SceneListIterator iScene;
  for (iScene = sceneList.begin(); iScene != sceneList.end(); ++iScene) {
    if ((*iScene) -> GetName () == removeName) break;
  }
  if (iScene != sceneList.end()) {
    delete *iScene;
    sceneList.erase (iScene);
    if (verbosity >= G4VisManager::confirmations) {
      G4cout << "Scene \"" << removeName << "\" removed." << G4endl;
    }
    UpdateCandidateLists ();
    if (sceneList.empty ()) {
      UpdateVisManagerScene ();
      if (verbosity >= G4VisManager::warnings) {
	G4cout << "WARNING: No scenes left.  Please create a new one."
	       << G4endl;
      }
    }
    else {
      if (currentSceneName == removeName) {
	UpdateVisManagerScene (sceneList [0] -> GetName ());
	if (verbosity >= G4VisManager::warnings) {
	  G4cout << "WARNING: Current scene is now \""
		 << fpVisManager -> GetCurrentScene () -> GetName ()
		 << "\"" << G4endl;
	}
      }
      else {
	if (verbosity >= G4VisManager::confirmations) {
	  G4cout << "Current scene unchanged." << G4endl;
	}
      }
    }
  }
  else {
    if (verbosity >= G4VisManager::warnings) {
      G4cout << "WARNING: Scene \"" << removeName
	     << "\" not found - \"/vis/scene/list\" to see possibilities."
	     << G4endl;
    }
  }
}

////////////// /vis/scene/select ///////////////////////////////////////

G4VisCommandSceneSelect::G4VisCommandSceneSelect () {
  G4bool omitable, currentAsDefault;
  fpCommand = new G4UIcmdWithAString ("/vis/scene/select", this);
  fpCommand -> SetGuidance ("/vis/scene/select [<scene-name>]");
  fpCommand -> SetGuidance ("Selects current scene.");
  fpCommand -> SetGuidance
    ("Specify scene by name (\"/vis/scene/list\" to see possibilities).");
  fpCommand -> SetParameterName ("scene-name",
				 omitable = true,
				 currentAsDefault = true);
  sceneNameCommands.push_back (fpCommand);
}

G4VisCommandSceneSelect::~G4VisCommandSceneSelect () {
  delete fpCommand;
}

G4String G4VisCommandSceneSelect::GetCurrentValue (G4UIcommand* command) {
  return CurrentSceneName ();
}

void G4VisCommandSceneSelect::SetNewValue (G4UIcommand* command,
					   G4String newValue) {

  G4VisManager::Verbosity verbosity = fpVisManager->GetVerbosity();

  G4String& selectName = newValue;
  G4SceneList& sceneList = fpVisManager -> SetSceneList ();
  G4int iScene, nScenes = sceneList.size ();
  for (iScene = 0; iScene < nScenes; iScene++) {
    if (sceneList [iScene] -> GetName () == selectName) break;
  }
  if (iScene < nScenes) {
    UpdateVisManagerScene (selectName);
    if (verbosity >= G4VisManager::confirmations) {
      G4cout << "Scene \"" << selectName
	     << "\" selected." << G4endl;
    }
  }
  else {
    if (verbosity >= G4VisManager::warnings) {
      G4cout << "WARNING: Scene \"" << selectName
	     << "\" not found - \"/vis/scene/list\" to see possibilities."
	     << G4endl;
    }
  }
}
