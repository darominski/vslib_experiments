# Vitis workspace creation procedure
proc CreateWorkspace {workspace_dir} {
    setws -switch $workspace_dir
}

# Vitis platform project creation procedure
proc CreatePlatformProject {xsa_path proc_name workspace_dir} {
    set plat_name "diot_a53_1"
    set project_domain "cortex_a53_1"

    # The main command to create the platform based on DI/OT v2 board specification
    # It uses fixed-location XSA file, standalone (bare-metal) operating system,
    # places the FSBL on the _0 processor (by default), and 64bit architecture (default),
    # and puts the project in its own subdirectory.
    # The FSBL target and architecture can be specified in V2023.1 by:
    # -arch {64-bit} -fsbl-target {psu_cortexa53_0}
    #
    # As there are only 2 Cortexa A53 cores, the VSlib takes the 2nd (_1) bare metal one
    # tn the FGC4, with 4 A53 cores, it will be the 4th (_3).
    if (![file exists $plat_name]) {
	# platform project is only generated once even if there are multiple application projects
	platform create -name $plat_name\
	    -hw $xsa_path\
	    -proc $proc_name\
	    -os "standalone"\
	    -out $workspace_dir

	# necessary for the platform project to appear in Vitis:
	importproject $workspace_dir
	# -domains ensures that only the specified domain will be built
	platform generate -domains "standalone_domain"
    }
    platform active $plat_name
}

# Vitis custom application project creation procedure
proc CreateApplicationProject {app_name proc_name source_path workspace_dir} {
    # creates empty application project connected to the active bare-metal domain
    app create -name $app_name\
	-proc $proc_name\
	-domain "standalone_domain"\
	-os "standalone"\
	-lang "C++"\
	-template "Empty Application"

    # fills the project with the code from the cloned repository
    importsources -name $app_name -path $source_path -soft-link

    # configures correctly the location of the include files for both Release and Debug builds
    set builds {release debug}
    foreach build $builds {
	app config -name $app_name -set build-config $build
	app config -name $app_name -add "include-path" $source_path/inc
	app config -name $app_name -add "include-path" $source_path/lib
	app config -name $app_name -add "library-search-path" $source_path/lib
    }

    # builds the application
    app build -name $app_name
}

set workspace_dir [lindex $argv 0]
set xsa_path [lindex $argv 1]
set source_path [lindex $argv 2]
set app_name [lindex $argv 3]

set proc_name "psu_cortexa53_1"

# Vitis workspace creation
CreateWorkspace $workspace_dir
cd $workspace_dir

# Platform project creation
CreatePlatformProject $xsa_path $proc_name $workspace_dir

# Application project creation and build
CreateApplicationProject $app_name $proc_name $source_path $workspace_dir
