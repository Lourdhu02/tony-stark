#!/usr/bin/env python3
"""build.py: assemble the MkDocs source tree (site/_src) from the repository.

The repo is written for GitHub first. This script adapts it for the docs site:

  - copies every Markdown file (plus assets/) into site/_src, preserving layout
  - <details><summary>…</summary>…</details>  →  collapsible admonitions
  - $`…`$ (GitHub's escaping form of inline math)  →  $…$
  - links to folders  →  folder/README.md
  - links to source files (.c, .py, Makefile, …)  →  GitHub blob URLs

    python3 site/build.py && mkdocs build --strict -f site/mkdocs.yml
"""

import os
import pathlib
import re
import shutil

ROOT = pathlib.Path(__file__).resolve().parents[1]
SRC = ROOT / "site" / "_src"
GITHUB = "https://github.com/Lourdhu02/tony-stark"
SKIP = {"site", ".git", ".github", "node_modules", "build", "__pycache__"}

LINK = re.compile(r'(\]\(|href="|src=")([^)"\s]+)')
DETAILS = re.compile(r"<details><summary>(.*?)</summary>\n(.*?)\n</details>", re.S)
TICK_MATH = re.compile(r"\$`([^`]+)`\$")


def included(path: pathlib.Path) -> bool:
    return not any(part in SKIP or (part.startswith(".") and part != ".claude") for part in path.relative_to(ROOT).parts)


def site_path(repo_path: pathlib.Path) -> pathlib.Path:
    """Where a repo file lands in the site tree (.claude/ → claude/, since MkDocs skips dot-dirs)."""
    rel = repo_path.relative_to(ROOT)
    parts = ["claude" if p == ".claude" else p for p in rel.parts]
    return SRC.joinpath(*parts)


def rewrite_link(target: str, md_file: pathlib.Path) -> str:
    if re.match(r"^(https?:|mailto:|#|data:)", target):
        return target
    path, _, anchor = target.partition("#")
    anchor = "#" + anchor if anchor else ""
    resolved = (md_file.parent / path).resolve()
    try:
        rel = resolved.relative_to(ROOT)
    except ValueError:
        return target
    if resolved.is_dir():
        if (resolved / "README.md").exists():
            return (path.rstrip("/") + "/README.md" if path else "README.md") + anchor
        return f"{GITHUB}/tree/main/{rel.as_posix()}{anchor}"
    if resolved.suffix == ".md" and resolved.exists() and included(resolved):
        if ".claude" in rel.parts or ".claude" in md_file.relative_to(ROOT).parts:
            return os.path.relpath(site_path(resolved), site_path(md_file).parent) + anchor
        return target
    if resolved.exists() and rel.parts[0] == "assets":
        return target
    if resolved.exists():
        return f"{GITHUB}/blob/main/{rel.as_posix()}{anchor}"
    return target


def details_to_admonition(match: re.Match) -> str:
    title, body = match.group(1), match.group(2)
    title = re.sub(r"<code>(.*?)</code>", r"`\1`", title)
    title = re.sub(r"<b>(.*?)</b>", r"\1", title).replace('"', "”").strip()
    kind = "question" if re.match(r"\d+\.", title) else "tip"
    indented = "\n".join(("    " + line) if line.strip() else "" for line in body.strip("\n").split("\n"))
    return f'??? {kind} "{title}"\n\n{indented}\n'


LIST_ITEM = re.compile(r"^\s*([-*+]|\d+\.)\s")


def separate_lists(text: str) -> str:
    """GitHub renders a list right after a paragraph line; Python-Markdown needs a blank line first."""
    out, fence = [], False
    for line in text.split("\n"):
        if line.lstrip().startswith("```"):
            fence = not fence
        prev = out[-1] if out else ""
        if (not fence and LIST_ITEM.match(line) and prev.strip()
                and not LIST_ITEM.match(prev) and not prev.startswith((" ", "\t", "|", "#", ">"))):
            out.append("")
        out.append(line)
    return "\n".join(out)


def transform(text: str, md_file: pathlib.Path) -> str:
    text = separate_lists(text)
    text = DETAILS.sub(details_to_admonition, text)
    text = TICK_MATH.sub(r"$\1$", text)
    return LINK.sub(lambda m: m.group(1) + rewrite_link(m.group(2), md_file), text)


def main():
    if SRC.exists():
        shutil.rmtree(SRC)
    count = 0
    for md in sorted(ROOT.rglob("*.md")):
        if not included(md):
            continue
        out = site_path(md)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(transform(md.read_text(), md))
        count += 1
    shutil.copytree(ROOT / "assets", SRC / "assets")
    (SRC / "stylesheets").mkdir(exist_ok=True)
    shutil.copy(ROOT / "site" / "extra.css", SRC / "stylesheets" / "extra.css")
    (SRC / "javascripts").mkdir(exist_ok=True)
    shutil.copy(ROOT / "site" / "mathjax.js", SRC / "javascripts" / "mathjax.js")
    print(f"site/_src: {count} pages")


if __name__ == "__main__":
    main()
