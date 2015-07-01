cmake_minimum_required(VERSION 2.8)

project(libReconstructionUnitTest)
set(TEST_SOURCE_DIR "src/unit")
INCLUDE(CMakeTests.txt)

project(libReconstructionIntegrationTest)
set(TEST_SOURCE_DIR "src/integration")
INCLUDE(CMakeTests.txt)
