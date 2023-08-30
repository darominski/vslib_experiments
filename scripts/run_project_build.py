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


def _parse_arguments() -> (str, str, str, str, str):
    """
    Parses the script arguments and returns absolute paths of the specified
    (possibly relative) paths.

    Returns:
        Tuple with path to, in order, Vitis installation, XSA file, source
    code, project creation directory, and application name.
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

    # Parse the arguments
    args = parser.parse_args()

    # All paths are expected to be absolute in the Tcl script
    vitis_path = os.path.abspath(args.vitisPath)
    xsa_path = os.path.abspath(args.xsaPath)
    source_path = os.path.abspath(args.source)
    target_path = os.path.abspath(args.targetPath)

    def _validate_path(path: str) -> bool:
        """
        Validates that the provided path exists.

        Args:
        path (str): absolute path to be checked.
        Returns:
        True if path exists, False otherwise.
        """
        return os.path.exists(path)

    for path in [vitis_path, xsa_path, source_path]:
        if not os.path.exists(path):
            error_msg = path + " does not exist!"
            logging.error(error_msg)
            exit("Terminated.")

    return vitis_path, xsa_path, source_path, target_path, args.name


def _run_project_creation(vitis_path: str, root_path: str,
                          target_path: str, xsa_path: str,
                          source_path: str, app_name: str) -> None:
    """
    Calls the execution of the project building script as a subprocess.

    Args:
        vitis_path (str): Path to the Vitis framework installation directory
        root_path (str): Path to the location of the project building script
        target_path (str): Path to the location where the Vitis project shall
    be created
        xsa_path (str): Path to the XSA input file location
        source_path (str): Path to the source code to be copied
    """
    # Run the Tcl script containing XSCT commands that will create and build
    # the full Vitis project
    process = subprocess.Popen([os.path.join(vitis_path+'/bin/xsct'),
                                os.path.join(root_path+'/build_project.tcl'),
                                target_path, xsa_path, source_path, app_name],
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


def main():
    """
    This script handles the directory creation and calling of a dedicated Tcl
    script containing all the commands necessary to create and build Vitis
    application project from the provided source code and hardware description.
    """
    logging.basicConfig(level=logging.INFO, format='%(message)s')

    vitis_path, xsa_path, source_path, target_path, app_name = \
        _parse_arguments()

    # Try to create the directory that will house the project
    _create_dir(target_path)

    # move to the newly created directory, but save the original location
    root_path = os.path.abspath(os.getcwd())
    os.chdir(target_path)

    _run_project_creation(
        vitis_path=vitis_path,
        root_path=root_path,
        target_path=target_path,
        xsa_path=xsa_path,
        source_path=source_path,
        app_name=app_name)


if __name__ == '__main__':
    main()
