import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

INPUTS = [
    ROOT / "build/app/CM7/compile_commands.json",
    ROOT / "build/app/CM4/compile_commands.json",
]

OUTPUT = ROOT / "build" / "compile_commands.json"

merged = []

for path in INPUTS:
    if not path.exists():
        raise SystemExit(f"没有找到 {path}")

    with path.open("r", encoding="utf-8") as file:
        merged.extend(json.load(file))

with OUTPUT.open("w", encoding="utf-8") as file:
    json.dump(merged, file, indent=2, ensure_ascii=False)

print(f"已生成 {OUTPUT}")
print(f"共合并 {len(merged)} 条编译命令")