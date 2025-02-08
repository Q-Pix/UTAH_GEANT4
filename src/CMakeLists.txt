
## ---------------------------------------------------------
##  G4_QPIX | src/CMakeLists.txt
##
##  CMake build script.
##   * Author: Justo Martin-Albo
##   * Creation date: 14 Aug 2019
## ---------------------------------------------------------

SET(SRC BaseGeometry.h
        FactoryBase.h
        AnalysisManager.cpp
        GeneratorParticle.cpp
        MCTruthManager.cpp
        MCParticle.cpp
        DiffusionGeometry.cpp
        DiffusionGenerator.cpp
        DetectorConstruction.cpp
        PrimaryGeneration.cpp
        RunAction.cpp
        EventAction.cpp
        SteppingAction.cpp
        TrackingAction.cpp
        TrackingSD.cpp
        TrackingHit.cpp
        OpticalMaterialProperties.cpp
        LiquidArgonProperties.cpp
        MaterialsList.cpp
        PmtHit.cpp
        PmtSD.cpp
        GenericPhotosensor.cpp
        UTA_TPC_Geometry.cpp
        UTA_TPC_ParticleGenerator.cpp
        QPIX_App.cpp
        BatchSession.cpp
        RTDCodeManager.cpp

        )

add_library(${CMAKE_PROJECT_NAME} OBJECT ${SRC})
