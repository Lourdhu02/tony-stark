"""Shared pytest setup for every Mark XI system (given code).

- makes `import blueprint` work from any system folder
- prints the progress bar and the machine-readable RESULT line read by tools/armory.sh
"""

import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).parent / "lab"))


def pytest_terminal_summary(terminalreporter):
    stats = terminalreporter.stats
    passed = len(stats.get("passed", []))
    total = passed + len(stats.get("failed", [])) + len(stats.get("error", []))
    width = 24
    filled = passed * width // total if total else 0
    suite = pathlib.Path(str(terminalreporter.config.args[0])).resolve().parent.name if terminalreporter.config.args else "tests"
    terminalreporter.write_line("")
    terminalreporter.write_line(f"  {suite:<12} {'█' * filled}{'░' * (width - filled)} {passed}/{total}")
    terminalreporter.write_line(f"RESULT {passed} {total}")
