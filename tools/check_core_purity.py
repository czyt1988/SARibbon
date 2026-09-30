#!/usr/bin/env python3
"""Core purity gate for SARibbon 3.0 (plan-01 S9 / v2 section 6.2).

Scans src/core (or any directory) for QtWidgets/QtQuick contamination:

* module layer (hard gate): #include of QWidget, QApplication, QStyle,
  QStyledItemDelegate, any Q*Layout class, QQuickItem, the QQml family,
  QAction (Qt5 QtWidgets), plus modular include paths <QtWidgets/...> and
  <QtQuick/...>. Prefix matching: 'QApplication' also catches
  QApplicationStyle hints; 'QQuickItem' catches derived header names.
* lexical layer: bare type usages qApp / QApplication:: / QWidget / QLayout
  with word boundaries (over-matching is preferred over under-matching).

Explicitly legal QtGui names that must NOT be flagged: QGuiApplication,
QScreen, QFontMetrics, QColor, QIcon (they do not share a forbidden prefix).

Exit code 1 on the first violation report (all violations are printed).
Standard library only (re/os/sys/argparse/pathlib).
"""
import argparse
import os
import re
import sys

DEFAULT_FORBID_INCLUDE = [
    "QWidget",
    "QApplication",
    "QStyle",
    "QStyledItemDelegate",
    "QLayout",
    "QAction",
    "QQuickItem",
    "QQml",
    "<QtWidgets/",
    "<QtQuick/",
]

# QLayout family regex: QLayout itself plus QGridLayout/QVBoxLayout/QBoxLayout/
# QFormLayout/QStackedLayout/... (v2 section 3.7 "QLayout and the Q*Layout family")
LAYOUT_FAMILY = re.compile(r"^Q[A-Za-z]*Layout$")

FORBID_LEXICAL = [
    re.compile(r"\bqApp\b"),
    re.compile(r"\bQApplication::"),
    re.compile(r"\bQWidget\b"),
    re.compile(r"\bQLayout\b"),
]

INCLUDE_RE = re.compile(r'^\s*#\s*include\s*(?:<([^>]+)>|"([^"]+)")')

SCAN_EXTS = (".h", ".hpp", ".cpp")


def check_include_target(target: str, forbidden: list) -> str | None:
    """Return the violated rule if the include target is forbidden."""
    for rule in forbidden:
        if rule.startswith("<"):
            if target.startswith(rule[1:]):
                return rule
        else:
            # bare class-name / prefix match on the file base name
            base = os.path.basename(target)
            if base.startswith(rule):
                return rule
            if LAYOUT_FAMILY.match(base):
                return f"{base} (Q*Layout family)"
    return None


def scan_file(path: str, forbidden: list) -> list:
    violations = []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for lineno, line in enumerate(f, 1):
            m = INCLUDE_RE.match(line)
            if m:
                target = m.group(1) or m.group(2)
                hit = check_include_target(target, forbidden)
                if hit:
                    violations.append(
                        f"{path}:{lineno}: forbidden include <{target}> (rule: {hit})"
                    )
                    continue
            for rx in FORBID_LEXICAL:
                if rx.search(line):
                    violations.append(
                        f"{path}:{lineno}: forbidden token /{rx.pattern}/ in: {line.strip()[:100]}"
                    )
    return violations


def main() -> int:
    parser = argparse.ArgumentParser(description="SARibbon core purity scanner")
    parser.add_argument("directory", help="directory to scan (e.g. src/core)")
    parser.add_argument(
        "--forbid-include",
        nargs="*",
        default=DEFAULT_FORBID_INCLUDE,
        help="include rules (prefix or <Mod/ match), default: core module list",
    )
    args = parser.parse_args()

    root = args.directory
    if not os.path.isdir(root):
        print(f"error: {root} is not a directory", file=sys.stderr)
        return 2

    violations = []
    for dirpath, _dirnames, filenames in os.walk(root):
        for name in sorted(filenames):
            if name.endswith(SCAN_EXTS):
                violations.extend(scan_file(os.path.join(dirpath, name), args.forbid_include))

    if violations:
        print(f"core purity check FAILED: {len(violations)} violation(s)")
        for v in violations:
            print(v)
        return 1
    print(f"core purity check passed: {root} is clean")
    return 0


if __name__ == "__main__":
    sys.exit(main())
