add_library(akumuli SHARED
    akumuli.cpp
    seriesparser.cpp
    metadatastorage.cpp
    stringpool.cpp
    datetime.cpp
    log_iface.cpp
    util.cpp
    storage2.cpp
    crc32c.cpp
    status_util.cpp
    cursor.cpp
    storage_engine/blockstore.cpp
    storage_engine/volume.cpp
    storage_engine/nbtree.cpp
    storage_engine/compression.cpp
    storage_engine/column_store.cpp
    # query_processing
    query_processing/queryparser.cpp
    queryprocessor.cpp
    queryprocessor_framework.cpp
    hashfnfamily.cpp
    anomalydetector.cpp
    query_processing/anomaly.cpp
    saxencoder.cpp
    query_processing/sax.cpp
    query_processing/paa.cpp
    query_processing/filterbyid.cpp
    query_processing/randomsamplingnode.cpp
    query_processing/spacesaver.cpp
    query_processing/limiter.cpp
    query_processing/queryparser.cpp
)

include_directories(.)

include(CppcheckTargets)
add_cppcheck(akumuli UNUSED_FUNCTIONS STYLE POSSIBLE_ERROR FORCE)

# libakumuli

install(
    TARGETS
        akumuli
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_PREFIX}/lib
)

install(
    FILES ../include/akumuli_version.h
	  ../include/akumuli_config.h
	  ../include/akumuli.h
	  ../include/akumuli_def.h
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/include
)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME "akumuli")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Evgeny Lazin")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Time-series storage engine")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

set(CPACK_PACKAGE_VERSION "0.5.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "5")
set(CPACK_PACKAGE_VERSION_PATCH "1")

include(CPack)

link_directories(${Boost_LIBRARY_DIRS})

if(!APPLE)
target_link_libraries(akumuli2
    sqlite3
    "${LOG4CXX_LIBRARIES}"
    "${APR_LIBRARY}"
    "${APRUTIL_LIBRARY}"
    ${Boost_LIBRARIES}
)
endif(!APPLE)
