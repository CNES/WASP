otb_create_application(
  NAME           ProductFormatter
  SOURCES        src/ProductFormatter.cpp
				 src/ProductCreatorAdapter.cpp  
  				 src/ProductCreatorSentinelMuscate.cpp
   				 src/ProductCreatorVenusMuscate.cpp
  				 
  LINK_LIBRARIES ${OTB_LIBRARIES} MuscateMetadata MetadataHelper)

target_include_directories(otbapp_ProductFormatter PUBLIC include)
install(TARGETS otbapp_ProductFormatter DESTINATION lib/otb/applications)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()