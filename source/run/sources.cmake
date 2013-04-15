#------------------------------------------------------------------------------
# sources.cmake
# Module : G4run
# Package: Geant4.src.G4run
#
# Sources description for a library.
# Lists the sources and headers of the code explicitely.
# Lists include paths needed.
# Lists the internal granular and global dependencies of the library.
# Source specific properties should be added at the end.
#
# Generated on : 24/9/2010
#
# $Id$
#
#------------------------------------------------------------------------------

# List external includes needed.
include_directories(${CLHEP_INCLUDE_DIRS})

# List internal includes needed.
include_directories(${CMAKE_SOURCE_DIR}/source/digits_hits/detector/include)
include_directories(${CMAKE_SOURCE_DIR}/source/digits_hits/digits/include)
include_directories(${CMAKE_SOURCE_DIR}/source/digits_hits/hits/include)
include_directories(${CMAKE_SOURCE_DIR}/source/digits_hits/utils/include)
include_directories(${CMAKE_SOURCE_DIR}/source/event/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/biasing/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/magneticfield/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/navigation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/volumes/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPGeometry/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPRandom/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/graphics_reps/include)
include_directories(${CMAKE_SOURCE_DIR}/source/intercoms/include)
include_directories(${CMAKE_SOURCE_DIR}/source/materials/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/cuts/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/decay/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/electromagnetic/utils/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/scoring/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/transportation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/track/include)
include_directories(${CMAKE_SOURCE_DIR}/source/tracking/include)
#TODO ANDREA: TODO REMOVE THIS DEPENDENCY AFTER REFACTORING G4WorkerThread
# Check also granula dependency
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/solids/specific/include)


#
# Define the Geant4 Module.
#
include(Geant4MacroDefineModule)
GEANT4_DEFINE_MODULE(NAME G4run
    HEADERS
        G4AdjointPrimaryGeneratorAction.hh
        G4AdjointSimManager.hh
        G4AdjointSimMessenger.hh
        G4ExceptionHandler.hh
        G4MSSteppingAction.hh
        G4MatScanMessenger.hh
        G4MaterialScanner.hh
	G4PhysicsListHelper.hh
	G4PhysicsListOrderingParameter.hh
        G4Run.hh
        G4RunManager.hh
	G4WorkerRunManager.hh
	G4MTRunManager.hh
        G4RunManagerKernel.hh
        G4WorkerRunManagerKernel.hh
        G4RunMessenger.hh
        G4UserPhysicsListMessenger.hh
        G4UserRunAction.hh
        G4VModularPhysicsList.hh
        G4VPersistencyManager.hh
        G4VPhysicsConstructor.hh
        G4VUserDetectorConstruction.hh
        G4VUserApplication.hh
        G4VUserParallelWorld.hh
        G4VUserPhysicsList.hh
        G4VUserPrimaryGeneratorAction.hh
	G4WorkerThread.hh
	G4WorkerInitialization.hh
	G4VUserWorkerInitialization.hh
        G4VUPLSplitter.hh
        rundefs.hh
    SOURCES
        G4AdjointPrimaryGeneratorAction.cc
        G4AdjointSimManager.cc
        G4AdjointSimMessenger.cc
        G4ExceptionHandler.cc
        G4MSSteppingAction.cc
        G4MatScanMessenger.cc
        G4MaterialScanner.cc
	G4PhysicsListHelper.cc
	G4PhysicsListOrderingParamater.cc
        G4Run.cc
        G4RunManager.cc
	G4WorkerRunManager.cc
	G4MTRunManager.cc
        G4RunManagerKernel.cc
        G4WorkerRunManagerKernel.cc
        G4RunMessenger.cc
        G4UserPhysicsListMessenger.cc
        G4UserRunAction.cc
        G4VModularPhysicsList.cc
        G4VPersistencyManager.cc
        G4VUserDetectorConstruction.cc
        G4VUserParallelWorld.cc
        G4VPhysicsConstructor.cc
        G4VUserPhysicsList.cc
        G4VUserPrimaryGeneratorAction.cc
	G4WorkerThread.cc
	G4WorkerInitialization.cc
	G4VUserWorkerInitialization.cc
    GRANULAR_DEPENDENCIES
        G4cuts
        G4decay
        G4detector
        G4detutils
        G4digits
        G4emutils
        G4event
        G4geombias
        G4geometrymng
        G4globman
        G4graphics_reps
        G4hits
        G4intercoms
        G4magneticfield
        G4materials
        G4navigation
        G4partman
        G4procman
        G4scoring
        G4track
        G4tracking
        G4transportation
        G4volumes
	G4specsolids
    GLOBAL_DEPENDENCIES
        G4digits_hits
        G4event
        G4geometry
        G4global
        G4graphics_reps
        G4intercoms
        G4materials
        G4particles
        G4processes
        G4track
        G4tracking
    LINK_LIBRARIES
)

# List any source specific properties here

