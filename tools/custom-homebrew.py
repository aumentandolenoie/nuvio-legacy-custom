#!/usr/bin/env python3
"""Generate public Homebrew Channel metadata for a built Nuvio Legacy Custom IPK.

Only generates files; it never publishes, installs or contacts the TV.
Publish the IPK, repo.json, webosbrew.manifest.json on the same GitHub Release.
"""
import argparse
import hashlib
import io
import json
from pathlib import Path
import subprocess
import tarfile

APP_ID = "space.nuvio.native.legacy.custom"

def appinfo_from_ipk(ipk):
    data = subprocess.check_output(["ar", "p", str(ipk), "data.tar.gz"])
    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as archive:
        matches = [m for m in archive.getmembers() if m.name.endswith("/appinfo.json")]
        if len(matches) != 1:
            raise ValueError("IPK must contain exactly one appinfo.json")
        return json.load(archive.extractfile(matches[0]))

def main():
    p = argparse.ArgumentParser()
    p.add_argument("ipk", type=Path)
    p.add_argument("--repository", default="aumentandolenoie/nuvio-legacy-custom")
    p.add_argument("--tag", required=True, help="GitHub Release tag, e.g. v2.0.5-custom.1")
    p.add_argument("--output", type=Path, default=Path("build/homebrew-custom"))
    opts = p.parse_args()

    if opts.repository.count("/") != 1 or not all(opts.repository.split("/")):
        p.error("--repository must be OWNER/REPOSITORY")
    if "/" in opts.tag or not opts.tag:
        p.error("--tag must be a single GitHub tag name")
    ipk = opts.ipk.resolve()
    if not ipk.is_file():
        p.error("IPK does not exist")
    app = appinfo_from_ipk(ipk)
    if app.get("id") != APP_ID or app.get("title") != "Nuvio Legacy Custom":
        p.error("This IPK is not the separately packaged Nuvio Legacy Custom app")

    digest = hashlib.sha256(ipk.read_bytes()).hexdigest()
    release = f"https://github.com/{opts.repository}/releases/download/{opts.tag}"
    icon = f"https://raw.githubusercontent.com/{opts.repository}/master/deploy/app/icon-large-20.png"
    package_manifest = {
        "id": APP_ID,
        "version": app["version"],
        "type": "native",
        "title": "Nuvio Legacy Custom",
        "appDescription": "Nuvio Legacy Custom with categorized StreamViX TV Guide",
        "iconUri": icon,
        "sourceUrl": f"https://github.com/{opts.repository}",
        "rootRequired": False,
        "ipkUrl": f"{release}/{ipk.name}",
        "ipkHash": {"sha256": digest},
        "ipkSize": ipk.stat().st_size,
    }
    index = {
        "paging": {"page": 1, "count": 1, "maxPage": 1, "itemsTotal": 1,
                   "prevUrl": None, "nextUrl": None},
        "packages": [{
            "id": APP_ID,
            "title": package_manifest["title"],
            "iconUri": icon,
            "manifestUrl": f"{release}/webosbrew.manifest.json",
            "manifest": package_manifest,
            "pool": "main",
            "shortDescription": package_manifest["appDescription"],
        }],
    }
    opts.output.mkdir(parents=True, exist_ok=True)
    (opts.output / "webosbrew.manifest.json").write_text(
        json.dumps(package_manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    (opts.output / "repo.json").write_text(
        json.dumps(index, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    (opts.output / (ipk.name + ".sha256")).write_text(
        digest + "  " + ipk.name + "\n", encoding="utf-8")

    print("Generated: " + str(opts.output / "repo.json"))
    print("Upload these files to the same PUBLIC GitHub Release:")
    print("  - " + str(ipk))
    print("  - " + str(opts.output / "repo.json"))
    print("  - " + str(opts.output / "webosbrew.manifest.json"))
    print("Homebrew Channel repository, only AFTER publishing:")
    print(f"https://github.com/{opts.repository}/releases/latest/download/repo.json")

if __name__ == "__main__":
    main()
