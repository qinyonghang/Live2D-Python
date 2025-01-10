import os
from typing import Tuple
from urllib.parse import urlparse
import hashlib
import zipfile
import tarfile
import requests
import shutil
from time import sleep


def extract_file(filepath: str, download_dir: str, source_dir: str, retry=5):
    if os.path.exists(source_dir):
        shutil.rmtree(source_dir)

    if filepath.endswith(".zip"):
        with zipfile.ZipFile(filepath, "r") as ref:
            members = ref.namelist()
            top_dir = os.path.commonpath(members)
            if top_dir:
                ref.extractall(download_dir)
            else:
                ref.extractall(source_dir)
    elif filepath.endswith(".tar.gz") or filepath.endswith(".tgz"):
        with tarfile.open(filepath, "r:gz") as ref:
            members = ref.getmembers()
            top_dir = os.path.commonpath([member.name for member in members])
            if top_dir:
                ref.extractall(download_dir)
            else:
                ref.extractall(source_dir)

    else:
        print(f"不支持的文件格式: {filepath}")
        return False

    if top_dir:
        while retry:
            try:
                os.rename(os.path.join(download_dir, top_dir), source_dir)
                break
            except PermissionError:
                sleep(1)
            print(f"Rename Retrying(retry={retry}) {filepath}...")
            retry -= 1
        if retry == 0:
            print(f"Rename Failed {filepath}")
            return False

    print(f"Successfully extracted {filepath} to {source_dir}")
    return True


class Downloader(object):
    def __init__(self, download_dir: str):
        self.download_dir = download_dir
        self.list = []

        try:
            with open(os.path.join(self.download_dir, "downloader.txt"), "r") as f:
                self.list = [line.strip() for line in f.readlines()]
        except FileNotFoundError:
            pass

    def __call__(
        self,
        url: str,
        url_hash: Tuple[str, str],
        download_name: str,
        source_dir: str,
        retry=5,
    ) -> bool:
        filename = os.path.basename(urlparse(url).path)
        if download_name is None:
            download_name = filename

        _, ext = os.path.splitext(filename)
        if filename.endswith(".tar.gz") and not download_name.endswith(".tar.gz"):
            download_name = download_name + ".tar.gz"
        elif ext and not download_name.endswith(ext):
            download_name = download_name + ext

        filepath = os.path.join(self.download_dir, download_name)
        if url in self.list and os.path.exists(filepath) and os.path.exists(source_dir):
            return True

        while retry > 0:
            if not os.path.exists(filepath):
                response = requests.get(url, stream=True)
                response.raise_for_status()
                os.makedirs(self.download_dir, exist_ok=True)
                with open(filepath, "wb") as f:
                    for chunk in response.iter_content(chunk_size=1024):
                        if chunk:
                            f.write(chunk)

            if url_hash is None:
                break
            _hash, _hash_str = url_hash
            _hash = _hash.lower()
            assert _hash in ["md5", "sha256"]
            with open(filepath, "rb") as f:
                if _hash == "md5" and _hash_str == hashlib.md5(f.read()).hexdigest():
                    break
                if (
                    _hash == "sha256"
                    and _hash_str == hashlib.sha256(f.read()).hexdigest()
                ):
                    break
                os.remove(filepath)
            print(f"Download Retrying(retry={retry}) {url}...")
            retry -= 1

        if retry == 0 or not extract_file(
            filepath, self.download_dir, source_dir, retry=retry
        ):
            return False

        if not url in self.list:
            with open(os.path.join(self.download_dir, "downloader.txt"), "a") as f:
                f.write(url + "\n")

        return True
