# When debugging the partitioning widget, or experimenting, you may
# want to allow unsafe partitioning choices (e.g. doing things to the
# current disk). Set DEBUG_PARTITION_UNSAFE to allow that (it turns off
# some filtering of devices).
option( DEBUG_PARTITION_UNSAFE "Allow unsafe partitioning choices." OFF )
option( DEBUG_PARTITION_LAME   "Unsafe partitioning will error out on exec." ON )

set( _partition_defs )
if( DEBUG_PARTITION_UNSAFE )
    if( DEBUG_PARTITION_LAME )
        list( APPEND _partition_defs DEBUG_PARTITION_LAME )
    endif()
    list( APPEND _partition_defs DEBUG_PARTITION_UNSAFE )
endif()

find_package(ECM ${ECM_VERSION} REQUIRED NO_MODULE)

find_package( KPMcore 4.0 )
set_package_properties(
    KPMcore PROPERTIES
    PURPOSE "For partitioning.v2 module"
)

if ( KPMcore_FOUND )
    find_package( Qt5 REQUIRED DBus )
    find_package( KF5 REQUIRED Config CoreAddons I18n WidgetsAddons )

    include_directories( ${KPMCORE_INCLUDE_DIR} )
    include_directories( ${PROJECT_BINARY_DIR}/src/libcalamaresui )

    calamares_add_plugin( partition.v2
        TYPE viewmodule
        EXPORT_MACRO PLUGINDLLEXPORT_PRO
        SOURCES
            PartitionViewStep.cpp
        UI
        LINK_PRIVATE_LIBRARIES
            kpmcore
            calamaresui
            KF5::CoreAddons
        COMPILE_DEFINITIONS ${_partition_defs}
        SHARED_LIB
    )
else()
    calamares_skip_module( "partition.v2 (missing suitable KPMcore)" )
endif()
