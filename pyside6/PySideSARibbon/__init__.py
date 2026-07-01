import os
import sys
from pathlib import Path

# Ensure PySide6 DLLs are on the path before importing saribbon
from PySide6.QtCore import Qt  # noqa: F401

# Add PySide6 and shiboken6 directories to DLL search path (Windows)
if sys.platform == 'win32':
    import site
    sp = next(p for p in site.getsitepackages() if 'site-packages' in p)
    for pkg in ('PySide6', 'shiboken6'):
        d = os.path.join(sp, pkg)
        if os.path.isdir(d):
            try:
                os.add_dll_directory(d)
            except (OSError, FileNotFoundError):
                pass

from . import saribbon
__version__ = "2.8.0"
