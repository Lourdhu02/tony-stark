# Runs this system's tests against the shared `blueprint` package in ../lab.
PYTHON ?= python3

all:
	@$(PYTHON) -c "import numpy, pytest" 2>/dev/null || { echo "missing deps: pip install -r ../lab/requirements.txt"; exit 1; }

test: all
	@$(PYTHON) -m pytest tests

clean:
	@find . ../lab -name __pycache__ -type d -exec rm -rf {} +

.PHONY: all test clean
