import os
import platform
import re
import subprocess
import sys
import shutil
import glob
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext

NAME = "PyLive2D"
VERSION = "1.0"
DESCRIPTION = "Live2D Python SDK"
AUTHOR = "qinyonghang"
AUTHOR_EMAIL = "yonghang.qin@google.com"
URL = "https://github.com/qinyonghang/Live2D-Python.git"
INSTALL_REQUIRES = []


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def get_cmake_version(self):
        try:
            out = subprocess.check_output(["cmake", "--version"])
        except OSError:
            sys.stderr.write(
                "CMake must be installed to build the following extensions: "
                + ", ".join(e.name for e in self.extensions)
            )
            sys.exit(1)
        return re.search(r"cmake version ([0-9.]+)", out.decode()).group(1)

    def run(self):
        cmake_version = self.get_cmake_version()
        if platform.system() == "Windows":
            if cmake_version < "3.12":
                sys.stderr.write("CMake >= 3.12 is required on Windows")
                sys.exit(1)
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            "-DPython3_EXECUTABLE=" + sys.executable,
            "-DCMAKE_BUILD_TYPE=Release",
            "-DBUILD_PYTHON_MODULE=ON",  # Custom flag to indicate building for Python
        ]

        build_args = ["--config", "Release"]
        if platform.system() == "Windows":
            cmake_args += [
                # "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format("Release", extdir),
                # "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_{}={}".format("Release", extdir),
                "-A",
                "x64" if platform.architecture()[0] == "64bit" else "Win32",
            ]
            build_args += ["--parallel"]  # Use --parallel instead of /maxcpucount
        else:
            build_args += ["--", "-j10"]
        build_folder = os.path.abspath(self.build_temp)

        if not os.path.exists(build_folder):
            os.makedirs(build_folder)

        cmake_setup = ["cmake", ext.sourcedir] + cmake_args
        cmake_build = ["cmake", "--build", ".", "--target", NAME] + build_args

        print("Building extension for Python {}".format(sys.version.split("\n", 1)[0]))
        print("Invoking CMake setup: '{}'".format(" ".join(cmake_setup)))
        sys.stdout.flush()
        subprocess.check_call(cmake_setup, cwd=build_folder)
        print("Invoking CMake build: '{}'".format(" ".join(cmake_build)))
        sys.stdout.flush()
        subprocess.check_call(cmake_build, cwd=build_folder)

        pyd_file = os.path.join(build_folder, "Release", f"{NAME}.pyd")
        shutil.copy(pyd_file, extdir)
        print(f"Copying {pyd_file} to ext directory: {extdir}")

        other_files = glob.glob(os.path.join(build_folder, "*.dll"))
        for other_file in other_files:
            shutil.copy(other_file, extdir)
            print(f"Copying {other_file} to ext directory: {extdir}")


setup(
    name=NAME,
    version=VERSION,
    description=DESCRIPTION,
    author=AUTHOR,
    author_email=AUTHOR_EMAIL,
    license="Apache License 2.0",
    url=URL,
    install_requires=INSTALL_REQUIRES,
    ext_modules=[CMakeExtension(NAME, ".")],  # Adjust the source directory
    cmdclass=dict(build_ext=CMakeBuild),
    packages=find_packages(),
    package_data={"": ["*.pyd", "*.so", "*.a", "*.dll"]},
    # include_package_data=True,
    # package_dir={"": "package"},
    keywords=["Live2D", "Cubism SDK", "Cubism SDK for Python"],
)
