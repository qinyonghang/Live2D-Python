import argparse
import os
import platform
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.downloader import Downloader


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--url", type=str)
    parser.add_argument("--url_hash", type=str, default=None)
    parser.add_argument("--download_dir", type=str)
    parser.add_argument("--download_name", type=str, default=None)
    parser.add_argument("--source_dir", type=str)
    parser.add_argument("--build_dir", type=str, default=None)
    parser.add_argument("--install_dir", type=str)
    parser.add_argument("--build_type", type=str, default="release")
    parser.add_argument("--cmakelists_dir", type=str, default=None)
    parser.add_argument("--cmake_args", type=str, default=None)
    parser.add_argument("--custom_compile", nargs="+", default=None)
    parser.add_argument("--skip_compile", action="store_true")
    parser.add_argument("--before_compile", nargs="+", default=None)
    parser.add_argument("--after_compile", nargs="+", default=None)
    parser.add_argument("--working_thread", type=int, default=4)
    return parser.parse_args()


if __name__ == "__main__":
    args = get_args()

    _hash, _hash_str = args.url_hash.split(":")

    downloader = Downloader(args.download_dir)
    ok = downloader(args.url, (_hash, _hash_str), args.download_name, args.source_dir)
    if not ok:
        print("Download failed!")
        exit(1)

    if args.skip_compile:
        exit(0)

    os.chdir(args.source_dir)

    if args.before_compile is not None:
        for cmd in args.before_compile:
            os.system(cmd)

    if args.custom_compile is not None:
        for cmd in args.custom_compile:
            os.system(cmd)
    else:
        if args.cmakelists_dir is None:
            args.cmakelists_dir = args.source_dir

        if args.build_dir is None:
            args.build_dir = os.path.join(args.source_dir, "build")

        build_type = args.build_type.capitalize()
        configure_command = f"cmake -S {args.cmakelists_dir} -B {args.build_dir} -DCMAKE_INSTALL_PREFIX={args.install_dir} -DCMAKE_BUILD_TYPE={build_type}"
        if args.cmake_args is not None:
            configure_command += f" {args.cmake_args}"
        os.system(configure_command)

        if platform.system() == "Windows":
            os.system(
                f"cmake --build {args.build_dir} --config {build_type} --target INSTALL --parallel {args.working_thread}"
            )
        elif platform.system() == "Linux":
            os.system(f"cmake --build {args.build_dir} -j{args.working_thread}")
            os.system(f"cmake --install {args.build_dir}")

    if args.after_compile is not None:
        for cmd in args.after_compile:
            os.system(cmd)
