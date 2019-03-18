otb_create_application(
  NAME           TotalWeight
  SOURCES        src/TotalWeight.cpp src/TotalWeightComputation.cpp
  LINK_LIBRARIES MuscateMetadata MetadataHelper ${OTB_LIBRARIES})

target_include_directories(otbapp_TotalWeight PUBLIC include)
install(TARGETS otbapp_TotalWeight DESTINATION lib/otb/applications/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()