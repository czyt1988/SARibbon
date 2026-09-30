#!/usr/bin/env python3
"""plan-03 S3.0: build the binding-side include mirror for <SARibbonCore/...>.

The widgets sources compiled by the bindings include forwarding headers whose
content is `#include <SARibbonCore/X.h>`. That directory layout only exists in
the main build-tree sync dir / install tree, so each binding build mirrors the
core headers flat into binding-include/SARibbonCore/ before compiling
(same mechanism as tools/Amalgamate.sh's _amalg_include).
"""
import argparse
import shutil
from pathlib import Path


def build_mirror(repo_root: Path, mirror_dir: Path) -> int:
    core_src = repo_root / "src" / "core"
    if not core_src.is_dir():
        raise SystemExit(f"error: {core_src} not found")
    dst = mirror_dir / "SARibbonCore"
    if dst.exists():
        shutil.rmtree(dst)
    dst.mkdir(parents=True)
    count = 0
    for pattern in ("*.h", "*.hpp"):
        for f in core_src.rglob(pattern):
            if f.name == "SARibbonCoreConfig.h":  # generated into the build tree
                continue
            if "_p." in f.name:
                continue
            shutil.copy2(f, dst / f.name)  # flattened
            count += 1
    # pick up the generated config header if a configured main build tree exists
    for cfg in sorted((repo_root / "build").glob("include/SARibbonCore/SARibbonCoreConfig.h")):
        shutil.copy2(cfg, dst / "SARibbonCoreConfig.h")
        break
    print(f"binding include mirror: {count} headers -> {dst}")
    return count


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--repo-root", default=".", help="repository root")
    ap.add_argument("--mirror-dir", default="binding-include", help="mirror output dir")
    args = ap.parse_args()
    build_mirror(Path(args.repo_root).resolve(), Path(args.mirror_dir).resolve())
