#------------------------------------------------------------------------------
# sources.cmake
# Module : G4emstandard
# Package: Geant4.src.G4processes.G4electromagnetic.G4emstandard
#
# Sources description for a library.
# Lists the sources and headers of the code explicitely.
# Lists include paths needed.
# Lists the internal granular and global dependencies of the library.
# Source specific properties should be added at the end.
#
# Generated on : 24/9/2010
#
# $Id: sources.cmake,v 1.4 2010-11-29 17:35:45 bmorgan Exp $
# GEANT4 Tag $Name: not supported by cvs2svn $
#
#------------------------------------------------------------------------------

# List external includes needed.
include_directories(${CLHEP_INCLUDE_DIRS})

# List internal includes needed.
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/navigation/include)
include_directories(${CMAKE_SOURCE_DIR}/source/geometry/volumes/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPGeometry/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPNumerics/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/HEPRandom/include)
include_directories(${CMAKE_SOURCE_DIR}/source/global/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/intercoms/include)
include_directories(${CMAKE_SOURCE_DIR}/source/materials/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/bosons/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/hadrons/barions/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/hadrons/ions/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/hadrons/mesons/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/leptons/include)
include_directories(${CMAKE_SOURCE_DIR}/source/particles/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/cuts/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/electromagnetic/utils/include)
include_directories(${CMAKE_SOURCE_DIR}/source/processes/management/include)
include_directories(${CMAKE_SOURCE_DIR}/source/track/include)

#
# Define the Geant4 Module.
#
include(Geant4MacroDefineModule)
GEANT4_DEFINE_MODULE(NAME G4emstandard
    HEADERS
        G4ASTARStopping.hh
        G4BetheBlochIonGasModel.hh
        G4BetheBlochModel.hh
        G4BetheHeitlerModel.hh
        G4BohrFluctuations.hh
        G4BraggIonGasModel.hh
        G4BraggIonModel.hh
        G4BraggModel.hh
        G4ComptonScattering.hh
        G4CoulombScattering.hh
        G4DipBustGenerator.hh
        G4GammaConversion.hh
        G4GoudsmitSaundersonMscModel.hh
        G4GoudsmitSaundersonTable.hh
        G4HeatedKleinNishinaCompton.hh
        G4ICRU49NuclearStoppingModel.hh
        G4ICRU73QOModel.hh
        G4InitXscPAI.hh
        G4IonCoulombCrossSection.hh
        G4IonCoulombScatteringModel.hh
        G4IonFluctuations.hh
        G4KleinNishinaCompton.hh
        G4KleinNishinaModel.hh
        G4ModifiedTsai.hh
        G4MollerBhabhaModel.hh
        G4MottCoefficients.hh
        G4NuclearStopping.hh
        G4PAIModel.hh
        G4PAIPhotonModel.hh
        G4PAIxSection.hh
        G4PAIySection.hh
        G4PEEffectFluoModel.hh
        G4PEEffectModel.hh
        G4PSTARStopping.hh
        G4PairProductionRelModel.hh
        G4PhotoElectricEffect.hh
        G4SauterGavrilaAngularDistribution.hh
        G4ScreeningMottCrossSection.hh
        G4SeltzerBergerModel.hh
        G4UniversalFluctuation.hh
        G4UrbanMscModel90.hh
        G4UrbanMscModel92.hh
        G4UrbanMscModel93.hh
        G4UrbanMscModel95.hh
        G4UrbanMscModel96.hh
        G4WaterStopping.hh
        G4WentzelOKandVIxSection.hh
        G4WentzelVIModel.hh
        G4WentzelVIRelModel.hh
        G4WentzelVIRelXSection.hh
        G4XrayRayleighModel.hh
        G4alphaIonisation.hh
        G4eBremsstrahlung.hh
	G4eBremParametrizedModel.hh
        G4eBremsstrahlungModel.hh
        G4eBremsstrahlungRelModel.hh
        G4eCoulombScatteringModel.hh
        G4eIonisation.hh
        G4eMultipleScattering.hh
        G4eSingleCoulombScatteringModel.hh
        G4eeToTwoGammaModel.hh
        G4eplusAnnihilation.hh
        G4hCoulombScatteringModel.hh
        G4hIonisation.hh
        G4hMultipleScattering.hh
        G4ionIonisation.hh
    SOURCES
        G4ASTARStopping.cc
        G4BetheBlochIonGasModel.cc
        G4BetheBlochModel.cc
        G4BetheHeitlerModel.cc
        G4BohrFluctuations.cc
        G4BraggIonGasModel.cc
        G4BraggIonModel.cc
        G4BraggModel.cc
        G4ComptonScattering.cc
        G4CoulombScattering.cc
        G4GammaConversion.cc
        G4DipBustGenerator.cc
        G4GoudsmitSaundersonMscModel.cc
        G4GoudsmitSaundersonTable.cc
        G4HeatedKleinNishinaCompton.cc
        G4ICRU49NuclearStoppingModel.cc
        G4ICRU73QOModel.cc
        G4InitXscPAI.cc
        G4IonCoulombCrossSection.cc
        G4IonCoulombScatteringModel.cc
        G4IonFluctuations.cc
        G4KleinNishinaCompton.cc
        G4KleinNishinaModel.cc
        G4ModifiedTsai.cc
        G4MollerBhabhaModel.cc
        G4MottCoefficients.cc
        G4NuclearStopping.cc
        G4PAIModel.cc
        G4PAIPhotonModel.cc
        G4PAIxSection.cc
        G4PAIySection.cc
        G4PEEffectFluoModel.cc
        G4PEEffectModel.cc
        G4PSTARStopping.cc
        G4PairProductionRelModel.cc
        G4PhotoElectricEffect.cc
        G4SauterGavrilaAngularDistribution.cc
        G4ScreeningMottCrossSection.cc
        G4SeltzerBergerModel.cc
        G4UniversalFluctuation.cc
        G4UrbanMscModel90.cc
        G4UrbanMscModel92.cc
        G4UrbanMscModel93.cc
        G4UrbanMscModel95.cc
        G4UrbanMscModel96.cc
        G4WaterStopping.cc
        G4WentzelOKandVIxSection.cc
        G4WentzelVIModel.cc
        G4WentzelVIRelModel.cc
        G4WentzelVIRelXSection.cc
        G4XrayRayleighModel.cc
        G4alphaIonisation.cc
        G4eBremsstrahlung.cc
	G4eBremParametrizedModel.cc
        G4eBremsstrahlungModel.cc
        G4eBremsstrahlungRelModel.cc
        G4eCoulombScatteringModel.cc
        G4eIonisation.cc
        G4eMultipleScattering.cc
        G4eSingleCoulombScatteringModel.cc
        G4eeToTwoGammaModel.cc
        G4eplusAnnihilation.cc
        G4hCoulombScatteringModel.cc
        G4hIonisation.cc
        G4hMultipleScattering.cc
        G4ionIonisation.cc
    GRANULAR_DEPENDENCIES
        G4baryons
        G4bosons
        G4cuts
        G4emutils
        G4geometrymng
        G4globman
        G4hepnumerics
        G4intercoms
        G4ions
        G4leptons
        G4materials
        G4mesons
        G4navigation
        G4partman
        G4procman
        G4track
        G4volumes
    GLOBAL_DEPENDENCIES
        G4geometry
        G4global
        G4intercoms
        G4materials
        G4particles
        G4track
    LINK_LIBRARIES
)

# List any source specific properties here

