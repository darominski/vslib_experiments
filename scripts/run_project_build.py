# Copyright information header
# CERN 2023
#
import argparse
import os
import subprocess
import logging


def _create_dir(target_path: str) -> None:
    """
    Creates a directory in the requested path.

    Args:
        target_path (str): absolute path where the new directory should be
    created.
    """

    try:
        os.makedirs(target_path)
    except FileExistsError as e:
        if os.path.isdir(target_path):
            logging.info("Directory already exists, moving on.")
        else:
            logging.error("There is an issue creating the directory with"
                          " the chosen name" + str(e))


def _parse_arguments() -> (str, str, str, str, str, int, str, str, str):
    """
    Parses the script arguments and returns absolute paths of the specified
    (possibly relative) paths.

    Returns:
        Tuple with path to, in order: Vitis installation, XSA file, source
    code, project creation directory, application name, CPU ID, path to Bmboot,
    and path to libraries necessary to build the VSlib.
    """
    # setting up the parser for optional script arguments
    parser = argparse.ArgumentParser(
        description='Vitis project generator',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    # Add arguments
    parser.add_argument('-vp', '--vitisPath', type=str, required=True,
                        help='Path to Vitis installation.')
    parser.add_argument('-xsa', '--xsaPath', type=str, required=True,
                        help='Path to the XSA file.')
    parser.add_argument('-s', '--source', type=str, required=True,
                        help='Path to the source code to be turned into'
                        ' a Vitis project.')
    parser.add_argument('-n', '--name', type=str, default='application',
                        required=False,
                        help='Name of the application project (optional).')
    parser.add_argument('-t', '--targetPath', type=str, default='project',
                        required=False,
                        help='Target path to where the Vitis project is to'
                        ' be created (optional).')
    parser.add_argument('-id', '--cpuId', type=int, required=True, help='CPU ID this application targets, 1 for DIOTv2, 2 or 3 for Kria K26.')
    parser.add_argument('-vslib', '--vslibPath', type=str, required=True, help='Path to the VSlib location.')
    parser.add_argument('-bm', '--bmboot', type=str, required=True,
                        help='Path to bmboot directory.')
    parser.add_argument('-libs', '--librariesHome', type=str, required=True, help='Path to the dependencies location.')

    # Parse the arguments
    args = parser.parse_args()

    # All paths are expected to be absolute in the Tcl script
    vitis_path = os.path.abspath(args.vitisPath)
    xsa_path = os.path.abspath(args.xsaPath)
    source_path = os.path.abspath(args.source)
    target_path = os.path.abspath(args.targetPath)
    cpu_id = args.cpuId
    vslib_path = os.path.abspath(args.vslibPath)
    bmboot_path = os.path.abspath(args.bmboot)
    libraries_path = os.path.abspath(args.librariesHome)

    return vitis_path, xsa_path, source_path, target_path, args.name, cpu_id, vslib_path, bmboot_path, libraries_path


def _run_project_creation(vitis_path: str, root_path: str,
                          target_path: str, xsa_path: str,
                          source_path: str, app_name: str,
                          cpu_id: str,
                          bmboot_include: str, bmboot_binary: str,
                          libraries_path: str, vslib_build_path: str) -> None:
    """
    Calls the execution of the project building script as a subprocess.

    Args:
        vitis_path (str): Path to the Vitis framework installation directory
        root_path (str): Path to the location of the project building script
        target_path (str): Path to the location where the Vitis project shall
    be created
        xsa_path (str): Path to the XSA input file location
        source_path (str): Path to the source code to be copied
        app_name (str): Name for the application project
        cpu_id (int): CPU ID for the platform project
        bmboot_include (str): Path to the bmboot include
        bmboot_binary_include (str): Path to the bmboot static library binary
        libraries_path (str): Path to the VSlib dependencies
        vslib_build_path (str): Path to the location of the vslib build
    """
    # Run the Tcl script containing XSCT commands that will create and build
    # the full Vitis project
    process = subprocess.Popen([os.path.join(vitis_path+'/bin/xsct'),
                                os.path.join(root_path+'/build_project.tcl'),
                                root_path, target_path, xsa_path, source_path, app_name, cpu_id, bmboot_include, bmboot_binary,
                                libraries_path, vslib_build_path],
                               stdout=subprocess.PIPE, universal_newlines=True)

    # Read and display the stdout line by line
    for line in process.stdout:
        logging.info(line.rstrip())

    # Wait for the process to finish
    process.wait()

    # Check the return code
    if process.returncode == 0:
        logging.info('Project generation executed successfully.')
    else:
        logging.error('Error during the project generation.')


def _build_vslib(vslib_path: str, libraries_path: str) -> str:

    # run cmake to grab all dependencies of the VSlib software and then run its build to a static library, which can be linked in vloop
    build_dir = 'build-vslib'
    # first, configure the cmake
    configure_process = subprocess.Popen(['cmake',
                                          '-S {}'.format(vslib_path),
                                          '-B {}'.format(build_dir),
                                          '-DCMAKE_C_COMPILER=aarch64-none-elf-gcc',
                                          '-DCMAKE_CXX_COMPILER=aarch64-none-elf-g++',
                                          '-DCMAKE_SIZE=aarch64-none-elf-size',
                                          '-DCMAKE_BUILD_TYPE=Release',
                                          '-DLIBRARIES_HOME={}'.format(libraries_path),
                                          '-DBUILD_TESTS=0'],
                               stdout=subprocess.PIPE, universal_newlines=True)

    # Read and display the stdout line by line
    for line in configure_process.stdout:
        logging.info(line.rstrip())

    # Wait for the process to finish
    configure_process.wait()

    # Check the return code
    if configure_process.returncode == 0:
        logging.info('Project generation executed successfully.')
    else:
        logging.error('Error during the project generation.')

    build_process = subprocess.Popen(['cmake',
                                      '--build',
                                      '{}'.format(build_dir)],
                               stdout=subprocess.PIPE, universal_newlines=True)
    # Read and display the stdout line by line
    for line in build_process.stdout:
        logging.info(line.rstrip())

    # Wait for the process to finish
    build_process.wait()

    # Check the return code
    if build_process.returncode == 0:
        logging.info('Project generation executed successfully.')
    else:
        logging.error('Error during the project generation.')

    return os.path.join(os.path.abspath(os.getcwd()), build_dir)

def main():
    """
    This script handles the directory creation and calling of a dedicated Tcl
    script containing all the commands necessary to create and build Vitis
    application project from the provided source code and hardware description.
    """
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    vitis_path, xsa_path, source_path, target_path, app_name, cpu_id, vslib_path, bmboot_path, libraries_path = \
        _parse_arguments()

    bmboot_include_path = os.path.join(os.path.abspath(bmboot_path), "include")
    bmboot_binary_path = os.path.join(os.path.abspath(bmboot_path), "build-bmboot/monitor_zynqmp-prefix/src/monitor_zynqmp-build")

    # Try to create the directory that will house the project
    _create_dir(target_path)

    # move to the newly created directory, but save the original location
    root_path = os.path.abspath(os.getcwd())

    # change path to the target path and create the project there
    os.chdir(target_path)

    # run CMake for VSlib
    vslib_build_path = _build_vslib(vslib_path, libraries_path)

    # create the Vitis project
    _run_project_creation(
        vitis_path=vitis_path,
        root_path=root_path,
        target_path=target_path,
        xsa_path=xsa_path,
        source_path=source_path,
        app_name=app_name,
        cpu_id=str(cpu_id),
        bmboot_include=bmboot_include_path,
        bmboot_binary=bmboot_binary_path,
        libraries_path=libraries_path,
        vslib_build_path=vslib_build_path
    )


if __name__ == '__main__':
    main()
