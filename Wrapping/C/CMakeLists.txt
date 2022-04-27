
set(CWRAPPER_NAMESPACE gdcmCWrapper)
string(TOLOWER ${CWRAPPER_NAMESPACE} CWRAPPER_LIBRARY_NAME)

set(CWRAPPER_SRCS
  gdcmCReader.cxx
)

include_directories(
  "${GDCM_BINARY_DIR}/Source/Common"
  "${GDCM_SOURCE_DIR}/Source/Common"
  "${GDCM_SOURCE_DIR}/Source/DataStructureAndEncodingDefinition"
  "${GDCM_SOURCE_DIR}/Source/MediaStorageAndFileFormat"
  "${GDCM_SOURCE_DIR}/Source/DataDictionary"
)

add_library(gdcmCWRAPPER ${CWRAPPER_SRCS})
target_link_libraries(gdcmCWRAPPER gdcmDICT gdcmMSFF)

set_target_properties(gdcmCWRAPPER PROPERTIES ${GDCM_LIBRARY_PROPERTIES})
if(NOT GDCM_INSTALL_NO_LIBRARIES)
  # Runtime
  install(TARGETS gdcmCWRAPPER EXPORT ${GDCM_TARGETS_NAME}
    RUNTIME DESTINATION ${GDCM_INSTALL_BIN_DIR} COMPONENT Runtime
    LIBRARY DESTINATION ${GDCM_INSTALL_LIB_DIR} COMPONENT Runtime ${NAMELINK_SKIP}
    ARCHIVE DESTINATION ${GDCM_INSTALL_LIB_DIR} COMPONENT Development
  )
  #Development
  #INSTALL(TARGETS gdcmCWRAPPER
  #  LIBRARY DESTINATION ${GDCM_INSTALL_LIB_DIR} COMPONENT Development ${NAMELINK_ONLY}
  #)
endif()

if(NOT GDCM_INSTALL_NO_DEVELOPMENT)
  set(header_files ${CMAKE_CURRENT_LIST_DIR}/gdcmCReader.h)
  install(FILES ${header_files}
    DESTINATION ${GDCM_INSTALL_INCLUDE_DIR} COMPONENT Development
  )
endif()

